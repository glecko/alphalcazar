#pragma once

#include "aliases.hpp"
#include "parameters.hpp"
#include <type_traits>

namespace Alphalcazar::Game {
	/// Mask used to obtain the owner from the piece info. See the docstring of \ref mPieceInfo
	constexpr std::uint8_t c_PieceOwnerMask = 0b00000011;
	/// Mask used to obtain the direction from the piece info. See the docstring of \ref mPieceInfo
	constexpr std::uint8_t c_PieceDirectionMask = 0b00011100;
	/// Mask used to obtain the piece type from the piece info. See the docstring of \ref mPieceInfo
	constexpr std::uint8_t c_PieceTypeMask = 0b11100000;
	/// Mask used to obtain the piece owner & type data from the piece info. See the docstring of \ref mPieceInfo
	constexpr std::uint8_t c_PieceTypeAndOwnerMask = c_PieceOwnerMask ^ c_PieceTypeMask;

	/*!
	 * \brief Represents a piece that belongs to a player.
	 *
	 * Can be either placed on the board or in its owner's hand.
	 * While the piece is in play on the board, it will have a movement direction.
	 */
	class Piece {
	public:
		/// Default constructor for \ref Piece, constructs an invalid piece
		Piece() noexcept {}

		Piece(PlayerId owner, PieceType type) noexcept {
			// Combine the bit representations of the owner and type (shifted 5 bits to the left to be properly aligned).
			// The resulting \ref mPieceInfo will have the bits representing the direction (4-6 from the left) set to 0.
			auto alignedOwnerData = static_cast<std::underlying_type<PlayerId>::type>(owner);
			PieceType alignedTypeData = (type << 5);
			mPieceInfo = alignedTypeData ^ alignedOwnerData;
		}

		/// Returns the type (that determines the movement order) of the piece
		PieceType GetType() const {
			// No need to apply the \ref c_PieceTypeMask here, since the type is represented
			// at the 3 left-most bits of \ref mPieceInfo, by shifting 5 bits to the right
			// we are already zeroing all the other bits. It's very likely the compiler would optimize
			// out the flag operation anyways, but it's best not to rely on that.
			return mPieceInfo >> 5;
		}

		/// Retrusn the owner of the piece
		PlayerId GetOwner() const {
			return static_cast<PlayerId>(mPieceInfo & c_PieceOwnerMask);
		}

		/// Returns the direction at which the piece is currently facing
		Direction GetMovementDirection() const {
			std::uint8_t unalignedDirectionData = mPieceInfo & c_PieceDirectionMask;
			std::uint8_t alignedDirectionData = unalignedDirectionData >> 2;
			return static_cast<Direction>(alignedDirectionData);
		}

		/// Returns whether the piece can be pushed by the movement of other pieces
		bool IsPushable() const {
			return GetType() == c_PushablePieceType;
		}

		/// Returns whether the piece can push other pieces when moving
		bool IsPusher() const {
			return GetType() == c_PusherPieceType;
		}

		/// Returns if the data structure represents a valid piece
		bool IsValid() const {
			// Since at all calls of this function, all relevant values are loaded in the L1 cache,
			// we parallelise the comparisons to avoid branching (causing a potential instruction-level cache miss)
			bool typeIsValid = GetType() != c_InvalidPieceType;
			bool ownerIsValid = (mPieceInfo & c_PieceOwnerMask) != 0;
			return typeIsValid && ownerIsValid;
		}

		/// Sets the direction at which the piece is facing
		void SetMovementDirection(Direction direction) {
			std::uint8_t alignedDirectionData = static_cast<std::uint8_t>(direction) << 2;
			mPieceInfo ^= alignedDirectionData;
		}

		bool operator==(const Piece& other) const {
			std::uint8_t pieceTypeAndOwnerData = mPieceInfo & c_PieceTypeAndOwnerMask;
			std::uint8_t otherPieceTypeAndOwnerData = other.mPieceInfo & c_PieceTypeAndOwnerMask;
			return pieceTypeAndOwnerData == otherPieceTypeAndOwnerData;
		}
	private:
		/*!
		 * \brief A 8-bit field containing the information about the piece.
		 *
		 * The 3 left-most bits are used to represent the type, or movement order, of the piece.
		 * It indicates in which order (relative to other pieces) the piece will execute its movement in the movement phase at the end of each turn.
		 * Since currently there are 6 possible values for this state (5 pieces and invalid), we use 3 bits (8 possible states) for it.
		 *
		 * The next 2 bits (4-6 from the left) are used to represent the movement direction of the piece while it is in play on the board.
		 * In theory there are 9 possible states for this field (4 cardinal directions, 4 intercardinal directions and the invalid state).
		 * However, the game rules currently only allow pieces to have one of the 4 cardinal directions. Therefore we can (until this changes) use
		 * 3 bits (8 possible states) for it.
		 *
		 * The last 2 bits (7-8 from the left) are used to represent the ID of the player that owns this piece.
		 * Since currently there are 3 possible values for this state (2 players and none), we use 2 bits (4 possible states) for it.
		 *
		 * Storing the data this way makes the whole \ref Piece struct as a whole take up 1 byte, which is very convenient.
		 */
		std::uint8_t mPieceInfo = 0;
	};
}
