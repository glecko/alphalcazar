#include "Board.hpp"

#include "parameters.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Piece.hpp"
#include <algorithm>

namespace Alphalcazar::Game {
	Board::Board() {
		for (Coordinate x = 0; x <= c_PlayAreaSize - 1; x++) {
			for (Coordinate y = 0; y <= c_PlayAreaSize - 1; y++) {
				Coordinates coordinates { x, y };
				if (coordinates.IsCorner()) {
					// Avoid creating tiles for corners of the play area
					continue;
				}
				mTiles[coordinates] = std::make_unique<Tile>(coordinates);
			}
		}
	}

	Board::Board(const Board& other, std::vector<Piece*> newPieces) {
		for (auto& [coordinates, tile] : other.mTiles) {
			mTiles[coordinates] = std::make_unique<Tile>(coordinates);
			auto piece = tile->GetPiece();
			if (piece != nullptr) {
				auto newPieceIt = std::find(newPieces.begin(), newPieces.end(), piece);
				mTiles[coordinates]->PlacePiece(*newPieceIt);
			}
		}
	}

	Board::~Board() {}

	void Board::PlacePiece(const Coordinates& coordinates, Piece* piece) {
		if (auto* tile = GetTile(coordinates)) {
			tile->PlacePiece(piece);
		}
	}

	BoardMovesCount Board::ExecuteMoves(PlayerId startingPlayerId) {
		BoardMovesCount movedPieces = 0;
		auto pieces = GetMovementOrderedPieces(startingPlayerId);
		for (auto* piece : pieces) {
			if (!piece->IsInPlay()) {
				// Check if the piece has been removed from play by another piece earlier in the loop
				// (by being pushed outside the board) in which case this piece will not execute any movement
				continue;
			}
			auto direction = piece->GetMovementDirection();
			const auto& sourceCoordinates = piece->GetCoordinates();
			auto* sourceTile = GetTile(sourceCoordinates);
			auto targetCoordinates = sourceCoordinates.GetCoordinateInDirection(direction, 1);
			if (auto* targetTile = GetTile(targetCoordinates)) {
				auto* targetTilePiece = targetTile->GetPiece();
				if (targetTilePiece == nullptr) {
					MovePiece(piece, *sourceTile, *targetTile);
					movedPieces++;
				} else if (piece->IsPusher()) {
					auto chainedMovements = GetChainedPushMovements(sourceCoordinates, direction);
					// We execute the chained push movements in order
					for (auto& [sourcePushTile, targetPushTile] : chainedMovements) {
						if (targetPushTile != nullptr) {
							MovePiece(sourcePushTile->GetPiece(), *sourcePushTile, *targetPushTile);
						} else {
							// If a piece is pushed to a non-existing tile (happens when a piece that is on the perimeter is pushed
							// further outwards of the board) we simply remove the piece from play
							sourcePushTile->RemovePiece();
						}
						movedPieces++;
					}
				} else if (targetTilePiece->IsPushable()) {
					auto pushTargetCoordinates = sourceCoordinates.GetCoordinateInDirection(direction, 2);
					auto* pushTargetTile = GetTile(pushTargetCoordinates);
					// A non-pushing piece cannot push a pushable piece if there is a piece on the tile
					// it would be pushed to. It doesn't matter if the piece behind it is also pushable
					if (pushTargetTile->GetPiece() == nullptr) {
						// We first move the pushed piece, then the pushing piece
						MovePiece(targetTilePiece, *targetTile, *pushTargetTile);
						MovePiece(piece, *sourceTile, *targetTile);
						movedPieces += 2;
					} else if (sourceCoordinates.IsPerimeter()) {
						// if a piece was unable to perform any movement on its turn while sitting on
						// a perimeter tile, it is immediatelly removed from play
						sourceTile->RemovePiece();
					}
				} else if (sourceCoordinates.IsPerimeter()) {
					// if a piece was unable to perform any movement on its turn while sitting on
					// a perimeter tile, it is immediatelly removed from play
					sourceTile->RemovePiece();
				}
			}
		}
		return movedPieces;
	}

	std::vector<Board::MovementDescription> Board::GetChainedPushMovements(const Coordinates& sourceCoordinates, Direction direction) const {
		std::vector<Board::MovementDescription> result;
		Coordinates nextCoordinate = sourceCoordinates;
		Tile* nextTile = GetTile(nextCoordinate);
		while (nextTile && nextTile->GetPiece() != nullptr) {
			// Get the coordinates and tile this piece will be pushed to
			auto pushToCoordinate = nextCoordinate.GetCoordinateInDirection(direction, 1);
			auto pushToTile = GetTile(pushToCoordinate);

			// Insert this movement (from the current tile to the tile we will push to) at the beginning
			// of the vector, as the pushing movements will need to happen in order
			// from the last piece on the chain to the first (pushing) piece
			MovementDescription movement = std::make_pair(nextTile, pushToTile);
			result.insert(result.begin(), movement);

			// The tile and coordinates we are pushing to become the next tile/coordinates we evaluate
			nextCoordinate = pushToCoordinate;
			nextTile = pushToTile;
		}
		return result;
	}

	void Board::MovePiece(Piece* piece, Tile& source, Tile& target) {
		source.RemovePiece();
		// A piece that moves or is moved to a perimeter tile gets removed from play immediatelly
		if (!target.GetCoordinates().IsPerimeter()) {
			target.PlacePiece(piece);
		}
	}

	Tile* Board::GetTile(const Coordinates& coord) const {
		if (mTiles.find(coord) != mTiles.end()) {
			return mTiles.at(coord).get();
		}
		return nullptr;
	}

	Tile* Board::GetTile(Coordinate x, Coordinate y) const {
		Coordinates coord { x, y };
		return GetTile(coord);
	}

	std::vector<Tile*> Board::GetTiles() const {
		std::vector<Tile*> result;
		result.reserve(mTiles.size());
		for (auto& [_, tile] : mTiles) {
			result.push_back(tile.get());
		}
		return result;
	}

	bool Board::IsFull() const {
		for (auto& [coordinates, tile] : mTiles) {
			// If we find a single non-perimetter file that has no piece on it, we return false
			if (!coordinates.IsPerimeter() && tile->GetPiece() == nullptr) {
				return false;
			}
		}
		return true;
	}

	GameResult Board::GetResult() const {
		std::optional<PlayerId> candidateWinner = std::nullopt;

		auto rows = GetAllRowIterationDirections();
		for (auto& [startCoordinate, direction] : rows) {
			if (auto playerId = CheckRowCompleteness(startCoordinate, direction); playerId.has_value()) {
				if (candidateWinner.has_value() && candidateWinner != playerId) {
					// Both players have completed at least one row/column/diagonal
					return c_AcceptDraws ? GameResult::DRAW : GameResult::NONE;
				}
				candidateWinner = playerId.value();
			}
		}

		if (candidateWinner.has_value()) {
			return candidateWinner.value() == PlayerId::PLAYER_ONE ? GameResult::PLAYER_ONE_WINS : GameResult::PLAYER_TWO_WINS;
		}
		return GameResult::NONE;
	}

	std::vector<std::pair<Coordinates, Direction>> Board::GetAllRowIterationDirections() const {
		std::vector<std::pair<Coordinates, Direction>> result;
		static constexpr std::size_t c_TotalBoardRows = 2 * c_BoardSize + 2;
		result.reserve(c_TotalBoardRows);

		// Add all columns by starting at their southmost tile of each and iterating north
		for (Coordinate x = 1; x <= c_BoardSize; x++) {
			Coordinates startCoordinate{ x, 1 };
			result.emplace_back(startCoordinate, Direction::NORTH);
		}

		// Add all rows by starting at the westmost tile of each and iterating east
		for (Coordinate y = 1; y <= c_BoardSize; y++) {
			Coordinates startCoordinate{ 1, y };
			result.emplace_back(startCoordinate, Direction::EAST);
		}

		// Add both diagonals
		Coordinates southWestDiagonalStartCoordinate { 1, 1 };
		result.emplace_back(southWestDiagonalStartCoordinate, Direction::NORTH_EAST);

		Coordinates southEastDiagonalStartCoordinate{ c_BoardSize, 1 };
		result.emplace_back(southEastDiagonalStartCoordinate, Direction::NORTH_WEST);

		return result;
	}

	std::optional<PlayerId> Board::CheckRowCompleteness(const Coordinates& startCoordinate, Direction direction) const {
		// Here we keep track of the player that has a potential of completing this row.
		// A single candidate is considered since a row will never be completed by a player if it has pieces of
		// both players on it.
		std::optional<PlayerId> candidateRowCompleter = std::nullopt;

		Coordinates nextCoordinate = startCoordinate;
		while (!nextCoordinate.IsPerimeter()) {
			if (auto* piece = mTiles.at(nextCoordinate)->GetPiece()) {
				if (candidateRowCompleter == std::nullopt) {
					// We appoint the owner of the first piece we find as our candidate
					candidateRowCompleter = piece->GetOwner();
				} else if (candidateRowCompleter != piece->GetOwner()) {
					// If we find a piece that doesn't belong to the already determined candidate for completing the row
					// we return nullopt as we know the row won't be complete
					return std::nullopt;
				}
			} else {
				// If a single tile of this row has no piece, it will not be complete
				return std::nullopt;
			}
			nextCoordinate = nextCoordinate.GetCoordinateInDirection(direction, 1);
		}
		return candidateRowCompleter;
	}

	std::vector<Piece*> Board::GetMovementOrderedPieces(PlayerId startingPlayerId) const {
		std::vector<Piece*> result;
		for (auto& [coordinates, tile] : mTiles) {
			if (auto* piece = tile->GetPiece()) {
				result.push_back(piece);
			}
		}

		// Sort by movement order
		std::sort(result.begin(), result.end(), [startingPlayerId](Piece* pieceA, Piece* pieceB) {
			if (pieceA->GetType() == pieceB->GetType()) {
				if (pieceA->GetOwner() == startingPlayerId) {
					return true;
				} else {
					return false;
				}
			}
			return pieceB->GetType() > pieceA->GetType();
		});

		return result;
	}
}