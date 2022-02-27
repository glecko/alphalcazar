#pragma once

#include "aliases.hpp"
#include <unordered_map>

namespace Alphalcazar::Game {
	/*!
	 * \brief Helper class to work with the 2D coordinate system of the tiles of the board.
	 * 
	 * Represents the coordinates of a (potential) tile position on the board. The coordinate system of the board
	 * has its origin (0, 0) at the south-west (bottom-left) most corner of the board. X coordinates increase to the east
	 * and Y coordinates increase to the north.
	 */
	struct Coordinates {
		Coordinate x, y;
		Coordinates(Coordinate x, Coordinate y);
		~Coordinates();

		bool operator==(const Coordinates& coord) const;

		/// Indicates if the coordinates represent a position in the perimeter of the board
		bool IsPerimeter() const;
		/// Indicates if the coordinates represent a corner of the board. No tile will exist at these coordinates.
		bool IsCorner() const;

		/// Returns a new Coordinates object representing a movement at a fixed distance in the specified direction
		Coordinates GetCoordinateInDirection(Direction direction, Coordinate distance) const;

		/// Returns whether the current coordinates are valid
		bool Valid() const;

		/// Builds and returns an instance of invalid coordinates
		static Coordinates Invalid();
	};
}

namespace std {
	// The \ref Coordinates class implements a hashing function to be able to be used as a key
	// for ordered/unordered maps.
	template <> struct hash<Alphalcazar::Game::Coordinates> {
		std::size_t operator()(const Alphalcazar::Game::Coordinates& coordinates) const noexcept {
			return ((hash<Alphalcazar::Game::Coordinate>()(coordinates.x) ^ (hash<Alphalcazar::Game::Coordinate>()(coordinates.y) << 1)) >> 1);
		}
	};
}
