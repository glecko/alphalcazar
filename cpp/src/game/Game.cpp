#include "Game.hpp"
#include "Board.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "PlacementMove.hpp"
#include "parameters.hpp"
#include "Strategy.hpp"
#include "Piece.hpp"

namespace Alphalcazar::Game {
	GameState::GameState() {}

	GameState::GameState(const GameState& other)
		: Turn{ other.Turn }
		, PlayerWithInitiative{ other.PlayerWithInitiative }
		, FirstMoveExecuted{ other.FirstMoveExecuted }
	{}

	void GameState::SwapPlayerWithInitiative() {
		PlayerWithInitiative = PlayerWithInitiative == PlayerId::PLAYER_ONE ? PlayerId::PLAYER_TWO : PlayerId::PLAYER_ONE;
	}

	Game::Game() {
		mBoard = std::make_unique<Board>();
		mPlayerOne = std::make_unique<Player>(PlayerId::PLAYER_ONE);
		mPlayerTwo = std::make_unique<Player>(PlayerId::PLAYER_TWO);
	}

	Game::Game(const Game& other)
		: mState { other.mState }
	{
		mPlayerOne = std::make_unique<Player>(*other.mPlayerOne);
		mPlayerTwo = std::make_unique<Player>(*other.mPlayerTwo);

		// Fetch a combined vector of all player pieces
		auto pieces = GetAllPieces();

		mBoard = std::make_unique<Board>(*other.mBoard, pieces);
	}

	Game::~Game() {};

	GameResult Game::Play(Strategy& firstPlayerStrategy, Strategy& secondPlayerStrategy) {
		GameResult result = GameResult::NONE;
		while (result == GameResult::NONE) {
			result = PlayTurn(firstPlayerStrategy, secondPlayerStrategy);
		}
		return result;
	}

	GameResult Game::PlayTurn(Strategy& firstPlayerStrategy, Strategy& secondPlayerStrategy) {
		ExecutePlayerMoves(firstPlayerStrategy, secondPlayerStrategy);
		return EvaluateTurnEndPhase();
	}

	GameResult Game::PlayNextPlacementMove(const PlacementMove& move) {
		auto* player = GetActivePlayer();
		auto result = GameResult::NONE;
		ExecutePlacementMove(player->GetId(), move);
		if (mState.FirstMoveExecuted) {
			result = EvaluateTurnEndPhase();
		}
		mState.FirstMoveExecuted = !mState.FirstMoveExecuted;
		return result;
	}

	GameResult Game::EvaluateTurnEndPhase() {
		auto executedMoves = mBoard->ExecuteMoves(mState.PlayerWithInitiative);
		mState.Turn += 1;
		mState.SwapPlayerWithInitiative();
		return EvaluateGameResult(executedMoves);
	}

	bool Game::IsStalemate(BoardMovesCount executedMoves) const {
		// Only check for stalemates if no moves were executed and the board is full
		// There is no possible stalemate scenario without these conditions
		if (executedMoves == 0 && mBoard->IsFull()) {
			// Check that neither player have a pushing piece in their hand
			// as a true stalemate cannot happen if a player can still place a pushing piece
			if (!mPlayerOne->GetPiece(c_PusherPieceType)->IsInPlay() && !mPlayerTwo->GetPiece(c_PusherPieceType)->IsInPlay()) {
				return true;
			}
		}
		return false;
	}

	void Game::ExecutePlayerMoves(Strategy& firstPlayerStrategy, Strategy& secondPlayerStrategy) {
		// At the beginning of the turn, we reset the "first move executed" state
		mState.FirstMoveExecuted = false;

		// Play the first move for the player with initiative
		if (mState.PlayerWithInitiative == PlayerId::PLAYER_ONE) {
			ExecutePlayerMove(PlayerId::PLAYER_ONE, firstPlayerStrategy);
		} else {
			ExecutePlayerMove(PlayerId::PLAYER_TWO, secondPlayerStrategy);
		}

		mState.FirstMoveExecuted = true;

		// Play the second move for the player without initiative
		if (mState.PlayerWithInitiative == PlayerId::PLAYER_ONE) {
			ExecutePlayerMove(PlayerId::PLAYER_TWO, firstPlayerStrategy);
		} else {
			ExecutePlayerMove(PlayerId::PLAYER_ONE, secondPlayerStrategy);
		}

		mPlayerMovesExecutedCallbacks.Invoke();
	}

	void Game::ExecutePlayerMove(PlayerId playerId, Strategy& strategy) {
		auto legalMoves = GetLegalMoves(playerId);
		// If a player has no available legal moves, their turn is skipped
		if (legalMoves.size() > 0) {
			PlacementMoveIndex placementMoveIndex = strategy.Execute(playerId, legalMoves, *this);
			if (placementMoveIndex < 0 || placementMoveIndex >= legalMoves.size()) {
				throw "Invalid legal move index returned by player strategy";
			}
			auto& placementMove = legalMoves[placementMoveIndex];
			ExecutePlacementMove(playerId, placementMove);
		}
	}

	void Game::ExecutePlacementMove(PlayerId playerId, const PlacementMove& move) {
		auto* piece = GetPlayer(playerId)->GetPiece(move.PieceType);
		mBoard->PlacePiece(move.Coordinates, piece);
	}

	std::vector<PlacementMove> Game::GetLegalMoves(PlayerId player) const {
		std::vector<PlacementMove> result;
		auto legalTiles = mBoard->GetLegalPlacementTiles();
		auto pieces = GetPlayer(player)->GetPiecesInHand();
		result.reserve(pieces.size() * legalTiles.size());
		for (auto* tile : legalTiles) {
			for (auto* piece : pieces) {
				result.push_back({ tile->GetCoordinates(), piece->GetType() });
			}
		}
		return result;
	}

	GameResult Game::EvaluateGameResult(BoardMovesCount executedMoves) {
		if (IsStalemate(executedMoves)) {
			return GameResult::DRAW;
		}
		return mBoard->GetResult();
	}

	Player* Game::GetPlayer(PlayerId playerId) const {
		switch (playerId) {
		case PlayerId::PLAYER_ONE:
			return mPlayerOne.get();
		case PlayerId::PLAYER_TWO:
			return mPlayerTwo.get();
		default:
			return nullptr;
		}
	}

	std::vector<Piece*> Game::GetAllPieces() const {
		std::vector<Piece*> result = GetPlayer(PlayerId::PLAYER_ONE)->GetPieces();
		auto playerTwoPieces = GetPlayer(PlayerId::PLAYER_TWO)->GetPieces();

		result.reserve(result.size() + playerTwoPieces.size());
		result.insert(result.end(), playerTwoPieces.begin(), playerTwoPieces.end());

		return result;
	}

	Player* Game::GetActivePlayer() const {
		return GetPlayerByInitiative(!mState.FirstMoveExecuted);
	}


	Player* Game::GetPlayerByInitiative(bool initiative) const {
		if (initiative) {
			return GetPlayer(mState.PlayerWithInitiative);
		} else {
			if (mState.PlayerWithInitiative == PlayerId::PLAYER_ONE) {
				return GetPlayer(PlayerId::PLAYER_TWO);
			} else {
				return GetPlayer(PlayerId::PLAYER_ONE);
			}
		}
	}

	GameState& Game::GetState() {
		return mState;
	}

	const GameState& Game::GetState() const {
		return mState;
	}

	Board& Game::GetBoard() {
		return *mBoard;
	}

	const Board& Game::GetBoard() const {
		return *mBoard;
	}

	Utils::CallbackHandler<Game::PlayerMovesExecutedCallback>& Game::OnPlayerMovesExecuted() {
		return mPlayerMovesExecutedCallbacks;
	}
}
