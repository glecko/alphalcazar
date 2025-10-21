#include "FontManager.hpp"
#include <imgui.h>

namespace Alphalcazar::Ui {
	constexpr float c_DefaultFontSize = 14.f;
	constexpr float c_PieceNumberFontSize = 40.f;
	
	FontManager::FontManager() {
		const ImGuiIO& io = ImGui::GetIO();
		
		// Add Arial as a default (first to load) font for small-size text
		mDefaultFont = io.Fonts->AddFontFromFileTTF("resources/fonts/Arial.ttf", c_DefaultFontSize);

		// Load other fonts 
		mPieceNumberFont = io.Fonts->AddFontFromFileTTF("resources/fonts/Namecat.ttf", c_PieceNumberFontSize);
	}

	ImFont* FontManager::GetPieceNumberFont() const {
		return mPieceNumberFont;
	}

	ImFont* FontManager::GetDefaultFont() const {
		return mDefaultFont;
	}

	float FontManager::GetDefaultFontSize() {
		return c_DefaultFontSize;
	}
}
