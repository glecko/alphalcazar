#pragma once

#include "minmax/minmax_aliases.hpp"

#include <game/Strategy.hpp>
#include <game/aliases.hpp>

#include <future>

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
		virtual Game::PlacementMove Execute(Game::PlayerId playerId, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game) override;
		Score GetLastExecutedMoveScore() const;
	private:
		Score Max(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);
		Score Min(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);
		Score GetNextBestScore(Game::PlayerId playerId, const Game::PlacementMove& move, Depth depth, const Game::Game& game, Score alpha, Score beta);

		Depth mDepth;
		Score mLastExecutedMoveScore = 0;
	};
}
