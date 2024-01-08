#pragma once

#include "game/Game.hpp"
#include "game/Board.hpp"
#include "game/Coordinates.hpp"
#include "game/aliases.hpp"

namespace Alphalcazar::Game {
	static std::array<Piece, 10> c_AllPieces = { {
		{ PlayerId::PLAYER_ONE, 1 },
		{ PlayerId::PLAYER_ONE, 2 },
		{ PlayerId::PLAYER_ONE, 3 },
		{ PlayerId::PLAYER_ONE, 4 },
		{ PlayerId::PLAYER_ONE, 5 },
		{ PlayerId::PLAYER_TWO, 1 },
		{ PlayerId::PLAYER_TWO, 2 },
		{ PlayerId::PLAYER_TWO, 3 },
		{ PlayerId::PLAYER_TWO, 4 },
		{ PlayerId::PLAYER_TWO, 5 },
	}};

	/// Data structure helper for describing a piece placement on the board
	struct PieceSetup {
		PlayerId PlayerId;
		PieceType PieceType;
		Direction Direction;
		Coordinates Coordinates;
	};

	/// Helper function for quickly configuring a game setup
	Game SetupGameForTesting(PlayerId playerWithInitiative, bool firstMoveExecuted, const std::vector<PieceSetup>& pieceSetups);

	/// Helper function for quickly configuring a board's initial position
	Board SetupBoardForTesting(const std::vector<PieceSetup>& pieceSetups);
}
