#pragma once

#include <memory>

namespace Alphalcazar::Ui {
	class PlayGameView;
	class FontManager;

	/*!
	 * \brief The top-level application manager of the UI.
	 */
	class Application {
	public:
		Application();
		~Application();

		void Update();

	private:
		std::unique_ptr<PlayGameView> mPlayGameView;
		std::unique_ptr<FontManager> mFontManager;
	};
}
