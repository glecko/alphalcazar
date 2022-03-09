#include "Tile.hpp"
#include "Piece.hpp"

namespace Alphalcazar::Game {
	Tile::Tile(const Coordinates& coordinates)
		: mCoordinates{ coordinates }
	{}

	Tile::Tile(const Tile& other) 
		: mCoordinates { other.mCoordinates }
	{}

	Tile::~Tile() {}

	void Tile::PlacePiece(Piece* piece) {
		if (mPiece) {
			throw "Tried placing a piece on a tile that already had a piece on it.";
		}
		mPiece = piece;
		mPiece->SetCoordinates(mCoordinates);
	}

	void Tile::RemovePiece() {
		mPiece->RemoveFromPlay();
		mPiece = nullptr;
	}

	const Coordinates& Tile::GetCoordinates() const {
		return mCoordinates;
	}

	Direction Tile::GetLegalPlacementDirection() const {
		return mCoordinates.GetLegalPlacementDirection();
	}

	Piece* Tile::GetPiece() {
		return mPiece;
	};
}
