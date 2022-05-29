#include "LegalMovements.hpp"
#include "config.hpp"

#include <Game.hpp>
#include <Piece.hpp>
#include <parameters.hpp>
#include <Coordinates.hpp>
#include <PlacementMove.hpp>

namespace Alphalcazar::Strategy::MinMax {
	std::pair<bool, bool> GetBoardSymmetries(const Game::Game& game) {
		bool xSymmetry = true;
		bool ySymmetry = true;
		for (auto& [coordinates, piece] : game.GetBoard().GetPieces()) {
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

	void FilterSymmetricMovements(std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game) {
		auto symmetries = GetBoardSymmetries(game);
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
}
