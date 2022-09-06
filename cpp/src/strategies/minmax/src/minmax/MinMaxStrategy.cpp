#include "minmax/MinMaxStrategy.hpp"

#include "minmax/BoardEvaluation.hpp"
#include "minmax/LegalMovements.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>
#include <game/PlacementMove.hpp>
#include <util/Log.hpp>
#include "util/ThreadPool.hpp"

namespace Alphalcazar::Strategy::MinMax {
	/// The initial value of the "alpha" parameter of the minmax algorithm
	constexpr Score c_AlphaStartingValue = -c_WinConditionScore * 10;
	/// The initial value of the "beta" parameter of the minmax algorithm
	constexpr Score c_BetaStartingValue = c_WinConditionScore * 10;

	MinMaxStrategy::MinMaxStrategy(const Depth depth, bool multithreaded)
		: mDepth { depth }
		, mMultithreaded { multithreaded }
	{
		if (mMultithreaded) {
			/*
			 * Alpha-beta-pruning works best when all branches are calculated sequentially. However,
			 * we want to make use of all cores of the machine we are running on. To maximise alpha-beta-cutoffs while
			 * making sure we make the most use of the cores of our current hardware, we create a thread pool with as
			 * many threads as the maximum supported hardware concurrency (minus 1, for the main thread).
			 *
			 * We always want to have at least 1 worker thread to ensure that single-core machines still execute this strategy.
			 */
			std::size_t threadCount = std::max(std::thread::hardware_concurrency() - 1, 1U);
			mThreadPool = std::make_unique<Utils::ThreadPool>(threadCount);
		}
	}

	MinMaxStrategy::~MinMaxStrategy() {}

	Game::PlacementMove MinMaxStrategy::Execute(Game::PlayerId playerId, const Utils::StaticVector<Game::PlacementMove, Game::c_MaxLegalMovesCount>& legalMoves, const Game::Game& game) {
		auto candidateMoves =  SortAndFilterMovements(playerId, legalMoves, game.GetBoard());

		assert(!candidateMoves.empty());
		Score bestScore = c_AlphaStartingValue;
		std::size_t bestMoveIndex = 0;
		if (mMultithreaded) {
			mFirstLevelAlpha = c_AlphaStartingValue;

			using MoveFutureResult = std::pair<Score, Game::PlacementMove>;
			std::vector<std::future<MoveFutureResult>> moveFutures;
			moveFutures.reserve(candidateMoves.size());
			for (std::size_t i = 0; i < candidateMoves.size(); ++i) {
				const ScoredPlacementMove& move = candidateMoves[i];
				auto moveFuture = mThreadPool->Execute([this, playerId, move, game]() -> MoveFutureResult {
					auto moveScore = GetNextBestScore(playerId, move, mDepth, game, mFirstLevelAlpha, c_BetaStartingValue);
					mFirstLevelAlpha = std::max(mFirstLevelAlpha.load(), moveScore);
					return std::make_pair(moveScore, move);
				});
				moveFutures.emplace_back(std::move(moveFuture));
			}

			for (auto& moveFuture : moveFutures) {
				moveFuture.wait();
			}

			for (std::size_t i = 0; i < moveFutures.size(); i++) {
				auto& moveFuture = moveFutures[i];
				MoveFutureResult moveFutureResult = moveFuture.get();
				if (moveFutureResult.first > bestScore) {
					bestScore = moveFutureResult.first;
					bestMoveIndex = i;
				}
			}
		} else {
			Score alpha = c_AlphaStartingValue;
			for (std::size_t i = 0; i < candidateMoves.size(); ++i) {
				auto& move = candidateMoves[i];
				auto moveScore = GetNextBestScore(playerId, move, mDepth, game, alpha, c_BetaStartingValue);
				if (moveScore > bestScore) {
					bestScore = moveScore;
					alpha = moveScore;
					bestMoveIndex = i;
				}
			}
		}
		mLastExecutedMoveScore = bestScore;
		const auto& bestMove = candidateMoves[bestMoveIndex];
		Utils::LogDebug("Player {} played {} (idx {}/{}) with score {}.", static_cast<std::size_t>(playerId), bestMove, bestMoveIndex, candidateMoves.size(), bestScore);
		return bestMove;
	}

	Score MinMaxStrategy::Max(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta) {
		if (depth == 0) {
			return EvaluateBoard(playerId, game);
		}
		Score bestScore = c_AlphaStartingValue;
		// We are in "Max" so we are evaluating the player who is executing the strategy
		auto legalMoves = game.GetLegalMoves(playerId);
		auto candidateMoves = SortAndFilterMovements(playerId, legalMoves, game.GetBoard());
		for (std::size_t i = 0; i < candidateMoves.size(); ++i) {
			const auto& move = candidateMoves[i];
			auto nextBestScore = GetNextBestScore(playerId, move, depth, game, alpha, beta);

			bestScore = std::max(nextBestScore, bestScore);
			alpha = std::max(bestScore, alpha);
			if (mMultithreaded) {
				alpha = std::max(alpha, mFirstLevelAlpha.load());
			}

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
		auto candidateMoves = SortAndFilterMovements(playerId, legalMoves, game.GetBoard());
		for (std::size_t i = 0; i < candidateMoves.size(); ++i) {
			const auto& move = candidateMoves[i];
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
