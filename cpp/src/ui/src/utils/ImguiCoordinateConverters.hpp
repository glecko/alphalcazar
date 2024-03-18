#pragma once

#include "game/Coordinates.hpp"
#include <imgui.h>

namespace Alphalcazar::Ui {
	/*!
	 * \brief Converts a board coordinates structure to a set of ImGui 2D coordinates.
	 *        Returns the coordinates of the top-left corner of the specified board tile.
	 *
	 * \param coord The game board coordinates to convert.
	 * \param coordinatesToPixels The board coordinate to pixel ratio (a.k.a. the height/width of a board tile in pixels)
	 * \param offset A fixed offset to add to the result when converting.
	 */
	ImVec2 ToImVec2TopLeft(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset);

	/// Same as \ref ToImVec2TopLeft, but returns the coordinates of the bottom-right corner of the specified board tile.
	ImVec2 ToImVec2BottomRight(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset);
	
	/// Same as \ref ToImVec2TopLeft, but returns the coordinates of the bottom-left corner of the specified board tile.
	ImVec2 ToImVec2BottomLeft(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset);

	/// Same as \ref ToImVec2TopLeft, but returns the center coordinates of the specified board tile.
	ImVec2 ToImVec2Center(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset);
}
