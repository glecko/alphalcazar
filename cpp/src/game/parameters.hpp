#pragma once

#include <cstdint>
#include "aliases.hpp"

namespace Alphalcazar::Game {
	static constexpr PieceType c_PieceTypes = 5;
	static constexpr PieceType c_PushablePieceType = 1;
	static constexpr PieceType c_PusherPieceType = 4;

	static constexpr Coordinate c_BoardSize = 3;
	static constexpr Coordinate c_PlayAreaSize = c_BoardSize + 2;
	static constexpr Coordinate c_CenterCoordinate = c_BoardSize / 2 + 1;
	/*!
	 * \brief Whether the game ends in a draw condition when both players complete a row.
	 * 
	 * If true, the game will end in a draw if at the end of a turn both players have
	 * achieved the win condition (completed a row/column/diagonal).
	 * 
	 * If false, the above situation will not be considered a valid condition and the game will
	 * continue with a new turn as usual, until a single player has completed a row/column/diagonal.
	 * 
	 * \note The game can still result in a draw if this is false if a stalemate is reached on the board.
	 */
	static constexpr bool c_AcceptDraws = true;
}
