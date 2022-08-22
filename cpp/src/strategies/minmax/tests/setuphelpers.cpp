#include "setuphelpers.hpp"

#include <game/Board.hpp>
#include <game/Piece.hpp>

namespace Alphalcazar::Strategy::MinMax {
	PieceSetup::PieceSetup(Game::PlayerId playerId, Game::PieceType pieceType, Game::Direction direction, Game::Coordinates&& coordinates)
		: PlayerId{ playerId }
		, PieceType{ pieceType }
		, Direction{ direction }
		, Coordinates{ coordinates }
	{}

	Game::Game SetupGameForMinMaxTesting(Game::PlayerId playerWithInitiative, bool firstMoveExecuted, const std::vector<PieceSetup>& pieceSetups) {
		Game::Game game{};
		game.GetState().FirstMoveExecuted = firstMoveExecuted;
		game.GetState().PlayerWithInitiative = playerWithInitiative;

		for (auto& pieceSetup : pieceSetups) {
			Game::Piece piece{ pieceSetup.PlayerId, pieceSetup.PieceType };
			game.GetBoard().PlacePiece(pieceSetup.Coordinates, piece, pieceSetup.Direction);
		}
		return game;
	}
}
