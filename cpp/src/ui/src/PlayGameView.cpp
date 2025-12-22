#include "PlayGameView.hpp"

#include "game/Game.hpp"
#include "BoardView.hpp"
#include "PlayerHandView.hpp"

#include <imgui.h>

namespace Alphalcazar::Ui {

	PlayGameView::PlayGameView(const FontManager& fontManager)
		: mGame { std::make_unique<Game::Game>() }
		, mBoardView{ std::make_unique<BoardView>(mGame->GetBoard(), fontManager) }
		, mPlayerOneHandView{ std::make_unique<PlayerHandView>(*mGame, fontManager) }
		, mPlayerTwoHandView{ std::make_unique<PlayerHandView>(*mGame, fontManager) } {

		mPlayerOneHandView->SetPlayer(Game::PlayerId::PLAYER_ONE);
		mPlayerTwoHandView->SetPlayer(Game::PlayerId::PLAYER_TWO);
	}

	PlayGameView::~PlayGameView() = default;

	void PlayGameView::Update() {
		ImGui::Begin("Play advanced tic-tac-toe!");

		if (ImDrawList* drawList = ImGui::GetWindowDrawList()) {
			mPlayerOneHandView->Update(*drawList);
			mBoardView->Update(*drawList);
			mPlayerTwoHandView->Update(*drawList);
		}
		
		ImGui::End();
	}
}
