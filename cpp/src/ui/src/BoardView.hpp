#pragma once

#include "game/Coordinates.hpp"

struct ImDrawList;

namespace Alphalcazar::Game {
	class Board;
	class Tile;
	class Piece;
}

namespace Alphalcazar::Ui {
	class FontManager;

	/*!
	 * \brief ImGui view that draws a given board & its contents (e.g. pieces).
	 *
	 * Its update function takes a ImGui draw list, on which it will draw the board, at
	 * the current cursor position. The view will also update the cursor position after
	 * drawing the board.
	 */
	class BoardView {
	public:
		BoardView(const Game::Board& board, const FontManager& fontManager);
		~BoardView();

		void Update(ImDrawList& drawList);

	private:
		const FontManager& mFontManager;
		const Game::Board& mBoard;

		/// Draws a representation of a tile and its content (e.g. if it contains a piece)
		void DrawTile(const Game::Coordinates& coord, const Game::Tile& tile, ImDrawList& drawList) const;
	};
}
