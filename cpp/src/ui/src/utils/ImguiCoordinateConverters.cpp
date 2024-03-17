#include "ImguiCoordinateConverters.hpp"

namespace Alphalcazar::Ui {
	ImVec2 ToImVec2TopLeft(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset) {
		return ImVec2{
			static_cast<float>(coord.x) * coordinatesToPixels + offset.x,
			static_cast<float>(coord.y) * coordinatesToPixels + offset.y
		};
	}

	ImVec2 ToImVec2BottomRight(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset) {
		return ImVec2{
			static_cast<float>(coord.x + 1) * coordinatesToPixels + offset.x,
			static_cast<float>(coord.y + 1) * coordinatesToPixels + offset.y
		};
	}

	ImVec2 ToImVec2BottomLeft(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset) {
		return ImVec2{
			static_cast<float>(coord.x) * coordinatesToPixels + offset.x,
			static_cast<float>(coord.y + 1) * coordinatesToPixels + offset.y
		};
	}

	ImVec2 ToImVec2Center(const Game::Coordinates& coord, float coordinatesToPixels, const ImVec2& offset) {
		return ImVec2{
			(static_cast<float>(coord.x ) + 0.5f) * coordinatesToPixels + offset.x,
			(static_cast<float>(coord.y) + 0.5f) * coordinatesToPixels + offset.y
		};
	}
}
