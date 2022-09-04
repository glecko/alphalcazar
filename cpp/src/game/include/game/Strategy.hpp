#pragma once

#include "aliases.hpp"
#include "parameters.hpp"
#include <util/StaticVector.hpp>

namespace Alphalcazar::Game {
	class Game;
	struct PlacementMove;

	/*!
	 * \brief Virtual class that represents how players will decide which move to play on their turn.
	 * Implementations of a Strategy should define a Execute() method that determines the best move for a given game state.
	 * Examples of valid strategies can be playing a random move, asking a human for input or computing the best move to play with heuristics.
	 */
	class Strategy {
	public:
		Strategy() = default;
		virtual ~Strategy() = default;

		/*!
		 * \brief Returns the placement move to execute given the state of the game 
		          and the player who executes the strategy.
	     *
		 * \param playerId The ID of the player who's turn it is to play.
		 * \param legalMoves The list of available legal moves the player currently has available.
		 * \param game The game object. Can be used to factor in the state of the game and the board in order to chose a move.
		 * 
		 * \returns The move to be executed.
		 */
		virtual PlacementMove Execute(PlayerId playerId, const Utils::StaticVector<PlacementMove, c_MaxLegalMovesCount>& legalMoves, const Game& game) = 0;
	};
}
