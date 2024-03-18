#include "PlayerHandView.hpp"

#include "fonts/FontManager.hpp"
#include "game/Game.hpp"
#include "utils/ImguiDrawers.hpp"

#include <imgui.h>

namespace Alphalcazar::Ui {
	constexpr float c_PieceInHandRadius = 25.f;
	constexpr float c_HandItemsPadding = 10.f;

	constexpr float c_StartingPlayerCrownRadius = 15.f;
	constexpr ImColor c_StartingPlayerCrownColor = ImColor(224, 187, 118, 255);
	
	PlayerHandView::PlayerHandView(const Game::Game& game, const FontManager& fontManager)
	: mFontManager { fontManager }
	, mGame { game } {};

	PlayerHandView::~PlayerHandView() = default;
	
	void PlayerHandView::SetPlayer(Game::PlayerId player) {
		mPlayerId = player;
	}

	void PlayerHandView::Update(ImDrawList& drawList) {
		const ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		
		float pieceHorizontalPosition = cursorPos.x;
		const float handRowCenterHeight = cursorPos.y + c_PieceInHandRadius;
		for (const Game::Piece& piece : mGame.GetPiecesInHand(mPlayerId)) {
			
			const ImVec2 piecePos = { pieceHorizontalPosition + c_PieceInHandRadius, handRowCenterHeight };
			DrawPiece(piece, piecePos, c_PieceInHandRadius, drawList, mFontManager.GetPieceNumberFont());

			pieceHorizontalPosition += c_PieceInHandRadius * 2 + c_HandItemsPadding;
		}

		// If the player who's hand we're drawing has the initiative, draw a crown indicator to reflect this
		if (mGame.GetState().PlayerWithInitiative == mPlayerId) {
			const ImVec2 startingPlayerCrownCenter = { pieceHorizontalPosition + c_StartingPlayerCrownRadius, handRowCenterHeight };
			drawList.AddCircleFilled(startingPlayerCrownCenter, c_StartingPlayerCrownRadius, c_StartingPlayerCrownColor);
		}

		// Update the cursor screen position by adding the draw row height to its Y component
		const ImVec2 newCursorPos = { cursorPos.x, cursorPos.y + c_PieceInHandRadius * 2 };
		ImGui::SetCursorScreenPos(newCursorPos);
	}
}
