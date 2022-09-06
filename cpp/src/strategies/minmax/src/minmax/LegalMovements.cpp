#include "minmax/LegalMovements.hpp"
#include "minmax/config.hpp"

#include <game/Board.hpp>
#include <game/Piece.hpp>
#include <util/Log.hpp>

#include <algorithm>

namespace Alphalcazar::Strategy::MinMax {
	std::pair<bool, bool> GetBoardSymmetries(const Game::Board& board) {
		bool xSymmetry = true;
		bool ySymmetry = true;
		auto [pieces, piecesCount] = board.GetPieces();
		for (std::size_t i = 0; i < piecesCount; ++i) {
			auto& [coordinates, piece] = pieces[i];
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

	/*!
	 * \brief Returns an heuristic approximation of the score we expect to gain from a given placement move, for sorting purposes
	 *
	 * \param move The move for which to calculate the heuristic approximation.
	 * \param board The board of the game for which the legal movement is valid.
	 * \param opponentBoardPieces A list of the pieces the opponent has on the board (excluding perimeter)
	 */
	Score GetHeuristicPlacementMoveScore(const ScoredPlacementMove& move, const Game::Board& board, const std::size_t opponentBoardPieceCount) {
		// First, we check if we have good reason to believe that the movement would result in the placed
		// piece not even entering the board. While this can be beneficial in some very specific situations,
		// most times it would just be a blunder, so it makes sense to assign these movements the lowest score
		if (move.PieceType != Game::c_PusherPieceType) {
			auto placementDirection = move.Coordinates.GetLegalPlacementDirection();
			auto pieceTargetCoordinate = move.Coordinates.GetCoordinateInDirection(placementDirection, 1);
			auto* pieceTargetTile = board.GetTile(pieceTargetCoordinate);
			if (pieceTargetTile->HasPiece()) {
				// There's a piece on the tile that our piece is looking at
				// We check if we can expect the piece to be gone before our piece moves
				// or if the piece can be pushed by us
				auto& pieceTargetTilePiece = pieceTargetTile->GetPiece();
				if (!pieceTargetTilePiece.IsPushable()) {
					auto blockingPieceTargetCoordinate = pieceTargetCoordinate.GetCoordinateInDirection(pieceTargetTilePiece.GetMovementDirection(), 1);
					// We check if the target piece moves after us, or if it will attempt to move to the position where we have placed
					// the piece, causing its movement to be blocked
					if (pieceTargetTilePiece.GetType() > move.PieceType || blockingPieceTargetCoordinate == move.Coordinates) {
						return 0;
					}
				}
			}
		}

		// Once we assume that the piece will be able to enter the board, we use the heuristic score of the piece
		// as a base value for the final heuristic score
		Score resultScore = c_PieceOnBoardScores[move.PieceType - 1];

		// Check the docstring for \ref c_PusherBonusPerOpponentPiece for more information
		if (move.PieceType == Game::c_PusherPieceType && opponentBoardPieceCount >= 2) {
			resultScore += static_cast<Score>(c_PusherBonusPerOpponentPiece * opponentBoardPieceCount);
		}

		// We adjust the score based on if the piece is on the center lane (more valuable) or a lateral lane.
		// We multiply the positive scores and divide the negative scores by the multiplier, as a higher lane multiplier
		// is meant to always increase the absolute value of the move score
		if (move.Coordinates.IsOnCenterLane()) {
			if (resultScore >= 0) {
				resultScore = static_cast<Score>(resultScore * c_FreshCenterLanePieceMultiplier);
			} else {
				resultScore = static_cast<Score>(resultScore / c_FreshCenterLanePieceMultiplier);
			}
		} else {
			if (resultScore >= 0) {
				resultScore = static_cast<Score>(resultScore * c_FreshCornerPieceMultiplier);
			} else {
				resultScore = static_cast<Score>(resultScore / c_FreshCornerPieceMultiplier);
			}
		}

		return resultScore;
	}

	Utils::StaticVector<ScoredPlacementMove, Game::c_MaxLegalMovesCount> SortAndFilterMovements(Game::PlayerId playerId, const Utils::StaticVector<Game::PlacementMove, Game::c_MaxLegalMovesCount>& legalMoves, const Game::Board& board) {
		Utils::StaticVector<ScoredPlacementMove, Game::c_MaxLegalMovesCount> result;

		auto [xSymmetry, ySymmetry] = GetBoardSymmetries(board);
		auto opponentId = playerId == Game::PlayerId::PLAYER_ONE ? Game::PlayerId::PLAYER_TWO : Game::PlayerId::PLAYER_ONE;
		std::size_t opponentBoardPieceCount = board.GetPieceCount(opponentId, true);

		// We build a list of \ref ScoredPlacementMove by looping through the legal moves, removing
		// symmetrical moves and calculating their heuristic score (which will later be used to sort the list)
		if (!xSymmetry && !ySymmetry) {
			// If there are no simmetries on the board, build a list with all moves
			for (std::size_t i = 0; i < legalMoves.size(); ++i) {
				ScoredPlacementMove move{ legalMoves[i] };
				move.Score = GetHeuristicPlacementMoveScore(move, board, opponentBoardPieceCount);
				result.insert(move);
			}
		} else {
			// Else, only insert and calculate the score for non-symmetrical moves
			for (std::size_t i = 0; i < legalMoves.size(); ++i) {
				ScoredPlacementMove move{ legalMoves[i] };
				if (xSymmetry && ySymmetry) {
					// If the board has both x and y symmetry, it must be empty
					// On an empty board, there are really only 2 different tiles on which to play : center or corner
					// We simply select an arbitrary center tile and an arbitrary corner tile: (4, 2) and (4, 3)
					if (move.Coordinates.x == 4 && (move.Coordinates.y == 3 || move.Coordinates.y == 2)) {
						move.Score = GetHeuristicPlacementMoveScore(move, board, opponentBoardPieceCount);
						result.insert(move);
					}
				} else if (xSymmetry) {
					if (move.Coordinates.y >= Game::c_CenterCoordinate) {
						move.Score = GetHeuristicPlacementMoveScore(move, board, opponentBoardPieceCount);
						result.insert(move);
					}
				} else if (ySymmetry) {
					if (move.Coordinates.x >= Game::c_CenterCoordinate) {
						move.Score = GetHeuristicPlacementMoveScore(move, board, opponentBoardPieceCount);
						result.insert(move);
					}
				}
			}
		}

		// Sort the list by the heuristic score of the placement moves
		std::sort(result.begin(), result.end(), [](const ScoredPlacementMove& moveA, ScoredPlacementMove& moveB) {
			return moveA.Score > moveB.Score;
		});

		return result;
	}
}
