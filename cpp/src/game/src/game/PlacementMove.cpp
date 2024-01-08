#include "game/PlacementMove.hpp"

namespace Alphalcazar::Game {
	bool PlacementMove::Valid() const {
		const bool coordinatesValid = Coordinates.Valid();
		const bool pieceTypeValid = PieceType != 0;
		return coordinatesValid && pieceTypeValid;
	}

	bool PlacementMove::operator==(const PlacementMove& other) const {
		const bool pieceTypesMatch = PieceType == other.PieceType;
		const bool coordinatesMatch = Coordinates == other.Coordinates;
		return pieceTypesMatch && coordinatesMatch;
	}
}
