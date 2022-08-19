#include "game/Piece.hpp"
#include "game/Tile.hpp"
#include <util/Log.hpp>

namespace Alphalcazar::Game {
	Tile::Tile() {}

	Tile::Tile(const Tile& other)
		: mPiece { other.mPiece }
	{}

	Tile::~Tile() {}

	bool Tile::operator!=(const Tile& other) const {
		return mPiece != other.mPiece;
	}

	void Tile::PlacePiece(const Piece& piece) {
		if (mPiece.IsValid()) {
			Utils::LogError("Tried placing a piece on a tile that already had a piece on it.");
		}
		mPiece = piece;
	}

	void Tile::RemovePiece() {
		// Replace the existing piece with an invalid piece
		mPiece = {};
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
