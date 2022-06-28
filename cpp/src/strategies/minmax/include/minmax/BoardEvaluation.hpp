#pragma once

#include "game/aliases.hpp"
#include "minmax/minmax_aliases.hpp"

namespace Alphalcazar::Game {
	class Game;
}

namespace Alphalcazar::Strategy::MinMax {
	/// Converts a game result to a score.
	Score GameResultToScore(Game::PlayerId playerId, Game::GameResult result);

	/*!
	 * \brief Evaluates the board of a given game and returns a heuristic score for the specified player.
	 * 
	 * \param playerId The ID of the player for which the score is being calculated.
	 * \param game The game to evaluate.
	 * 
	 * \note The function has player symmetry, meaning that if the score of a game for a player is X, the score
	 *       of the same game for the opponent will be -X.
	 */
	Score EvaluateBoard(Game::PlayerId playerId, const Game::Game& game);

	/*!
	 * \brief Adjusts a given score for a given depth level.
	 * 
	 * The absolute value of the score will be reduced (moved closer to 0) in order to
	 * ensure that if several moves lead to the same position/result, the heuristic advantage/disadvantage
	 * is reduced for longer paths and maximized for immediate paths.
	 *
	 * \param score The original score before adjusting for depth.
	 * \param depth The depth level for which to adjust the score
	 */
	Score GetDepthAdjustedScore(Score score, Depth depth);
}
