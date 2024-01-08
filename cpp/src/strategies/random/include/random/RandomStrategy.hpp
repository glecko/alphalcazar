#pragma once

#include <game/Strategy.hpp>
#include <game/aliases.hpp>

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
	class RandomStrategy final : public Game::Strategy {
	public:
		RandomStrategy();
		Game::PlacementMove Execute(Game::PlayerId playerId, const Utils::StaticVector<Game::PlacementMove, Game::c_MaxLegalMovesCount>& legalMoves, const Game::Game& game) override;
	private:
		std::random_device mRandomDevice;
		std::mt19937 mRandomEngine;
	};
}
