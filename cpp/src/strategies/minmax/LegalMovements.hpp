#pragma once

#include "aliases.hpp"
#include "minmax_aliases.hpp"

#include <vector>
#include <tuple>

namespace Alphalcazar::Game {
	class Game;
	struct PlacementMove;
}

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief Calculates the symmetry axes of the board of a given game.
	 * 
	 * A board of this game can only have x-axis or y-axis symmetry, as pieces can only move along these axis. Pieces cannot
	 * have directions like south-west or north-east, making diagonal symmetries impossible.
	 * 
	 * \returns A pair with 2 boolean values, the first being the x-axis symmetry of the board and the second the y-axis symmetry.
	 */
	std::pair<bool, bool> GetBoardSymmetries(const Game::Game& game);
	void FilterSymmetricMovements(std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game);
}
