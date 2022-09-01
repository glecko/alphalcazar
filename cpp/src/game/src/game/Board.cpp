#include "game/Board.hpp"

#include "game/Piece.hpp"
#include "game/board_utils.hpp"
#include <util/Log.hpp>

#include <algorithm>

namespace {
	/// Returns the index of the \ref mPlacedPieceCoordinates array of a given piece
	std::size_t GetPlacedPieceTypeIndex(const Alphalcazar::Game::Piece& piece) {
		std::size_t pieceTypeIndex = static_cast<std::size_t>(piece.GetType());
		// The first 5 positions of the array are held by the (1-5) pieces of player one,
		// the next 5 position by the (1-5) pieces of player 2. To get the index, we get the piece index
		// and shift it forward by 5 for player 2 and by 0 for player 1.
		std::size_t pieceOwnerOffset = static_cast<std::size_t>(piece.GetOwner()) - 1;
		return pieceTypeIndex + (pieceOwnerOffset * Alphalcazar::Game::c_PieceTypes) - 1;
	}

	/// Returns the [min, max] index range at which the coordinates of the pieces of a given player are located on the \ref mPlacedPieceCoordinates array
	std::pair<std::size_t, std::size_t> GetPlacePieceIndexRange(Alphalcazar::Game::PlayerId playerId) {
		// See the docstring of \ref mPlacedPieceCoordinates for more information.
		std::size_t min = 0;
		std::size_t max = Alphalcazar::Game::c_PieceTypes * 2 - 1;
		switch (playerId) {
		case Alphalcazar::Game::PlayerId::PLAYER_ONE:
			max = Alphalcazar::Game::c_PieceTypes - 1;
			break;
		case Alphalcazar::Game::PlayerId::PLAYER_TWO:
			min = Alphalcazar::Game::c_PieceTypes;
			break;
		default:
			// Keep the default min/max
			break;
		}
		return std::make_pair(min, max);
	}
}

namespace Alphalcazar::Game {
	Board::Board() {}

	Board::~Board() {}

	void Board::PlacePiece(const Coordinates& coordinates, const Piece& piece) {
		Tile* tile = GetTile(coordinates);
		if constexpr (c_BoardPiecePlacementIntegrityChecks) {
			if (!tile) {
				Utils::LogError("Attempted to place a piece on a non-existing perimeter tile (at {})", coordinates);
			}
		}
		auto direction = coordinates.GetLegalPlacementDirection();
		if (direction != Direction::NONE) {
			tile->PlacePiece(piece);
			tile->GetPiece().SetMovementDirection(coordinates.GetLegalPlacementDirection());

			SetPlacedPieceCoordinates(piece, coordinates);
		}
	}

	void Board::PlacePiece(const Coordinates& coordinates, const Piece& piece, Direction direction) {
		Tile* tile = GetTile(coordinates);
		if constexpr (c_BoardPiecePlacementIntegrityChecks) {
			if (!tile) {
				Utils::LogError("Attempted to place a piece on a non-existing tile (at {})", coordinates);
			}
		}
		tile->PlacePiece(piece);
		tile->GetPiece().SetMovementDirection(direction);

		SetPlacedPieceCoordinates(piece, coordinates);
	}

	BoardMovesCount Board::ExecutePieceMove(const Piece& piece) {
		BoardMovesCount movedPieces = 0;
		// Check if the piece to move exists at some coordinate on the board
		const Coordinates& originCoordinates = GetPlacedPieceCoordinates(piece);
		if (originCoordinates.Valid()) {
			Tile* originTile = GetTile(originCoordinates);
			Piece* originPiece = originTile->GetPiece();

			auto direction = originPiece->GetMovementDirection();
			Coordinates targetCoordinates = originCoordinates.GetCoordinateInDirection(direction, 1);
			if (Tile* targetTile = GetTile(targetCoordinates)) {
				Piece* targetTilePiece = targetTile->GetPiece();
				if (targetTilePiece == nullptr) {
					MovePiece(*originTile, *targetTile, targetCoordinates);
					movedPieces++;
				} else if (originPiece->IsPusher()) {
					auto chainedMovements = GetChainedPushMovements(originCoordinates, direction);
					// We execute the chained push movements in order
					for (auto& [sourcePushTile, targetPushTile, targetPushCoordinate] : chainedMovements) {
						if (targetPushTile != nullptr) {
							MovePiece(*sourcePushTile, *targetPushTile, targetPushCoordinate);
						} else {
							// If a piece is pushed to a non-existing tile (happens when a piece that is on the perimeter is pushed
							// further outwards of the board) we simply remove the piece from play
							RemovePiece(*sourcePushTile);
						}
						movedPieces++;
					}
				} else if (targetTilePiece->IsPushable() && !originPiece->IsPushable()) {
					auto pushTargetCoordinates = originCoordinates.GetCoordinateInDirection(direction, 2);
					auto* pushTargetTile = GetTile(pushTargetCoordinates);
					// A non-pushing piece cannot push a pushable piece if there is a piece on the tile
					// it would be pushed to. It doesn't matter if the piece behind it is also pushable
					if (!pushTargetTile->HasPiece()) {
						// We first move the pushed piece, then the pushing piece
						MovePiece(*targetTile, *pushTargetTile, pushTargetCoordinates);
						MovePiece(*originTile, *targetTile, targetCoordinates);
						movedPieces += 2;
					} else if (originCoordinates.IsPerimeter()) {
						// if a piece was unable to perform any movement on its turn while sitting on
						// a perimeter tile, it is immediatelly removed from play
						RemovePiece(*originTile);
					}
				} else if (originCoordinates.IsPerimeter()) {
					// if a piece was unable to perform any movement on its turn while sitting on
					// a perimeter tile, it is immediatelly removed from play
					RemovePiece(*originTile);
				}
			}
		}
		return movedPieces;
	}

	BoardMovesCount Board::ExecuteMoves(PlayerId startingPlayerId) {
		BoardMovesCount movedPieces = 0;
		for (PieceType pieceTypeToMove = 1; pieceTypeToMove <= c_PieceTypes; pieceTypeToMove++) {
			Piece startingPlayerPiece { startingPlayerId, pieceTypeToMove };
			movedPieces += ExecutePieceMove(startingPlayerPiece);
			Piece secondPlayerPiece { startingPlayerId == PlayerId::PLAYER_ONE ? PlayerId::PLAYER_TWO : PlayerId::PLAYER_ONE, pieceTypeToMove };
			movedPieces += ExecutePieceMove(secondPlayerPiece);
		}
		return movedPieces;
	}

	std::vector<Board::MovementDescription> Board::GetChainedPushMovements(const Coordinates& sourceCoordinates, Direction direction) {
		std::vector<Board::MovementDescription> result;
		Coordinates nextCoordinate = sourceCoordinates;
		Tile* nextTile = GetTile(nextCoordinate);
		while (nextTile && nextTile->HasPiece()) {
			// Get the coordinates and tile this piece will be pushed to
			auto pushToCoordinate = nextCoordinate.GetCoordinateInDirection(direction, 1);
			auto pushToTile = GetTile(pushToCoordinate);

			// Insert this movement (from the current tile to the tile we will push to) at the beginning
			// of the vector, as the pushing movements will need to happen in order
			// from the last piece on the chain to the first (pushing) piece
			MovementDescription movement = std::make_tuple(nextTile, pushToTile, pushToCoordinate);
			result.insert(result.begin(), movement);

			// The tile and coordinates we are pushing to become the next tile/coordinates we evaluate
			nextCoordinate = pushToCoordinate;
			nextTile = pushToTile;
		}
		return result;
	}

	void Board::MovePiece(Tile& source, Tile& target, const Coordinates& targetCoordinates) {
		if (auto* piece = source.GetPiece()) {
			// A piece that moves or is moved to a perimeter tile gets removed from play immediatelly
			if (!targetCoordinates.IsPerimeter()) {
				SetPlacedPieceCoordinates(*piece, targetCoordinates);
				target.PlacePiece(*piece);
			} else {
				SetPlacedPieceCoordinates(*piece, Coordinates::Invalid());
			}
			source.RemovePiece();
		}
	}

	void Board::RemovePiece(Tile& tile) {
		if (auto* piece = tile.GetPiece()) {
			SetPlacedPieceCoordinates(*piece, Coordinates::Invalid());
			tile.RemovePiece();
		}
	}

	Tile* Board::GetTile(const Coordinates& coord) {
		if (coord.IsPlayArea()) {
			return &mTiles[coord.x][coord.y];
		}
		return nullptr;
	}

	Tile* Board::GetTile(Coordinate x, Coordinate y) {
		Coordinates coord { x, y };
		return GetTile(coord);
	}

	const Tile* Board::GetTile(const Coordinates& coord) const {
		if (coord.IsPlayArea()) {
			return &mTiles[coord.x][coord.y];
		}
		return nullptr;
	}

	const Tile* Board::GetTile(Coordinate x, Coordinate y) const {
		Coordinates coord{ x, y };
		return GetTile(coord);
	}

	Tile* Board::GetPieceTile(const Piece& piece) {
		Coordinates& coordinates = GetPlacedPieceCoordinates(piece);
		if (coordinates.Valid()) {
			return GetTile(coordinates);
		}
		return nullptr;
	}

	std::vector<const Tile*> Board::GetTiles() const {
		std::vector<const Tile*> result;
		result.reserve(mTiles.size());
		LoopOverTiles([&result](const Coordinates&, const Tile& tile) {
			result.push_back(&tile);
		});
		return result;
	}

	std::vector<const Tile*> Board::GetPerimeterTiles() const {
		std::vector<const Tile*> result;
		result.reserve(mTiles.size());
		LoopOverTiles([&result](const Coordinates& coordinates, const Tile& tile) {
			if (coordinates.IsPerimeter()) {
				result.push_back(&tile);
			}
		});
		return result;
	}

	std::vector<Coordinates> Board::GetLegalPlacementCoordinates() const {
		std::vector<Coordinates> result;
		constexpr auto perimeterCoordinates = Coordinates::GetPerimeterCoordinates();
		// Allocating the maximum amount of memory the vector could potentially
		// take up. Will waste memory sometimes but we'll use all of it most of the times,
		// and memory usage is not as much of an issue as CPU usage here
		result.reserve(perimeterCoordinates.size());
		for (auto& coordinates : perimeterCoordinates) {
			auto* tile = GetTile(coordinates);
			if (!tile->HasPiece()) {
				result.push_back(coordinates);
			}
		}
		return result;
	}

	bool Board::IsFull() const {
		bool result = true;
		LoopOverTiles([&result](const Coordinates& coordinates, const Tile& tile) {
			// If we find a single non-perimetter file that has no piece on it, we return false
			if (!coordinates.IsPerimeter() && !tile.HasPiece()) {
				result = false;
				return;
			}
		});
		return result;
	}

	GameResult Board::GetResult() const {
		std::optional<PlayerId> candidateWinner = std::nullopt;

		// The list of coordinates/directions to check only depends on the board size constant
		// and can be evaluated at compile time for better runtime performance
		constexpr auto c_BoardRows = GetAllRowIterationDirections();
		for (auto& [x, y, direction, distance] : c_BoardRows) {
			Coordinates startCoordinate { x, y };
			if (auto playerId = CheckRowCompleteness(startCoordinate, direction, distance); playerId.has_value()) {
				if (candidateWinner.has_value() && candidateWinner != playerId) {
					// Both players have completed at least one row/column/diagonal
					if constexpr (c_AcceptDraws) {
						return GameResult::DRAW;
					}
					return GameResult::NONE;
				}
				candidateWinner = playerId.value();
			}
		}

		if (candidateWinner.has_value()) {
			return candidateWinner.value() == PlayerId::PLAYER_ONE ? GameResult::PLAYER_ONE_WINS : GameResult::PLAYER_TWO_WINS;
		}
		return GameResult::NONE;
	}

	std::optional<PlayerId> Board::CheckRowCompleteness(const Coordinates& startCoordinate, Direction direction, Coordinate length) const {
		// Here we keep track of the player that has a potential of completing this row.
		// A single candidate is considered since a row will never be completed by a player if it has pieces of
		// both players on it.
		std::optional<PlayerId> candidateRowCompleter = std::nullopt;

		for (Coordinate distance = 0; distance < length; distance++) {
			Coordinates nextCoordinate = startCoordinate.GetCoordinateInDirection(direction, distance);
			if (auto piece = mTiles[nextCoordinate.x][nextCoordinate.y].GetPiece()) {
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
		}
		return candidateRowCompleter;
	}

	Coordinates& Board::GetPlacedPieceCoordinates(const Piece& piece) {
		std::size_t index = GetPlacedPieceTypeIndex(piece);
		return mPlacedPieceCoordinates[index];
	}

	void Board::SetPlacedPieceCoordinates(const Piece& piece, const Coordinates& coordinates) {
		std::size_t index = GetPlacedPieceTypeIndex(piece);
		mPlacedPieceCoordinates[index] = coordinates;
	}

	std::vector<std::pair<Coordinates, Piece>> Board::GetPieces(PlayerId player, bool excludePerimeter) const {
		std::vector<std::pair<Coordinates, Piece>> result;
		// We preallocate space in the vector according to the max amount of pieces that could
		// fit in the vector if all relevant pieces were on the board. Will waste memory sometimes
		// but memory usage is not as much of an issue as CPU usage here.
		result.reserve(player == PlayerId::NONE ? c_PieceTypes * 2 : c_PieceTypes);

		auto [min, max] = GetPlacePieceIndexRange(player);
		for (std::size_t i = min; i <= max; i++) {
			auto coordinates = mPlacedPieceCoordinates[i];
			if (coordinates.Valid()) {
				if (excludePerimeter && coordinates.IsPerimeter()) {
					continue;
				}
				const Tile* tile = GetTile(coordinates);
				if constexpr (c_BoardPiecePlacementIntegrityChecks) {
					if (!tile) {
						Utils::LogError("Placed pieces cache pointed at {} for index {}, but no tile exists at those coordinates.", coordinates, i);
					}
				}
				if constexpr (c_BoardPiecePlacementIntegrityChecks) {
					if (!tile->HasPiece()) {
						Utils::LogError("Placed pieces cache pointed at {} for index {}, but no piece exists at the tile at those coordinates.", coordinates, i);
					}
				}
				result.emplace_back(coordinates, tile->GetPiece());
			}
		}
		return result;
	}

	std::size_t Board::GetPieceCount(PlayerId player, bool excludePerimeter) const {
		if (player == PlayerId::NONE) {
			return 0;
		}
		std::size_t pieceCount = 0;
		auto [min, max] = GetPlacePieceIndexRange(player);
		for (std::size_t i = min; i <= max; i++) {
			auto coordinates = mPlacedPieceCoordinates[i];
			if (coordinates.Valid()) {
				if (!excludePerimeter || !coordinates.IsPerimeter()) {
					pieceCount++;
				}
			}
		}
		return pieceCount;
	}

	std::bitset<c_PieceTypes> Board::GetPiecePlacements(PlayerId player) const {
		std::bitset<c_PieceTypes> result;
		if (player == PlayerId::NONE) {
			return result;
		}
		auto [min, max] = GetPlacePieceIndexRange(player);
		for (std::size_t i = min; i <= max; i++) {
			auto coordinates = mPlacedPieceCoordinates[i];
			result[i - min] = coordinates.Valid();
		}
		return result;
	}

	void Board::LoopOverTiles(std::function<void(const Coordinates& coordinates, const Tile& tile)> action) const {
		for (Coordinate x = 0; x <= c_PlayAreaSize - 1; x++) {
			for (Coordinate y = 0; y <= c_PlayAreaSize - 1; y++) {
				Coordinates coordinates { x, y };
				if (coordinates.IsCorner()) {
					// The corners of the play area don't exist
					continue;
				}
				action(coordinates, mTiles[x][y]);
			}
		}
	}

	void Board::LoopOverTiles(std::function<void(const Coordinates& coordinates, Tile& tile)> action) {
		for (Coordinate x = 0; x <= c_PlayAreaSize - 1; x++) {
			for (Coordinate y = 0; y <= c_PlayAreaSize - 1; y++) {
				Coordinates coordinates { x, y };
				if (coordinates.IsCorner()) {
					// The corners of the play area don't exist
					continue;
				}
				action(coordinates, mTiles[x][y]);
			}
		}
	}
}