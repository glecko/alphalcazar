#pragma once

#include "Coordinates.hpp"
#include "aliases.hpp"

namespace Alphalcazar::Game {
	class PlacementMove {
	public:
		PlacementMove(Coordinates coordinates, PieceType pieceType, PlayerId playerId);
		~PlacementMove();

		bool IsValid() const;
		Coordinates& GetCoordinates();
		PieceType GetPieceType();
		PlayerId GetPlayerId();
	private:
		Coordinates mCoordinates;
		PieceType mPieceType;
		PlayerId mPlayerId;
	};
}
