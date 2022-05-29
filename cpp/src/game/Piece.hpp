#pragma once

#include "aliases.hpp"
#include "Coordinates.hpp"

namespace Alphalcazar::Game {
	/*!
	 * \brief Represents a piece that belongs to a player.
	 * 
	 * Can be either placed on the board or in its owner's hand.
	 * While the piece is in play on the board, it will have a movement direction.
	 */
	class Piece {
	public:
		/// Default constructor for \ref Piece, constructs an invalid piece
		Piece();
		Piece(PlayerId owner, PieceType type);
		Piece(const Piece& other);
		~Piece();

		bool operator==(const Piece& other) const;

		PieceType GetType() const;
		PlayerId GetOwner() const;
		Direction GetMovementDirection() const;

		bool IsPushable() const;
		bool IsPusher() const;

		bool IsValid() const;

		void SetMovementDirection(Direction direction);
	private:
		/// The movement direction of the piece while it is in play on the board
		Direction mDirection = Direction::NONE;
		PieceType mType = c_InvalidPieceType;
		PlayerId mOwner = PlayerId::NONE;
	};
}
