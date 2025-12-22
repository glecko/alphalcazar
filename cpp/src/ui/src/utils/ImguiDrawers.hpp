#pragma once

namespace Alphalcazar::Game {
	class Piece;
}

struct ImDrawList;
struct ImVec2;
struct ImFont;

namespace Alphalcazar::Ui {

	/*!
	 * \brief Draws a piece in a ImGui draw list.
	 *
	 * \param piece The piece to draw.
	 * \param centerPos The coordinates at which to draw the piece's center.
	 * \param radius The radius (in pixels) the piece should have.
	 * \param drawList The ImGui draw list onto which we want to draw the piece.
	 * \param textFont The ImGui font we want the piece number to be drawn with.
	 */
	void DrawPiece(
		const Game::Piece& piece,
		const ImVec2& centerPos,
		float radius,
		ImDrawList& drawList,
		const ImFont* textFont
	);
}
