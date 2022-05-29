#pragma once

#include "aliases.hpp"
#include "parameters.hpp"
#include <array>

namespace Alphalcazar::Game {
	/*!
	 * \brief The amount of rows that need to be checked for win conditions.
	 * 
	 * Its value consists of: Every individual row, every invidual column (2 times the board size)
	 * and the 2 diagonals of the board.
	 */
	constexpr std::size_t c_RowIterationDirectionsCount = 2 * c_BoardSize + 2;

	/// Data structure used to represent a row that can be iterated on
	struct RowIterationDirection {
		/// The starting x coordinate of the row
		Coordinate x;
		/// The starting y coordinate of the row
		Coordinate y;
		/// The direction that needs to be followed to reach the end of the row
		Direction Direction;
		/// The distance that needs to be iterated until the end of the row is reached
		Coordinate Length;
	};

	/*!
	 * \brief Returns an array of all row iteration directions that need to be checked for win conditions.
	 *
	 * Each row will be descibed as start coordinates, a direction and a distance. The row may then be fully iterated by starting at the
	 * start coordinate and iterating in its attached direction for as many tiles as specified in the distance.
	 */
	constexpr std::array<RowIterationDirection, c_RowIterationDirectionsCount> GetAllRowIterationDirections() {
		std::array<RowIterationDirection, c_RowIterationDirectionsCount> result{};

		// Add all columns by starting at their southmost tile of each and iterating north
		for (std::size_t x = 1; x <= c_BoardSize; x++) {
			result[x - 1] = { static_cast<Coordinate>(x), 1, Direction::NORTH, c_BoardSize };
		}

		// Add all rows by starting at the westmost tile of each and iterating east
		for (std::size_t y = 1; y <= c_BoardSize; y++) {
			result[c_BoardSize + y - 1] = { 1, static_cast<Coordinate>(y), Direction::EAST, c_BoardSize };
		}

		// Add both diagonals
		result[c_BoardSize * 2] = { 1, 1, Direction::NORTH_EAST, c_BoardSize };
		result[c_BoardSize * 2 + 1] = { c_BoardSize, 1, Direction::NORTH_WEST, c_BoardSize };

		return result;
	}
}
