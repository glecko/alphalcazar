#pragma once

#include "aliases.hpp"
#include "Coordinates.hpp"
#include "parameters.hpp"
#include "Tile.hpp"
#include "util/StaticVector.hpp"

#include <array>
#include <optional>
#include <functional>
#include <bitset>

namespace Alphalcazar::Game {
	class Piece;

	/*!
	 * \brief Represents the board of an ongoing game.
	 * Is responsible for executing all piece movements and evaluating 3-in-a-row win conditions.
	 */
	class Board {
	public:
		Board();
		~Board();

		/*!
		 * \brief Places a given piece at the tile at the specified coordinates.
		 *
		 * May only be used to place pieces on the perimeter of the board, and automatically sets
		 * the direction of the piece according to the placement direction of the perimeter tile.
		 */
		void PlacePiece(const Coordinates& coordinates, const Piece& piece);
		/*!
		 * \brief Places a given piece at the tile at the specified coordinates with a given direction.
		 *
		 * \note This method should not be called during a game, as it allows placement on any tile with any direction
		 *       without enforcement of the game's rules. It may be used to set up starting positions or by unit tests.
		 */
		void PlacePiece(const Coordinates& coordinates, const Piece& piece, Direction direction);

		/*!
		 * \brief Execute one movement for all pieces currently on the board, in order
		 *
		 * \returns The amount of piece movement that ocurred a result of these movements. Pieces that moved
		 *          multiple times (ex. by moving and later being pushed) are counted multiple times.
		 */
		BoardMovesCount ExecuteMoves(PlayerId startingPlayerId);

		/// Returns if the board tiles (without including the perimeter) have a piece currently on them.
		bool IsFull() const;
		/// Returns the game result based on the current state of the board
		GameResult GetResult() const;
		/// Returns the tile at the given coordinates
		Tile* GetTile(const Coordinates& coord);
		Tile* GetTile(Coordinate x, Coordinate y);
		const Tile* GetTile(const Coordinates& coord) const;
		const Tile* GetTile(Coordinate x, Coordinate y) const;
		/// Returns all tiles of the board
		std::array<const Tile*, c_PlayAreaTileCount> GetTiles() const;
		/// Returns all perimeter tiles of the board
		std::array<const Tile*, c_PerimeterTileCount> GetPerimeterTiles() const;
		/// Returns a list of all coordinates where a player may legally place a piece on their turn
		Utils::StaticVector<Coordinates, c_PerimeterTileCount> GetLegalPlacementCoordinates() const;

		/// Returns the tile a given piece is placed on, or nullptr if the specified piece is not on the board
		Tile* GetPieceTile(const Piece& piece);
		/*!
		 * \brief Returns a list of all pieces in play on the board (including perimeter by default).
		 *
		 * \param player If a valid player ID is specified, the function will only return pieces of this player.
		 * \param excludePerimeter If true, pieces on the perimeter of the board will not be included on the list.
		 */
		Utils::StaticVector<std::pair<Coordinates, Piece>, c_PieceTypes> GetPieces(PlayerId player, bool excludePerimeter = false) const;
		Utils::StaticVector<std::pair<Coordinates, Piece>, c_PieceTypes * 2> GetPieces(bool excludePerimeter = false) const;

		/*!
		 * \brief Returns the amount of pieces a player has on the board.
		 *
		 * Much faster than calling \ref GetPieces if you're only interested in the amount of pieces on the board.
		 *
		 * \param player The player for which to return the piece count.
		 * \param excludePerimeter If true, pieces on the perimeter of the board will not be counted.
		 */
		std::size_t GetPieceCount(PlayerId player, bool excludePerimeter = false) const;
		/*!
		 * \brief Returns a bitset indicating if a player's piece of a certain type is present on the board or not.
		 *
		 * Will return a set of \ref c_PieceTypes bits each indicating if a given piece is present on the board (bit will be set)
		 * or not (bit will be unset). Keep in mind that there will be an offset of 1 between the piece type and the index
		 * (The existance of PieceType 1 will be indicated by the bit at index 0).
		 *
		 * Much faster than running \ref GetPieces if you are only interested in whether a piece exists on the board or not.
		 *
		 * \param player The player for which the piece placements will be returned
		 */
		std::bitset<c_PieceTypes> GetPiecePlacements(PlayerId player) const;
	private:
		/*!
		 * \brief Executes one piece movement, if the specified piece is on the board
		 *
		 * \returns The amount of pieces that were moved as a result of this piece movement. Can be 0 if
		 *          the piece movement was blocked, 1 if it moved normally, 2+ if it pushed other pieces during its movement.
		 */
		BoardMovesCount ExecutePieceMove(const Piece& piece);
		/// Moves the piece from the source tile to the target tile
		void MovePiece(Tile& source, Tile& target, const Coordinates& targetCoordinates);
		/// Removes the piece (if any) on the specified tile
		void RemovePiece(Tile& tile);
		/*!
		 * \brief Returns the ID of the player that has completed the specified row, or nullopt if no player has done so.
		 *
		 * \note A row is considered "complete" when all of the tiles along it are occupied by a piece of the same player.
		 *
		 * \param startCoordinate The coordinate where the row starts.
		 * \param direction The direction we follow to check for row completness, until a perimeter tile is found.
		 */
		std::optional<PlayerId> CheckRowCompleteness(const Coordinates& startCoordinate, Direction direction, Coordinate length) const;

		using MovementDescription = std::tuple<Tile*, Tile*, Coordinates>;
		/*!
		 * \brief Returns a \ref ReversedStaticVector of chained push movements that occur when a piece wants to move from the specified
		 *        source coordinates in the specified direction.
		 *
		 * Each chained movement will be described as a pair of the source tile from which a piece needs to be moved from and
		 * the target tile it needs to be moved to.
		 *
		 * \note The order of the array describes the order the movements are supposed to be executed (with the last piece of the chain
		 *       first and the movement of the pushing piece last).
		 */
		Utils::ReversedStaticVector<Board::MovementDescription, c_PlayAreaSize> GetChainedPushMovements(const Coordinates& sourceCoordinates, Direction direction);

		/// Executes a specified function for every tile of this board
		void LoopOverTiles(std::function<void(const Coordinates& coordinates, const Tile& tile)> action) const;
		void LoopOverTiles(std::function<void(const Coordinates& coordinates, Tile& tile)> action);

		/// Get the coordinates at which a given piece is placed on the board. Returns invalid coordinates if the piece does not exist on the board.
		Coordinates& GetPlacedPieceCoordinates(const Piece& piece);
		/*!
		 * \brief Sets the coordinates at which a piece is located on the board for future calls of \ref GetPlacedPieceCoordinates.
		 *        If we wish to remove the piece from the array, set it to have invalid coordinates.
		 */
		void SetPlacedPieceCoordinates(const Piece& piece, const Coordinates& coordinates);

		/*!
		 * \brief Loops over a [min, max] range of piece placements, fetches the corresponding piece and executed a custom action for each of them.
		 *
		 * \param min The min of the range of the piece placements.
		 * \param min The max of the range of the piece placements.
		 * \param excludePerimeter Whether to skip executing the action for pieces placed on the perimeter of the board.
		 * \param action The action to execute for every piece.
		 */
		void FetchPiecesFromIndexRange(std::size_t min, std::size_t max, bool excludePerimeter, std::function<void(const Coordinates& coordinates, const Piece& piece)> action) const;

		/// 2D array containing all tiles of the board (both perimeter and board tiles) indexed by their coordinates
		std::array<std::array<Tile, c_PlayAreaSize>, c_PlayAreaSize> mTiles;
		/*!
		 * \brief An array containing the coordinates of each piece type, or invalid if the piece is not located on the board.
		 *
		 * The first \ref c_PieceTypes positions of the array are used by the pieces of player 1, and the next
		 * \ref c_PieceTypes positions by the pieces of player 2.
		 *
		 * This information stored (even though it is also available on the tiles) to quickly find the coordinates/tile at which
		 * a piece is located without having to loop over all the tiles.
		 */
		std::array<Coordinates, c_PieceTypes * 2> mPlacedPieceCoordinates;
	};
}
