#include "PlacementMove.hpp"

namespace Alphalcazar::Game {
	PlacementMove::PlacementMove(Coordinates coordinates, PieceType pieceType, PlayerId playerId)
		: mCoordinates { coordinates }
		, mPieceType { pieceType }
		, mPlayerId { playerId }
	{}

	PlacementMove::~PlacementMove() {}

	bool PlacementMove::IsValid() const {
		return true;
	}

	Coordinates& PlacementMove::GetCoordinates() {
		return mCoordinates;
	}

	PieceType PlacementMove::GetPieceType() {
		return mPieceType;
	}

	PlayerId PlacementMove::GetPlayerId() {
		return mPlayerId;
	}
}