#include "game/Piece.hpp"
#include "game/Tile.hpp"
#include "safety_checks.hpp"
#include <util/Log.hpp>

namespace Alphalcazar::Game {
	void Tile::PlacePiece(const Piece& piece) {
		if constexpr (c_BoardPiecePlacementIntegrityChecks) {
			if (mPiece.IsValid()) {
				Utils::LogError("Tried placing a piece on a tile that already had a piece on it.");
			}
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

	const Piece& Tile::GetPiece() const {
		return mPiece;
	}

	Piece& Tile::GetPiece() {
		return mPiece;
	}
}
