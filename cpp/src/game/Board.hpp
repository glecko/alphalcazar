#pragma once

#include "aliases.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include "Coordinates.hpp"

namespace Alphalcazar::Game {
	class Tile;
	class Piece;

	/*!
	 * \brief Represents the board of an ongoing game.
	 * Is responsible for executing all piece movements and evaluating 3-in-a-row win conditions.
	 */
	class Board {
	public:
		Board();
		Board(const Board& other, std::vector<Piece*> pieces);
		~Board();

		/// Places a given piece at the tile at the specified coordinates
		void PlacePiece(const Coordinates& coordinates, Piece* piece);

		/// Execute one movement for all pieces currently on the board, in order
		BoardMovesCount ExecuteMoves(PlayerId startingPlayerId);

		/// Returns if the board tiles (without including the perimeter) have a piece currently on them.
		bool IsFull() const;
		/// Returns the game result based on the current state of the board
		GameResult GetResult() const;
		/// Returns the tile at the given coordinates
		Tile* GetTile(const Coordinates& coord) const;
		Tile* GetTile(Coordinate x, Coordinate y) const;
		/// 
		/*!
		 * \brief Returns the perimeter tile at the given coordinates.
		 * Slightly faster than GetTile() if we know we are searching for a perimeter tile.
		 */
		Tile* GetPerimeterTile(const Coordinates& coord) const;
		/// Returns all tiles of the board
		std::vector<Tile*> GetTiles() const;
		/// Returns all perimeter tiles of the board
		std::vector<Tile*> GetPerimeterTiles() const;
		/// Returns a list of all tiles where a player may legally place a piece on their turn
		std::vector<Tile*> GetLegalPlacementTiles() const;
	private:
		/// Moves the specified piece from the source tile to the target tile
		void MovePiece(Piece* piece, Tile& source, Tile& target);
		std::vector<Piece*> GetMovementOrderedPieces(PlayerId startingPlayerId) const;
		/*!
		 * \brief Returns the ID of the player that has completed the specified row, or nullptr if no player has done so.
		 * 
		 * \note A row is considered "complete" when all of the tiles along it are occupied by a piece of the same player.
		 * 
		 * \param startCoordinate The coordinate where the row starts.
		 * \param direction The direction we follow to check for row completness, until a perimeter tile is found.
		 */
		std::optional<PlayerId> CheckRowCompleteness(const Coordinates& startCoordinate, Direction direction, Coordinate length) const;

		using MovementDescription = std::pair<Tile*, Tile*>;
		/*!
		 * \brief Returns a list of chained push movements that occur when a piece wants to move from the specified
		 *        source coordinates in the specified direction.
		 *
		 * Each chained movement will be described as a pair of the source tile from which a piece needs to be moved from and
		 * the target tile it needs to be moved to.
		 * 
		 * \note The list will be returned in the order the movements are supposed to be executed (with the last piece of the chain
		 * first and the movement of the pushing piece last)
		 */
		std::vector<MovementDescription> GetChainedPushMovements(const Coordinates& sourceCoordinates, Direction direction) const;

		/// A map of all tiles of the board (both perimeter and board tiles) indexed by their coordinates
		std::unordered_map<Coordinates, std::unique_ptr<Tile>> mTiles;
		/// A map of the perimeter tiles of the board indexed by their coordinates
		std::unordered_map<Coordinates, Tile*> mPerimeterTiles;
	};
}
