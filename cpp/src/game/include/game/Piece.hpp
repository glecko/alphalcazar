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
		Piece() noexcept
			: mType{ c_InvalidPieceType }
			, mDirection{ Direction::NONE }
			, mOwner{ PlayerId::NONE }
		{}

		Piece(PlayerId owner, PieceType type) noexcept
			: mType{ type }
			, mDirection{ Direction::NONE }
			, mOwner{ owner }
		{}

		Piece(const Piece& other) noexcept
			: mType{ other.mType }
			, mDirection{ other.mDirection }
			, mOwner{ other.mOwner }
		{}

		PieceType GetType() const {
			return mType;
		}

		PlayerId GetOwner() const {
			return mOwner;
		}

		Direction GetMovementDirection() const {
			return mDirection;
		}

		bool IsPushable() const {
			return mType == c_PushablePieceType;
		}

		bool IsPusher() const {
			return mType == c_PusherPieceType;
		}

		bool IsValid() const {
			bool typeIsValid = mType != c_InvalidPieceType;
			bool ownerIsValid = mOwner != PlayerId::NONE;
			return typeIsValid && ownerIsValid;
		}

		void SetMovementDirection(Direction direction) {
			mDirection = direction;
		}

		bool operator==(const Piece& other) const {
			bool typeMatches = other.mType == mType;
			bool ownerMatches = other.mOwner == mOwner;
			return typeMatches && ownerMatches;
		}
	private:
		/*!
		 * \brief The type, or movement order, of the piece.
		 *
		 * The type of a piece indicates in which order (relative to other pieces) the piece will execute its movement
		 * in the movement phase at the end of each turn.
		 *
		 * \note Since currently there are 6 possible values for this state (5 pieces and invalid), we assign 3 bits (8 possible states)
		 *       to this member.
		 */
		PieceType mType : 3;
		/*!
		 * \brief The movement direction of the piece while it is in play on the board
		 *
		 * \note In theory there are 9 possible states for this field (4 cardinal directions, 4 intercardinal directions and the invalid state).
		 *       However, the game rules currently only allow pieces to have one of the 4 cardinal directions. Therefore we can (until this changes) assign
		 *       3 bits (8 possible states) to this field. This makes the whole \ref Piece struct as a whole take up 1 byte, which is very convenient.
		 */
		Direction mDirection : 3;
		/*!
		 * \brief The ID of the player that owns this piece.
		 *
		 * \note Since currently there are 3 possible values for this state (2 players and none), we assign 2 bits (4 possible states) to this member.
		 */
		PlayerId mOwner : 2;
	};
}
