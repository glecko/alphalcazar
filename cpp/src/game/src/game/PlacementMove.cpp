#include "game/PlacementMove.hpp"

namespace Alphalcazar::Game {
	PlacementMove::PlacementMove()
		: Coordinates{ Coordinates::Invalid() }
		, PieceType{ 0 }
	{}

	PlacementMove::PlacementMove(const Game::Coordinates& coordinates, Game::PieceType pieceType)
		: Coordinates{ coordinates }
		, PieceType{ pieceType }
	{}

	PlacementMove::~PlacementMove() {}

	bool PlacementMove::Valid() const {
		return Coordinates.Valid() && PieceType != 0;
	}

	bool PlacementMove::operator==(const PlacementMove& other) const {
		return PieceType == other.PieceType && Coordinates == other.Coordinates;
	}
}
