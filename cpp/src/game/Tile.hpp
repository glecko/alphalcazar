#pragma once

#include "aliases.hpp"
#include "Coordinates.hpp"
#include <memory>

namespace Alphalcazar::Game {
	class Piece;

	/*!
	 * \brief Represents a single tile of the board.
	 */
	class Tile {
	public:
		Tile(const Coordinates& coordinates);
		Tile(const Tile& other);
		~Tile();

		/// Places a piece on this tile
		void PlacePiece(Piece* piece);
		/// Removes the piece that was placed on this tile, if any
		void RemovePiece();
		/// Returns the coordinates at which this tile is located on its parent board
		const Coordinates& GetCoordinates() const;
		/*!
		 * \brief Returns the direction in which pieces may be legally placed on this tile.
		 * 
		 * Returns an invalid direction if the tile is not a perimeter tile.
		 */
		Direction GetLegalPlacementDirection() const;
		/// Returns the piece that is currently placed on this tile
		Piece* GetPiece();
	private:
		Piece* mPiece = nullptr;
		Coordinates mCoordinates;
	};
}
