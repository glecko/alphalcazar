#pragma once

#include <memory>

namespace Alphalcazar::Game {
	class Game;
}

namespace Alphalcazar::Ui {
	class BoardView;
	class FontManager;
	class PlayerHandView;

	/*!
	 * \brief ImGui view that allows playing a game of advanced tic-tac-toe
	 *        and drawing the state of this game.
	 */
	class PlayGameView {
	public:
		PlayGameView(const FontManager& fontManager);
		~PlayGameView();

		void Update();

	private:
		std::unique_ptr<Game::Game> mGame;

		// Children views
		std::unique_ptr<BoardView> mBoardView;
		std::unique_ptr<PlayerHandView> mPlayerOneHandView;
		std::unique_ptr<PlayerHandView> mPlayerTwoHandView;
	};
}
