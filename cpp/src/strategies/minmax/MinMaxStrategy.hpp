#pragma once

#include "minmax_aliases.hpp"

#include <Strategy.hpp>
#include <aliases.hpp>

#include <future>
#include <util/ThreadPool.hpp>

namespace Alphalcazar::Game {
	struct PlacementMove;
	class Game;
}

namespace Alphalcazar::Strategy::MinMax {

	/*!
	 * \brief 
	 */
	class MinMaxStrategy final : public Game::Strategy {
	public:
		MinMaxStrategy(const Depth depth);
		virtual Game::PlacementMoveIndex Execute(Game::PlayerId playerId, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game) override;
		Score GetLastExecutedMoveScore() const;
	private:
		Score Max(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);
		Score Min(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);
		Score GetNextBestScore(Game::PlayerId playerId, const Game::PlacementMove& move, Depth depth, const Game::Game& game, Score alpha, Score beta);
		Score GetNextBestScoreAsync(Game::PlayerId playerId, const Game::PlacementMove& move, Depth depth, const Game::Game& game);
		Score GetFirstLevelAlpha();
		void SetFirstLevelAlpha(Score alpha);

		Depth mDepth;
		Score mLastExecutedMoveScore = 0;
		/*!
		 * \brief The value of alpha at the first level of branching (where the branches are sepparated into different threads).
		 * 
		 * Since the different branches will be explored in parallel, each thread needs to make sure to atomically update this alpha value
		 * so that alpha-pruning can take place in the parallel running branches.
		 */
		Score mFirstLevelAlpha;
		Utils::ThreadPool mThreadPool;
	};
}
