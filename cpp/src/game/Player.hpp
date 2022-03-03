#pragma once

#include "aliases.hpp"
#include <vector>
#include <memory>

namespace Alphalcazar::Game {
	class Piece;

	/*!
	 * \brief A player currently playing in a Game instance.
	 * Manages the player's available pieces in hand.
	 */
	class Player {
	public:
		Player(PlayerId id);
		Player(const Player& other);
		~Player();

		bool operator==(const Player& player) const;
		bool operator!=(const Player& player) const;

		/// Returns the ID of the player
		PlayerId GetId() const;
		/// Retusn the piece of the given type owned by this player
		Piece* GetPiece(PieceType type) const;
		/// Returns a vector of all pieces owned by this player
		std::vector<Piece*> GetPieces() const;
		/// Returns a vector of all the pieces this player has in the hand (not in play)
		std::vector<Piece*> GetPiecesInHand() const;
	private:
		PlayerId mId;
		std::vector<std::unique_ptr<Piece>> mPieces;
	};
}
