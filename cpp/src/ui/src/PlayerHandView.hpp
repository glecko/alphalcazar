#pragma once

#include "game/aliases.hpp"

struct ImDrawList;

namespace Alphalcazar::Game {
	class Game;
}

namespace Alphalcazar::Ui {
	class FontManager;

	/*!
	 * \brief ImGui view that draws the hand of a player (a row containing
	 * the pieces they have yet to place on the board)
	 *
	 * Its update function takes a ImGui draw list, on which it will draw the pieces, at
	 * the current cursor position. The view will also update the cursor position after
	 * drawing the pieces in hand.
	 */
	class PlayerHandView {
	public:
		PlayerHandView(const Game::Game& game, const FontManager& fontManager);
		~PlayerHandView();

		/// Sets the player for which this view draws the pieces in hand
		void SetPlayer(Game::PlayerId player);

		void Update(ImDrawList& drawList);
		
	private:
		const FontManager& mFontManager;
		const Game::Game& mGame;

		/// The ID of the player for which this view draws the pieces in hand
		Game::PlayerId mPlayerId = Game::PlayerId::NONE;
	};
}
