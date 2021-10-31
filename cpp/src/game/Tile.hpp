#pragma once

#include "aliases.hpp"
#include <memory>

namespace Alphalcazar::Game {
	class Piece;

	class Tile {
	public:
		Tile();
		Tile(const Tile& other) = delete;
		~Tile();

		void PlacePiece(Piece* piece);
		void RemovePiece();
		Piece* GetPiece();
	private:
		Piece* mPiece = nullptr;
	};
}
