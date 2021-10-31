#include "Game.hpp"
#include "Board.hpp"
#include "Player.hpp"
#include "PlacementMove.hpp"
#include "Strategy.hpp"

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
		auto pieces = mPlayerOne->GetPieces();
		auto playerTwoPieces = mPlayerTwo->GetPieces();
		pieces.insert(pieces.end(), playerTwoPieces.begin(), playerTwoPieces.end());

		mBoard = std::make_unique<Board>(*other.mBoard, pieces);
	}

	Game::~Game() {};

	GameResult Game::Play(const Strategy& firstPlayerStrategy, const Strategy& secondPlayerStrategy) {
		GameResult result = GameResult::NONE;
		while (result == GameResult::NONE) {
			PlayTurn(firstPlayerStrategy, secondPlayerStrategy);
		}
		return result;
	}

	GameResult Game::PlayTurn(const Strategy& firstPlayerStrategy, const Strategy& secondPlayerStrategy) {
		ExecutePlayerMoves(firstPlayerStrategy, secondPlayerStrategy);
		auto executedMoves = mBoard->ExecuteMoves(mState.PlayerWithInitiative);
		mState.Turn += 1;
		mState.SwapPlayerWithInitiative();
		return EvaluateGameResult(executedMoves);
	}

	bool Game::IsStalemate(BoardMovesCount executedMoves) const {
		// Only check for stalemates if no moves were executed and the board is full
		// There is no possible stalemate scenario without these conditions
		if (executedMoves == 0 and mBoard->IsFull()) {
			// TO-DO: Check that no player has a pusher piece in hand!
			// Else it's not a true stalemate
			return true;
		}
		return false;
	}

	void Game::ExecutePlayerMoves(const Strategy& firstPlayerStrategy, const Strategy& secondPlayerStrategy) {
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

	void Game::ExecutePlayerMove(PlayerId playerId, const Strategy& strategy) {
		auto& placementMove = strategy.Execute(playerId, *this);
		if (placementMove.IsValid()) {
			auto* player = GetPlayer(placementMove.GetPlayerId());
			auto* piece = player->GetPiece(placementMove.GetPieceType());
			mBoard->PlacePiece(placementMove.GetCoordinates(), piece);
		}
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

	Utils::CallbackHandler<Game::PlayerMovesExecutedCallback>& Game::OnPlayerMovesExecuted() {
		return mPlayerMovesExecutedCallbacks;
	}
}
