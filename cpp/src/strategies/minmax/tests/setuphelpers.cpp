#include "setuphelpers.hpp"

#include <game/Board.hpp>
#include <game/Piece.hpp>

namespace Alphalcazar::Strategy::MinMax {
	Game::Game SetupGameForMinMaxTesting(Game::PlayerId playerWithInitiative, bool firstMoveExecuted, const std::vector<PieceSetup>& pieceSetups) {
		Game::Game game{};
		game.GetState().FirstMoveExecuted = firstMoveExecuted;
		game.GetState().PlayerWithInitiative = playerWithInitiative;

		for (const auto& pieceSetup : pieceSetups) {
			Game::Piece piece{ pieceSetup.PlayerId, pieceSetup.PieceType };
			game.GetBoard().PlacePiece(pieceSetup.Coordinates, piece, pieceSetup.Direction);
		}
		return game;
	}
}
