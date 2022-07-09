#include "minmax/MinMaxStrategy.hpp"

#include "minmax/BoardEvaluation.hpp"
#include "minmax/LegalMovements.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>
#include <game/PlacementMove.hpp>
#include <util/Log.hpp>

namespace Alphalcazar::Strategy::MinMax {
	/// The initial value of the "alpha" parameter of the minmax algorithm
	constexpr Score c_AlphaStartingValue = -c_WinConditionScore * 10;
	/// The initial value of the "beta" parameter of the minmax algorithm
	constexpr Score c_BetaStartingValue = c_WinConditionScore * 10;

	MinMaxStrategy::MinMaxStrategy(const Depth depth)
		: mDepth { depth }
	{}

	Game::PlacementMove MinMaxStrategy::Execute(Game::PlayerId playerId, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game) {
		Score bestScore = c_AlphaStartingValue;
		Score alpha = c_AlphaStartingValue;
		auto filteredMoves = legalMoves;
		FilterSymmetricMovements(filteredMoves, game.GetBoard());
		SortLegalMovements(filteredMoves, game.GetBoard());

		assert(!legalMoves.empty());
		Game::PlacementMove& bestMove = filteredMoves[0];
		for (const auto& move : filteredMoves) {
			auto moveScore = GetNextBestScore(playerId, move, mDepth, game, alpha, c_BetaStartingValue);
			if (moveScore > bestScore) {
				bestScore = moveScore;
				bestMove = move;
			}
		}

		mLastExecutedMoveScore = bestScore;
		Utils::LogDebug("Played {} with score {}.", bestMove, bestScore);
		return bestMove;
	}

	Score MinMaxStrategy::Max(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta) {
		if (depth == 0) {
			return EvaluateBoard(playerId, game);
		}
		Score bestScore = c_AlphaStartingValue;
		// We are in "Max" so we are evaluating the player who is executing the strategy
		auto legalMoves = game.GetLegalMoves(playerId);
		FilterSymmetricMovements(legalMoves, game.GetBoard());
		SortLegalMovements(legalMoves, game.GetBoard());
		for (const auto& move : legalMoves) {
			auto nextBestScore = GetNextBestScore(playerId, move, depth, game, alpha, beta);

			bestScore = std::max(nextBestScore, bestScore);
			alpha = std::max(bestScore, alpha);
			if (alpha > beta) {
				break;
			}
		}
		return bestScore;
	}

	Score MinMaxStrategy::Min(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta) {
		if (depth == 0) {
			return EvaluateBoard(playerId, game);
		}
		Score bestScore = c_BetaStartingValue;
		// We are in "Min" so we are evaluating the opponent
		auto opponentId = playerId == Game::PlayerId::PLAYER_ONE ? Game::PlayerId::PLAYER_TWO : Game::PlayerId::PLAYER_ONE;
		auto legalMoves = game.GetLegalMoves(opponentId);
		FilterSymmetricMovements(legalMoves, game.GetBoard());
		SortLegalMovements(legalMoves, game.GetBoard());
		for (const auto& move : legalMoves) {
			auto nextBestScore = GetNextBestScore(playerId, move, depth, game, alpha, beta);
			bestScore = std::min(nextBestScore, bestScore);
			beta = std::min(bestScore, beta);
			if (beta < alpha) {
				break;
			}
		}
		return bestScore;
	}

	Score MinMaxStrategy::GetNextBestScore(Game::PlayerId playerId, const Game::PlacementMove& move, Depth depth, const Game::Game& game, Score alpha, Score beta) {
		Game::Game gameCopy = game;
		auto result = gameCopy.PlayNextPlacementMove(move);
		Score nextBestScore = 0;
		if (result != Game::GameResult::NONE) {
			nextBestScore = GameResultToScore(playerId, result);
		} else {
			// Only decrease the depth if this placement move completed a turn
			// as we want to evaluate complete turns only, never half a turn
			Depth nextDepth = gameCopy.GetState().FirstMoveExecuted ? depth : depth - 1;
			Game::PlayerId activePlayerId = gameCopy.GetActivePlayer();
			if (activePlayerId == playerId) {
				nextBestScore = Max(playerId, nextDepth, gameCopy, alpha, beta);
			} else {
				nextBestScore = Min(playerId, nextDepth, gameCopy, alpha, beta);
			}
			// If we decreased the depth when calculating the next move score
			// we add a depth penalty. Since this function is called recursively, we only
			// adjust for 1 depth level at a time.
			if (nextDepth < depth) {
				nextBestScore = GetDepthAdjustedScore(nextBestScore, 1);
			}
		}
		return nextBestScore;
	}

	Score MinMaxStrategy::GetLastExecutedMoveScore() const {
		return mLastExecutedMoveScore;
	}
}
