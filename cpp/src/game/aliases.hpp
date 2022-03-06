#pragma once

#include <cstdint>

namespace Alphalcazar::Game {
	using BoardMovesCount = std::uint16_t;
	using PieceType = std::uint8_t;
	using Coordinate = std::int8_t; // (+-128 should be more than enough for a 3x3 board)
	using PlacementMoveIndex = std::uint8_t;

	enum class GameResult {
		NONE,
		DRAW,
		PLAYER_ONE_WINS,
		PLAYER_TWO_WINS,
	};

	enum class PlayerId {
		NONE,
		PLAYER_ONE,
		PLAYER_TWO
	};

	enum class Direction {
		NONE,
		NORTH,
		SOUTH,
		EAST,
		WEST,
		SOUTH_EAST,
		SOUTH_WEST,
		NORTH_EAST,
		NORTH_WEST
	};
}
