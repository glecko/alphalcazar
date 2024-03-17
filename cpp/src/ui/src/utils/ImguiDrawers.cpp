#include "ImguiDrawers.hpp"

#include "game/Piece.hpp"
#include <imgui.h>
#include <string>

namespace Alphalcazar::Ui {
	constexpr ImColor c_PlayerOnePieceColor = ImColor(154, 205, 50, 255);
	constexpr ImColor c_PlayerTwoPieceColor = ImColor(255, 140, 0, 255);
	constexpr ImColor c_InvalidPieceColor = ImColor(0, 0, 0, 255);
	constexpr ImColor c_PieceNumberColor = ImColor(255, 255, 255, 255);
	
	/*!
	 * \brief How far (as a multiple of the piece radius) from the pieces center we
	 *        draw the end of the triangle that represents the piece's movement direction.
	 */
	constexpr float c_PieceDirectionIndicatorToRadiusRatio = 1.2f;

	/// The ratio of font size of the text used to draw the piece number to the radius of the piece. 
	constexpr float c_PieceFontSizeToRadiusRatio = 1.25f;

	/// Returns the ImGui color with which do draw a piece's background color
	ImColor GetPieceColor(const Game::Piece& piece) {
		switch(piece.GetOwner()) {
		case Game::PlayerId::PLAYER_ONE:
			return c_PlayerOnePieceColor;
		case Game::PlayerId::PLAYER_TWO:
			return c_PlayerTwoPieceColor;
		case Game::PlayerId::NONE:
			return c_InvalidPieceColor;
		}
		return c_InvalidPieceColor;
	}
	
	void DrawPiece(
		const Game::Piece& piece,
		const ImVec2& centerPos,
		const float radius,
		ImDrawList& drawList,
		const ImFont* textFont
	) {
		const ImColor pieceColor = GetPieceColor(piece);
		
		drawList.AddCircleFilled(centerPos, radius, pieceColor);
		
		const float directionIndicatorRadius = radius * c_PieceDirectionIndicatorToRadiusRatio;
		switch(piece.GetMovementDirection()) {
		case Game::Direction::NORTH: {
			const ImVec2 p1 { centerPos.x + radius, centerPos.y };
			const ImVec2 p2 { centerPos.x - radius, centerPos.y };
			const ImVec2 p3 { centerPos.x, centerPos.y + directionIndicatorRadius / 2.f };
			drawList.AddTriangleFilled(p1, p2, p3, pieceColor);	
		} break;
		case Game::Direction::SOUTH: {
			const ImVec2 p1 { centerPos.x + radius, centerPos.y };
			const ImVec2 p2 { centerPos.x - radius, centerPos.y };
			const ImVec2 p3 { centerPos.x, centerPos.y - directionIndicatorRadius / 2.f };
			drawList.AddTriangleFilled(p1, p2, p3, pieceColor);	
		} break;
		case Game::Direction::EAST: {
			const ImVec2 p1 { centerPos.x, centerPos.y + radius };
			const ImVec2 p2 { centerPos.x, centerPos.y - radius };
			const ImVec2 p3 { centerPos.x + directionIndicatorRadius / 2.f, centerPos.y };
			drawList.AddTriangleFilled(p1, p2, p3, pieceColor);
		} break;
		case Game::Direction::WEST: {
			const ImVec2 p1 { centerPos.x, centerPos.y + radius };
			const ImVec2 p2 { centerPos.x, centerPos.y - radius };
			const ImVec2 p3 { centerPos.x - directionIndicatorRadius / 2.f, centerPos.y };
			drawList.AddTriangleFilled(p1, p2, p3, pieceColor);
		} break;
		default: break;
		}
		
		// Draw the piece number text last (on top)
		const float textFontSize = radius * c_PieceFontSizeToRadiusRatio;
		const ImVec2 textPos { centerPos.x - textFontSize / 4.f, centerPos.y - textFontSize / 2.f };
		drawList.AddText(
			textFont,
			textFontSize,
			textPos,
			c_PieceNumberColor,
			std::to_string(piece.GetType()).c_str()
		);
	}
}
