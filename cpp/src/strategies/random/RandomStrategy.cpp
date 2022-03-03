#include "RandomStrategy.hpp"

#include "Game.hpp"
#include "PlacementMove.hpp"

namespace Alphalcazar::Strategy::Random {
	RandomStrategy::RandomStrategy() 
		: mRandomDevice {}
		, mRandomEngine { mRandomDevice() }
	{}

	std::size_t RandomStrategy::Execute(Game::PlayerId&, const std::vector<Game::PlacementMove>& legalMoves, const Game::Game&) {
		std::uniform_int_distribution<std::mt19937::result_type> distribution { 0, static_cast<std::uint16_t>(legalMoves.size() - 1) };
		return distribution(mRandomEngine);
	}
}
