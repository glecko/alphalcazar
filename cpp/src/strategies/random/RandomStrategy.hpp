#pragma once

#include <Strategy.hpp>
#include <aliases.hpp>

#include <random>

namespace Alphalcazar::Game {
	struct PlacementMove;
	class Game;
}

namespace Alphalcazar::Strategy::Random {

	/*!
	 * \brief A very basic strategy that simply plays a random moves among
	 *        the player's available legal moves.
	 */
	class RandomStrategy : public Game::Strategy {
	public:
		RandomStrategy();
		virtual Game::PlacementMoveIndex Execute(Game::PlayerId playerId, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game& game) override;
	private:
		std::random_device mRandomDevice;
		std::mt19937 mRandomEngine;
	};
}
