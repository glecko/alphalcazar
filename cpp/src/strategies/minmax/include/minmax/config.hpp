#pragma once

#include "minmax/minmax_aliases.hpp"
#include "game/parameters.hpp"

#include <array>

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief The score associated with a move/board position that makes the player win.
	 * Should be high enough to completely override any other possible score accumulations.
	 *
	 * \note A loss will be represented with the negated win condition score.
	 */
	constexpr Score c_WinConditionScore = 10000;
	/*!
	 * \brief A penalty that will be subtracted from a movement's score each movement for each level of depth.
	 *
	 * This will ensure that if several moves lead to the same position/result (a win, for example), the
	 * move that achieves this in the least amount of moves will be chosen. On the contrary, this will ensure
	 * that an inevitable loss will be postponed as much as possible.
	 *
	 * \note The value of this penalty should be low enough to never have an impact on the score apart
	 * from deciding between moves with equal score but different depth.
	 */
	constexpr Score c_DepthScorePenalty = 1;

	constexpr std::array<Score, Game::c_PieceTypes> c_PieceOnBoardScores{{
		80, // Piece 1
		120, // Piece 2
		140, // Piece 3
		-40, // Piece 4
		100 // Piece 5
	}};

	constexpr float c_CenterPieceMultiplier = 2.f;
	constexpr float c_FreshCornerPieceMultiplier = 1.55f;
	constexpr float c_FreshCenterLanePieceMultiplier = 1.7f;
	constexpr float c_PieceAboutToExitMultiplier = 0.7f;
	constexpr float c_LateralCenterLanePieceMultiplier = 1.f;

	// Heuristic values that only apply to placement moves (for sorting purposes)

	/*!
	 * \brief A bonus score added to the pusher (4) piece per piece that the opponent has on the board (excluding perimeter).
	 *
	 * \note Will only be applied if the opponent has at least 2 pieces on the board (could be threatening a potential mate in 1).
	 *
	 * The reason for this is that the pusher piece usually has a higher values on the player's hand than on the board, as it can be used
	 * to avoid otherwise forced wins by the opponent. Therefore playing it when there is no immediate loss threat makes no sense, but as
	 * the opponent gets more pieces on the board, the chance increases that playing it is the best move.
	 *
	 * Its value is set to 30 because at the current heuristic score of the pusher piece (-40), when the opponent has 2 pieces on the board
	 * it will result in a score of 130, making it the second piece to be evaluated (after piece 3), and the highest score if the opponent has
	 * 3 or more pieces on the board. It also makes sure the score of a pusher can't exceed the win score (-40 + 85 * 5 * 1.7 = 655).
	 */
	constexpr Score c_PusherBonusPerOpponentPiece = 85;
}
