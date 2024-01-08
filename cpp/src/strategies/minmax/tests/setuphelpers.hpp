#pragma once

#include <game/Game.hpp>
#include <game/aliases.hpp>

namespace Alphalcazar::Strategy::MinMax {
	/// Data structure helper for describing a piece placement on the board
	struct PieceSetup {
		Game::PlayerId PlayerId;
		Game::PieceType PieceType;
		Game::Direction Direction;
		Game::Coordinates Coordinates;
	};

	/// Helper function for quickly configuring a game setup for a MinMax strategy test
	Game::Game SetupGameForMinMaxTesting(Game::PlayerId playerWithInitiative, bool firstMoveExecuted, const std::vector<PieceSetup>& pieceSetups);
}
