#include "Tile.hpp"
#include "Piece.hpp"

namespace Alphalcazar::Game {
	Tile::Tile() {}

	Tile::~Tile() {}

	void Tile::PlacePiece(Piece* piece) {
		if (mPiece) {
			throw "Tried placing a piece on a tile that already had a piece on it.";
		}
		mPiece = piece;
		mPiece->SetInPlay(true);
	}

	void Tile::RemovePiece() {
		mPiece->SetInPlay(false);
		mPiece = nullptr;
	}

	Piece* Tile::GetPiece() {
		return mPiece;
	};
}
