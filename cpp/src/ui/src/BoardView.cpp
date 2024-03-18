#include "BoardView.hpp"

#include "fonts/FontManager.hpp"
#include "game/Board.hpp"
#include "utils/ImguiCoordinateConverters.hpp"
#include "utils/ImguiDrawers.hpp"

#include <imgui.h>

namespace Alphalcazar::Ui {
	// Tile constants
	constexpr ImColor c_TileBorderColor = ImColor(50, 50, 50, 255);
	constexpr ImColor c_BoardTileBackgroundColor = ImColor(116, 178, 231, 255);
	constexpr ImColor c_PerimeterTileBackgroundColor = ImColor(58, 89, 115, 255);
	constexpr float c_TileSize = 80.f;

	// Piece constants
	constexpr float c_PieceRadius = c_TileSize * 0.4f;
	
	BoardView::BoardView(const Game::Board& board, const FontManager& fontManager)
		: mFontManager { fontManager }
		, mBoard { board } {};

	BoardView::~BoardView() = default;

	void BoardView::Update(ImDrawList& drawList) {
		mBoard.LoopOverTiles([this, &drawList](const Game::Coordinates& coord, const Game::Tile& tile) {
			DrawTile(coord, tile, drawList);
			return false;
		});

		// Update the cursor screen position by adding the drawn board height to its Y component
		constexpr Game::Coordinates c_BottomLeftTileCoord = { 0, Game::c_PlayAreaSize - 1 };
		const ImVec2 newCursorPos = ToImVec2BottomLeft(c_BottomLeftTileCoord, c_TileSize, ImGui::GetCursorScreenPos());
		ImGui::SetCursorScreenPos(newCursorPos);
	}
	
	void BoardView::DrawTile(const Game::Coordinates& coord, const Game::Tile& tile, ImDrawList& drawList) const {
		const ImVec2 topLeft = ToImVec2TopLeft(coord, c_TileSize, ImGui::GetCursorScreenPos());
		const ImVec2 bottomRight = ToImVec2BottomRight(coord, c_TileSize, ImGui::GetCursorScreenPos());
		
		const ImColor backgroundColor = coord.IsPerimeter() ? c_PerimeterTileBackgroundColor : c_BoardTileBackgroundColor;
		drawList.AddRectFilled(topLeft, bottomRight, backgroundColor);
		drawList.AddRect(topLeft, bottomRight, c_TileBorderColor);

		// Draw the tile's piece on top of the background
		if (tile.HasPiece()) {
			const Game::Piece& piece = tile.GetPiece();
			const ImVec2 center = ToImVec2Center(coord, c_TileSize, ImGui::GetCursorScreenPos());
			
			DrawPiece(piece, center, c_PieceRadius, drawList, mFontManager.GetPieceNumberFont());
		}
	}
}
