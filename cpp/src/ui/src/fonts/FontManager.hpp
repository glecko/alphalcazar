#pragma once

struct ImFont;

namespace Alphalcazar::Ui {

	/*!
	 * \brief System in charge of registering & managing ImGui fonts.
	 */
	class FontManager {
	public:
		FontManager();

		/// Returns the default ImGui font of the application
		ImFont* GetDefaultFont() const;

		/// Returns the ImGui font to be used to draw piece numbers
		ImFont* GetPieceNumberFont() const;

		/// Returns the default font size of the application
		static float GetDefaultFontSize();
		
	private:
		ImFont* mDefaultFont = nullptr;
		ImFont* mPieceNumberFont = nullptr;
	};
}
