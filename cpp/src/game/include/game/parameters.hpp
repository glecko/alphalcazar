#pragma once

#include <cstdint>
#include "aliases.hpp"

namespace Alphalcazar::Game {
	/// The amount of pieces / piece types each player has in total
	constexpr PieceType c_PieceTypes = 5;
	/// The type of the piece that can be pushed by other pieces, except by other pushable pieces
	constexpr PieceType c_PushablePieceType = 1;
	/// The type of piece that can push other pieces, including pusher pieces
	constexpr PieceType c_PusherPieceType = 4;

	/// The width and height (in tiles) of the game board. A value of 3 means the game will be played on a 3x3 board.
	constexpr Coordinate c_BoardSize = 3;

	/*!
	 * \brief The width and height (in tiles) of the complete play area.
	 * 
	 * Consists of the game board, and an additional perimeter of tiles around it on which pieces
	 * can be placed by players in order for them to move into the board.
	 */
	constexpr Coordinate c_PlayAreaSize = c_BoardSize + 2;

	/// The coordinate at the center of the board. This value will only have significance for odd board sizes.
	constexpr Coordinate c_CenterCoordinate = c_BoardSize / 2 + 1;

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
	constexpr bool c_AcceptDraws = false;
}
