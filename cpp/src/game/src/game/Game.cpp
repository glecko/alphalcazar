#include "game/Game.hpp"

#include "game/PlacementMove.hpp"
#include "game/parameters.hpp"
#include "game/Strategy.hpp"
#include "game/Piece.hpp"

namespace Alphalcazar::Game {
	Game::Game() = default;

	Game::~Game() = default;

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
		const PlayerId activePlayer = GetActivePlayer();
		auto result = GameResult::NONE;
		ExecutePlacementMove(activePlayer, move);
		if (mState.FirstMoveExecuted) {
			result = EvaluateTurnEndPhase();
		} else {
			mState.FirstMoveExecuted = true;
		}
		return result;
	}

	GameResult Game::EvaluateTurnEndPhase() {
		const auto executedMoves = mBoard.ExecuteMoves(mState.PlayerWithInitiative);
		mState.Turn += 1;
		mState.FirstMoveExecuted = false;
		SwapPlayerWithInitiative();
		return EvaluateGameResult(executedMoves);
	}

	void Game::ExecutePlayerMoves(Strategy& firstPlayerStrategy, Strategy& secondPlayerStrategy) {
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
	}

	void Game::ExecutePlayerMove(PlayerId playerId, Strategy& strategy) {
		const auto legalMoves = GetLegalMoves(playerId);
		// If a player has no available legal moves, their turn is skipped
		if (legalMoves.size() > 0) {
			const auto placementMove = strategy.Execute(playerId, legalMoves, *this);
			ExecutePlacementMove(playerId, placementMove);
		}
	}

	void Game::ExecutePlacementMove(PlayerId playerId, const PlacementMove& move) {
		const Piece piece = { playerId, move.PieceType };
		mBoard.PlacePiece(move.Coordinates, piece);
	}

	Utils::StaticVector<Piece, c_PieceTypes> Game::GetPiecesInHand(PlayerId player) const {
		Utils::StaticVector<Piece, c_PieceTypes> result;
		if (player == PlayerId::NONE) {
			return result;
		}

		const auto piecePlacements = mBoard.GetPiecePlacements(player);
		const std::size_t placedPiecesCount = piecePlacements.count();
		// If all pieces are on the board, immediatelly return an empty vector
		if (placedPiecesCount != c_PieceTypes) {
			for (PieceType type = 1; type <= c_PieceTypes; type++) {
				// Check if the piece type is not placed by the player on the board
				if (!piecePlacements[type - 1]) {
					result.insert({ player, type });
				}
			}
		}
		return result;
	}

	Utils::StaticVector<PlacementMove, c_MaxLegalMovesCount> Game::GetLegalMoves(PlayerId player) const {
		Utils::StaticVector<PlacementMove, c_PieceTypes* c_PerimeterTileCount> result;
		const auto legalCoordinates = mBoard.GetLegalPlacementCoordinates();
		const auto pieces = GetPiecesInHand(player);
		for (const Coordinates& coordinates : legalCoordinates) {
			for (const Piece& piece : pieces) {
				result.insert({ coordinates, piece.GetType() });
			}
		}
		return result;
	}

	GameResult Game::EvaluateGameResult(BoardMovesCount) const {
		return mBoard.GetResult();
	}

	PlayerId Game::GetActivePlayer() const {
		return GetPlayerByInitiative(!mState.FirstMoveExecuted);
	}

	PlayerId Game::GetPlayerByInitiative(bool initiative) const {
		if (initiative) {
			return mState.PlayerWithInitiative;
		}
		if (mState.PlayerWithInitiative == PlayerId::PLAYER_ONE) {
			return PlayerId::PLAYER_TWO;
		}
		return PlayerId::PLAYER_ONE;
	}

	void Game::SwapPlayerWithInitiative() {
		mState.PlayerWithInitiative = mState.PlayerWithInitiative == PlayerId::PLAYER_ONE ? PlayerId::PLAYER_TWO : PlayerId::PLAYER_ONE;
	}

	GameState& Game::GetState() {
		return mState;
	}

	const GameState& Game::GetState() const {
		return mState;
	}

	Board& Game::GetBoard() {
		return mBoard;
	}

	const Board& Game::GetBoard() const {
		return mBoard;
	}
}
