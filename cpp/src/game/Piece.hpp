#pragma once

#include "aliases.hpp"

namespace Alphalcazar::Game {
	class Tile;
	struct Coordinates;

	class Piece {
	public:
		Piece(PlayerId owner, PieceType type);
		Piece(const Piece& other);
		~Piece();

		bool operator==(const Piece& other) const;

		PieceType GetType() const;
		PlayerId GetOwner() const;
		Direction GetMovementDirection() const;

		bool IsPushable() const;
		bool IsPusher() const;

		void SetMovementDirection(Direction direction);
		void SetInPlay(bool inPlay);
		bool IsInPlay() const;
	private:
		Direction mDirection = Direction::NONE;
		PieceType mType;
		PlayerId mOwner;
		/*!
		 * Represents if the piece is currently placed on the board. Exists for convenience
		 * of players quickly checking their legal moves without needing to scan over the board
		 * to see if their pieces are in play.
		 */
		bool mInPlay = false;
	};
}
