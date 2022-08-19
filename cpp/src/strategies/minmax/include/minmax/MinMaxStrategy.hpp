#pragma once

#include "minmax/minmax_aliases.hpp"

#include <game/Strategy.hpp>
#include <game/aliases.hpp>

#include <atomic>
#include <memory>

namespace Alphalcazar::Game {
	struct PlacementMove;
	class Game;
}

namespace Alphalcazar::Utils {
	class ThreadPool;
}

namespace Alphalcazar::Strategy::MinMax {
	class TranspositionCache;

	/*!
	 * \brief A strategy that determines the move to play by using a min-max algorithm
	 *        on the available legal moves.
	 *
	 * Like a min-max chess engine, it is limited to a certain depth (due to the computational complexity
	 * of running this algorithm on this kind of game) and uses heuristics to evaluate the board score
	 * when the max depth is reached.
	 */
	class MinMaxStrategy final : public Game::Strategy {
	public:
		MinMaxStrategy(const Depth depth, bool multithreaded = true);
		~MinMaxStrategy();

		virtual Game::PlacementMove Execute(Game::PlayerId playerId, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game) override;

		/// Returns the score calculated for the move returned by the last \ref Execute function call
		Score GetLastExecutedMoveScore() const;
	private:
		/*!
		 * \brief Explores all possible branches (each being a legal move available to the active player) and returns
		 *        the score for the best available move.
		 */
		Score Max(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);

		/*!
		 * \brief Explores all possible branches (each being a legal move available to the opponent) and returns
		 *        the score for the best available move (from the opponent's perspective).
		 */
		Score Min(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);

		Score GetNextBestScore(Game::PlayerId playerId, const Game::PlacementMove& move, Depth depth, const Game::Game& game, Score alpha, Score beta);

		std::unique_ptr<TranspositionCache> mTranspositionCache;

		/// The thread pool that will run the min-max algorithm tasks if mMultithreaded is true
		std::unique_ptr<Utils::ThreadPool> mThreadPool;
		std::atomic<Score> mFirstLevelAlpha = 0;

		/// The score calculated for the move returned by the last \ref Execute function call
		Score mLastExecutedMoveScore = 0;
		/// The max depth to explore on min-max searches
		Depth mDepth;
		/// Whether the min-max search will be run on multiple threads
		bool mMultithreaded;
	};
}
