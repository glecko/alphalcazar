#pragma once

#include "aliases.hpp"
#include "Coordinates.hpp"
#include <memory>

namespace Alphalcazar::Game {
	class Piece;

	class Tile {
	public:
		Tile(const Coordinates& coordinates);
		Tile(const Tile& other) = delete;
		~Tile();

		void PlacePiece(Piece* piece);
		void RemovePiece();
		const Coordinates& GetCoordinates() const;
		Piece* GetPiece();
	private:
		Piece* mPiece = nullptr;
		Coordinates mCoordinates;
	};
}
