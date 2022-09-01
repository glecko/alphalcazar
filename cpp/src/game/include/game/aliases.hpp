#pragma once

#include <cstdint>

namespace Alphalcazar::Game {
	using BoardMovesCount = std::uint16_t;
	using PieceType = std::uint8_t;
	constexpr PieceType c_InvalidPieceType = 0;
	using Coordinate = std::int32_t;

	enum class GameResult : std::uint8_t {
		NONE = 0,
		DRAW,
		PLAYER_ONE_WINS,
		PLAYER_TWO_WINS,
	};

	enum class PlayerId : std::uint8_t {
		NONE = 0,
		PLAYER_ONE,
		PLAYER_TWO
	};

	enum class Direction : std::uint8_t {
		NONE = 0,
		NORTH,
		SOUTH,
		EAST,
		WEST,
		SOUTH_EAST,
		SOUTH_WEST,
		NORTH_EAST,
		NORTH_WEST,
		SIZE
	};
}
