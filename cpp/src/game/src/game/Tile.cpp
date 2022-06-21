#include "game/Piece.hpp"
#include "game/Tile.hpp"

namespace Alphalcazar::Game {
	Tile::Tile() {}

	Tile::Tile(const Coordinates& coordinates)
		: mCoordinates{ coordinates }
	{}

	Tile::Tile(const Tile& other)
		: mCoordinates { other.mCoordinates }
		, mPiece { other.mPiece }
	{}

	Tile::~Tile() {}

	void Tile::PlacePiece(const Piece& piece) {
		if (mPiece.IsValid()) {
			throw "Tried placing a piece on a tile that already had a piece on it.";
		}
		mPiece = piece;
	}

	void Tile::RemovePiece() {
		// Replace the existing piece with an invalid piece
		mPiece = {};
	}

	const Coordinates& Tile::GetCoordinates() const {
		return mCoordinates;
	}

	Direction Tile::GetLegalPlacementDirection() const {
		return mCoordinates.GetLegalPlacementDirection();
	}

	bool Tile::HasPiece() const {
		return mPiece.IsValid();
	}

	const Piece* Tile::GetPiece() const {
		if (mPiece.IsValid()) {
			return &mPiece;
		}
		return nullptr;
	}

	Piece* Tile::GetPiece() {
		if (mPiece.IsValid()) {
			return &mPiece;
		}
		return nullptr;
	}
}
