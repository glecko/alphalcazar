#pragma once

#include "game/aliases.hpp"
#include "minmax/minmax_aliases.hpp"

#include <vector>
#include <tuple>

namespace Alphalcazar::Game {
	class Board;
	struct PlacementMove;
}

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief Calculates the symmetry axes of the board of a given board.
	 *
	 * A board can only have x-axis or y-axis symmetry, as pieces can only move along these axis. Pieces cannot
	 * have directions like south-west or north-east, making diagonal symmetries impossible.
	 *
	 * \returns A pair with 2 boolean values, the first being the x-axis symmetry of the board and the second the y-axis symmetry.
	 */
	std::pair<bool, bool> GetBoardSymmetries(const Game::Board& board);

	/*!
	 * \brief Filters a list of legal movements, erasing those that are duplicated once board
	 *        symmetries are taken into account.
	 * 
	 * This means that if several moves would cause the resulting board states to be identical with some symmetry
	 * (ex. x-axis symmetry), only one of those moves (the first one) is kept in the list.
	 * 
	 * \param legalMoves The list of legal moves to filter. Will potentially be modified.
	 * \param board The board of the game for which the legal movements are valid.
	 */
	void FilterSymmetricMovements(std::vector<Game::PlacementMove>& legalMoves, const Game::Board& board);

	/*!
	 * \brief Sorts a list of legal movements by the heuristic score we expect to obtain from playing
	 *        each of those movements.
	 * 
	 * Alpha-beta-pruning algorithms are most efficient when the best moves are explored first. Since this is
	 * not possible until the branch is actually explore, we use this heuristic to try to approximate it.
	 * 
	 * \param legalMoves The list of legal moves to sort. Will potentially be modified.
	 * \param board The board of the game for which the legal movements are valid.
	 */
	void SortLegalMovements(std::vector<Game::PlacementMove>& legalMoves, const Game::Board& board);
}
