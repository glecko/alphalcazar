#include "minmax/LegalMovements.hpp"
#include "minmax/config.hpp"

#include <game/Board.hpp>
#include <game/Piece.hpp>
#include <game/parameters.hpp>
#include <game/Coordinates.hpp>
#include <game/PlacementMove.hpp>

#include <util/Log.hpp>

#include <algorithm>

namespace Alphalcazar::Strategy::MinMax {
	std::pair<bool, bool> GetBoardSymmetries(const Game::Board& board) {
		bool xSymmetry = true;
		bool ySymmetry = true;
		for (auto& [coordinates, piece] : board.GetPieces()) {
			auto direction = piece.GetMovementDirection();
			if (coordinates.y != Game::c_CenterCoordinate || direction == Game::Direction::NORTH || direction == Game::Direction::SOUTH) {
				// x-axis symmetry can only exist if all pieces (including perimeter ones) are placed along the horizontal center row
				// and all pieces are pointing east or west. A single piece not following these conditions breaks symmetry.
				xSymmetry = false;
			}
			if (coordinates.x != Game::c_CenterCoordinate || direction == Game::Direction::EAST || direction == Game::Direction::WEST) {
				// y-axis symmetry can only exist if all pieces (including perimeter ones) are placed along the vertical center row
				// and all pieces are pointing north or south. A single piece not following these conditions breaks symmetry.
				ySymmetry = false;
			}
			if (!ySymmetry && !xSymmetry) {
				// If we have already determined that the board has neither x nor y symmatry, there is no point
				// in continuing to loop
				break;
			}
		}
		return std::make_pair(xSymmetry, ySymmetry);
	}

	void FilterSymmetricMovements(std::vector<Game::PlacementMove>& legalMoves, const Game::Board& board) {
		auto symmetries = GetBoardSymmetries(board);
		bool xSymmetry = symmetries.first;
		bool ySymmetry = symmetries.second;

		legalMoves.erase(
			std::remove_if(
				legalMoves.begin(),
				legalMoves.end(),
				[xSymmetry, ySymmetry] (const Game::PlacementMove& move) {
					if (xSymmetry && ySymmetry) {
						// If the board has both x and y symmetry, it must be empty
						// On an empty board, there are really only 2 different tiles on which to play : center or corner
						// We simply select an arbitrary center tile and an arbitrary corner tile: (4, 2) and (4, 3)
						return move.Coordinates.x != 4 || (move.Coordinates.y != 3 && move.Coordinates.y != 2);
					} else if (xSymmetry) {
						return move.Coordinates.y < Game::c_CenterCoordinate;
					} else if (ySymmetry) {
						return move.Coordinates.x < Game::c_CenterCoordinate;
					}
					// Do not delete any legal movements if there is no symmetry of any kind
					return false;
				}
			),
			legalMoves.end()
		);
	}

	/// Returns an heuristic approximation of the score we expect to gain from a given placement move, for sorting purposes
	Score GetHeuristicPlacementMoveScore(const Game::PlacementMove& move, const Game::Board& board) {
		// First, we check if we have good reason to believe that the movement would result in the placed
		// piece not even entering the board. While this can be beneficial in some very specific situations,
		// most times it would just be a blunder, so it makes sense to assign these movements the lowest score
		if (move.PieceType != Game::c_PusherPieceType) {
			if (auto* moveTile = board.GetTile(move.Coordinates)) {
				auto placementDirection = moveTile->GetLegalPlacementDirection();
				auto pieceTargetCoordinate = move.Coordinates.GetCoordinateInDirection(placementDirection, 1);
				if (auto* pieceTargetTilePiece = board.GetTile(pieceTargetCoordinate)->GetPiece()) {
					// There's a piece on the tile that our piece is looking at
					// We check if we can expect the piece to be gone before our piece moves
					// or if the piece can be pushed by us
					if (!pieceTargetTilePiece->IsPushable()) {
						auto blockingPieceTargetCoordinate = pieceTargetCoordinate.GetCoordinateInDirection(pieceTargetTilePiece->GetMovementDirection(), 1);
						// We check if the target piece moves after us, or if it will attempt to move to the position where we have placed
						// the piece, causing its movement to be blocked
						if (pieceTargetTilePiece->GetType() > move.PieceType || blockingPieceTargetCoordinate == move.Coordinates) {
							return 0;
						}
					}
				}
			} else {
				Utils::LogError("Legal move pointed at {}, but no board tile was found at those coordinates.", move.Coordinates);
			}
		}

		// Once we assume that the piece will be able to enter the board, we assign a score based on the piece type and target row
		Score expectedPieceScore = c_PieceOnBoardScores[move.PieceType - 1];
		if (move.Coordinates.IsOnCenterLane()) {
			return expectedPieceScore * c_CenterLanePieceMultiplier;
		} else {
			return expectedPieceScore * c_LateralLanePieceMultiplier;
		}
	}

	void SortLegalMovements(std::vector<Game::PlacementMove>& legalMoves, const Game::Board& board) {
		std::sort(legalMoves.begin(), legalMoves.end(), [board](const Game::PlacementMove& moveA, const Game::PlacementMove& moveB) {
			// Sort the vector by the heuristic score of the placement moves
			return GetHeuristicPlacementMoveScore(moveA, board) > GetHeuristicPlacementMoveScore(moveB, board);
		});
	}
}
