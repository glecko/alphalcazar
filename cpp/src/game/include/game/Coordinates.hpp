#pragma once

#include "aliases.hpp"
#include "game/parameters.hpp"
#include <fmt/format.h>
#include <array>

namespace Alphalcazar::Game {
	constexpr Coordinate c_InvalidCoordinate = std::numeric_limits<Coordinate>::max();

	/*!
	 * \brief Helper class to work with the 2D coordinate system of the tiles of the board.
	 *
	 * Represents the coordinates of a (potential) tile position on the board. The coordinate system of the board
	 * has its origin (0, 0) at the south-west (bottom-left) most corner of the board. X coordinates increase to the east
	 * and Y coordinates increase to the north.
	 */
	struct Coordinates {
		Coordinate x = c_InvalidCoordinate;
		Coordinate y = c_InvalidCoordinate;

		bool operator==(const Coordinates& coord) const;
		bool operator!=(const Coordinates& coord) const;

		/// Indicates if the coordinates represent a position in the perimeter of the board
		bool IsPerimeter() const;
		/// Indicates if the coordinates represent a corner of the play area. No tile will exist at these coordinates.
		bool IsCorner() const;
		/// Indicates if the coordinates represent a valid play area tile.
		bool IsPlayArea() const;
		/// Indicates if the coordinates represent the center of the board
		bool IsCenter() const;
		/// Indicates if the coordinates represent a tile that is on either the row or column that contains the center tile of the board
		bool IsOnCenterLane() const;
		/// Indicates if the coordinates represent a corner of the board
		bool IsBoardCorner() const;

		/*!
		 * \brief Returns a new Coordinates object representing a movement at a fixed distance in the specified direction
		 *
		 * Returns invalid coordinates if the parameters are invalid (ex. invalid direction).
		 */
		Coordinates GetCoordinateInDirection(Direction direction, Coordinate distance) const;

		/*!
		 * \brief Returns the direction in which pieces may be legally placed on a tile with these coordinates.
		 *
		 * Returns an invalid direction if the tile is not a perimeter tile.
		 */
		Direction GetLegalPlacementDirection() const;

		/// Returns whether the current coordinates are valid
		bool Valid() const;

		/// Builds and returns an instance of invalid coordinates
		static Coordinates Invalid();

		/// Returns a list of all coordinates that correspond to valid perimeter tiles
		static constexpr std::array<Coordinates, c_PerimeterTileCount> GetPerimeterCoordinates() {
			std::array<Coordinates, c_PerimeterTileCount> result{};
			std::size_t i = 0;
			for (Coordinate x = 1; x <= c_BoardSize; x++) {
				result[i] = { x, 0 };
				i++;
			}
			for (Coordinate x = 1; x <= c_BoardSize; x++) {
				result[i] = { x, c_PlayAreaSize - 1 };
				i++;
			}
			for (Coordinate y = 1; y <= c_BoardSize; y++) {
				result[i] = { 0, y };
				i++;
			}
			for (Coordinate y = 1; y <= c_BoardSize; y++) {
				result[i] = { c_PlayAreaSize - 1, y };
				i++;
			}
			return result;
		}
	};
}

namespace std {
	/*
	* The \ref Coordinates class implements a hashing function to be able to be used as a key
	* for unordered maps.
	*/
	template <>
	struct hash<Alphalcazar::Game::Coordinates> {
		std::size_t operator()(const Alphalcazar::Game::Coordinates& coordinates) const noexcept {
			auto xHash = hash<Alphalcazar::Game::Coordinate>()(coordinates.x);
			auto yHash = hash<Alphalcazar::Game::Coordinate>()(coordinates.y);
			return ((xHash ^ (yHash << 1)) >> 1);
		}
	};
}

// We define the specializations of parse & format to make \ref Coordinates formattable. Based on: https://fmt.dev/latest/api.html#format-api
template <> struct fmt::formatter<Alphalcazar::Game::Coordinates> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const Alphalcazar::Game::Coordinates& coordinates, FormatContext& ctx) -> decltype(ctx.out()) {
		return format_to(ctx.out(), "({},{})", coordinates.x, coordinates.y);
	}
};
