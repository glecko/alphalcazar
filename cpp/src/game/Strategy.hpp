#pragma once

#include "aliases.hpp"

namespace Alphalcazar::Game {
	class Game;
	class PlacementMove;

	/*!
	 * \brief Virtual class that represents how players will decide which move to play on their turn.
	 * Implementations of a Strategy should define a Execute() method that determines the best move for a given game state.
	 * Examples of valid strategies can be playing a random move, asking a human for input or computing the best move to play with heuristics.
	 */
	class Strategy {
	public:
		Strategy() = default;
		virtual ~Strategy() = default;

		/// Returns the desired PlacementMove to execute given the state of the game and the player who executes the strategy
		virtual PlacementMove& Execute([[maybe_unused]] PlayerId& playerId, [[maybe_unused]] const Game& game) const = 0;
	};
}
