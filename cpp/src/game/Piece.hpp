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
		Piece(PlayerId owner, PieceType type);
		Piece(const Piece& other);
		~Piece();

		bool operator==(const Piece& other) const;

		PieceType GetType() const;
		PlayerId GetOwner() const;
		Direction GetMovementDirection() const;
		const Coordinates& GetCoordinates() const;

		bool IsPushable() const;
		bool IsPusher() const;
		bool IsInPlay() const;

		void SetMovementDirection(Direction direction);
		void SetCoordinates(const Coordinates& coordinates);
		void RemoveFromPlay();
	private:
		/// The movement direction of the piece while it is in play on the board
		Direction mDirection = Direction::NONE;
		/// The coordinates at which the piece is currently located, or invalid coordinates if the piece is not in play
		Coordinates mCoordinates = Coordinates::Invalid();
		PieceType mType;
		PlayerId mOwner;
	};
}
