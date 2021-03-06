#pragma once

#include "aliases.hpp"
#include "Piece.hpp"
#include <memory>
#include <optional>

namespace Alphalcazar::Game {
	/*!
	 * \brief Represents a single tile of the board.
	 *
	 * It holds information about which piece (if any) is placed on it.
	 */
	class Tile {
	public:
		Tile();
		Tile(const Tile& other);
		~Tile();

		/// Places a piece on this tile
		void PlacePiece(const Piece& piece);
		/// Removes the piece that was placed on this tile, if any
		void RemovePiece();
		/// Returns the piece that is currently placed on this tile, or std::nullopt if no piece is on this tile
		const Piece* GetPiece() const;
		Piece* GetPiece();

		bool HasPiece() const;
	private:
		Piece mPiece;
	};
}
