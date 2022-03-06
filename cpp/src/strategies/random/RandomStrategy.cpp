#include "RandomStrategy.hpp"

#include <Game.hpp>
#include <PlacementMove.hpp>

namespace Alphalcazar::Strategy::Random {
	RandomStrategy::RandomStrategy() 
		: mRandomDevice {}
		, mRandomEngine { mRandomDevice() }
	{}

	Game::PlacementMoveIndex RandomStrategy::Execute(Game::PlayerId, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game&) {
		std::uniform_int_distribution<std::mt19937::result_type> distribution { 0, static_cast<Game::PlacementMoveIndex>(legalMoves.size() - 1) };
		return static_cast<Game::PlacementMoveIndex>(distribution(mRandomEngine));
	}
}
