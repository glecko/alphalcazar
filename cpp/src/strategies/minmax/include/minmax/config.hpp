#pragma once

#include "minmax/minmax_aliases.hpp"
#include "game/parameters.hpp"
#include "game/Coordinates.hpp"

#include <array>

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief The score associated with a move/board position that makes the player win.
	 * Should be high enough to completely override any other possible score accumulations.
	 *
	 * \note A loss will be represented with the negated win condition score.
	 */
	constexpr Score c_WinConditionScore = 1000;
	/*!
	 * \brief A penalty that will be subtracted from a movement's score each movement for each level of depth.
	 *
	 * This will ensure that if several moves lead to the same position/result (a win, for example), the
	 * move that achieves this in the least amount of moves will be chosen. On the contrary, this will ensure
	 * that an inevitable loss will be posponed as much as possible.
	 *
	 * \note The value of this penalty should be low enough to never have an impact on the score appart
	 * from deciding between moves with equal score but different depth.
	 */
	constexpr Score c_DepthScorePenalty = 1;

	constexpr std::array<Score, Game::c_PieceTypes> c_PieceOnBoardScores{{
		80, // Piece 1
		120, // Piece 2
		140, // Piece 3
		-80, // Piece 4
		100 // Piece 5
	}};

	constexpr float c_CenterPieceMultiplier = 2.f;
	constexpr float c_FreshCornerPieceMultiplier = 1.55f;
	constexpr float c_FreshCenterLanePieceMultiplier = 1.7f;
	constexpr float c_PieceAboutToExitMultiplier = 0.7f;
	constexpr float c_LateralCenterLanePieceMultiplier = 1.f;
}
