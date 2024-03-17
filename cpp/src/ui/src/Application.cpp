#include "Application.hpp"

#include "PlayGameView.hpp"
#include "fonts/FontManager.hpp"

#include <imgui.h>

namespace Alphalcazar::Ui {

	Application::Application()
		: mFontManager{ std::make_unique<FontManager>() } {
		mPlayGameView = std::make_unique<PlayGameView>(*mFontManager);
	}

	Application::~Application() = default;

	void Application::Update() {
		mPlayGameView->Update();
	}
}
