#pragma once

#include "minmax_aliases.hpp"

#include <Strategy.hpp>
#include <aliases.hpp>

namespace Alphalcazar::Game {
	struct PlacementMove;
	class Game;
}

namespace Alphalcazar::Strategy::MinMax {

	/*!
	 * \brief 
	 */
	class MinMaxStrategy : public Game::Strategy {
	public:
		MinMaxStrategy(const Depth depth);
		virtual Game::PlacementMoveIndex Execute(Game::PlayerId playerId, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game) override;

		Score Max(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);
		Score Min(Game::PlayerId playerId, Depth depth, const Game::Game& game, Score alpha, Score beta);
	private:
		Score GetNextBestScore(Game::PlayerId playerId, const Game::PlacementMove& move, Depth depth, const Game::Game& game, Score alpha, Score beta);
		Depth mDepth;
	};
}
