#include "game/PlacementMove.hpp"

namespace Alphalcazar::Game {
	bool PlacementMove::Valid() const {
		bool coordinatesValid = Coordinates.Valid();
		bool pieceTypeValid = PieceType != 0;
		return coordinatesValid && pieceTypeValid;
	}

	bool PlacementMove::operator==(const PlacementMove& other) const {
		bool pieceTypesMatch = PieceType == other.PieceType;
		bool coordinatesMatch = Coordinates == other.Coordinates;
		return pieceTypesMatch && coordinatesMatch;
	}
}
