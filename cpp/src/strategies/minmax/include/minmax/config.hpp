#pragma once

#include "minmax/minmax_aliases.hpp"
#include "game/parameters.hpp"
#include "game/Coordinates.hpp"

#include <array>
#include <unordered_map>

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief The score associated with a move/board position that makes the player win.
	 * Should be high enough to completely override any other possible score accumulations.
	 *
	 * \note A loss will be represented with the negated win condition score.
	 */
	static constexpr Score c_WinConditionScore = 1000;
	/// The initial value of the "alpha" parameter of the minmax algorithm
	static constexpr Score c_AlphaStartingValue = -c_WinConditionScore * 10;
	/// The initial value of the "beta" parameter of the minmax algorithm
	static constexpr Score c_BetaStartingValue = c_WinConditionScore * 10;
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
	static constexpr Score c_DepthScorePenalty = 1;

	static constexpr std::array<Score, Game::c_PieceTypes> c_PieceOnBoardScores{{
		80, // Piece 1
		120, // Piece 2
		140, // Piece 3
		-80, // Piece 4
		100 // Piece 5
	}};

	static constexpr float c_CenterPieceMultiplier = 2.f;
	static constexpr float c_FreshCornerPieceMultiplier = 1.55f;
	static constexpr float c_FreshCenterLanePieceMultiplier = 1.7f;
	static constexpr float c_PieceAboutToExitMultiplier = 0.7f;
	static constexpr float c_LateralCenterLanePieceMultiplier = 1.f;

	/*!
	 * \brief A map of score multipliers in relation to all combinations of coordinates and piece directions.
	 *
	 * The multipliers of this map will be multiplied with the piece score of each piece on the board when evaluating a total board score.
	 * The general idea is to give higher score values to pieces that have strategic locations (like the center) or have a long expected
	 * lifespan (just entered the board and have 3 movements ahead), and lower scores to pieces that are about to exit the board.
	 */
	static std::unordered_map<std::pair<Game::Coordinates, Game::Direction>, float> c_CoordinatesScoreMultiplier {
		// Center
		{ { { 2, 2 }, Game::Direction::NORTH }, c_CenterPieceMultiplier },
		{ { { 2, 2 }, Game::Direction::SOUTH }, c_CenterPieceMultiplier },
		{ { { 2, 2 }, Game::Direction::EAST }, c_CenterPieceMultiplier },
		{ { { 2, 2 }, Game::Direction::WEST }, c_CenterPieceMultiplier },

		// Corners
		{ { { 1, 1 }, Game::Direction::NORTH }, c_FreshCornerPieceMultiplier },
		{ { { 1, 1 }, Game::Direction::SOUTH }, c_PieceAboutToExitMultiplier },
		{ { { 1, 1 }, Game::Direction::EAST }, c_FreshCornerPieceMultiplier },
		{ { { 1, 1 }, Game::Direction::WEST }, c_PieceAboutToExitMultiplier },

		{ { { 1, 3 }, Game::Direction::NORTH }, c_PieceAboutToExitMultiplier },
		{ { { 1, 3 }, Game::Direction::SOUTH }, c_FreshCornerPieceMultiplier },
		{ { { 1, 3 }, Game::Direction::EAST }, c_FreshCornerPieceMultiplier },
		{ { { 1, 3 }, Game::Direction::WEST }, c_PieceAboutToExitMultiplier },

		{ { { 3, 1 }, Game::Direction::NORTH }, c_FreshCornerPieceMultiplier },
		{ { { 3, 1 }, Game::Direction::SOUTH }, c_PieceAboutToExitMultiplier },
		{ { { 3, 1 }, Game::Direction::EAST }, c_PieceAboutToExitMultiplier },
		{ { { 3, 1 }, Game::Direction::WEST }, c_FreshCornerPieceMultiplier },

		{ { { 3, 3 }, Game::Direction::NORTH }, c_PieceAboutToExitMultiplier },
		{ { { 3, 3 }, Game::Direction::SOUTH }, c_FreshCornerPieceMultiplier },
		{ { { 3, 3 }, Game::Direction::EAST }, c_PieceAboutToExitMultiplier },
		{ { { 3, 3 }, Game::Direction::WEST }, c_FreshCornerPieceMultiplier },

		// Lane centers
		{ { { 1, 2 }, Game::Direction::NORTH }, c_LateralCenterLanePieceMultiplier },
		{ { { 1, 2 }, Game::Direction::SOUTH }, c_LateralCenterLanePieceMultiplier },
		{ { { 1, 2 }, Game::Direction::EAST }, c_FreshCenterLanePieceMultiplier },
		{ { { 1, 2 }, Game::Direction::WEST }, c_PieceAboutToExitMultiplier },

		{ { { 2, 1 }, Game::Direction::NORTH }, c_FreshCenterLanePieceMultiplier },
		{ { { 2, 1 }, Game::Direction::SOUTH }, c_PieceAboutToExitMultiplier },
		{ { { 2, 1 }, Game::Direction::EAST }, c_LateralCenterLanePieceMultiplier },
		{ { { 2, 1 }, Game::Direction::WEST }, c_LateralCenterLanePieceMultiplier },

		{ { { 2, 3 }, Game::Direction::NORTH }, c_PieceAboutToExitMultiplier },
		{ { { 2, 3 }, Game::Direction::SOUTH }, c_FreshCenterLanePieceMultiplier },
		{ { { 2, 3 }, Game::Direction::EAST }, c_LateralCenterLanePieceMultiplier },
		{ { { 2, 3 }, Game::Direction::WEST }, c_LateralCenterLanePieceMultiplier },

		{ { { 3, 2 }, Game::Direction::NORTH }, c_LateralCenterLanePieceMultiplier },
		{ { { 3, 2 }, Game::Direction::SOUTH }, c_LateralCenterLanePieceMultiplier },
		{ { { 3, 2 }, Game::Direction::EAST }, c_PieceAboutToExitMultiplier },
		{ { { 3, 2 }, Game::Direction::WEST }, c_FreshCenterLanePieceMultiplier }
	};
}
