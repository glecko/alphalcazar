#include "random/RandomStrategy.hpp"

#include <game/Game.hpp>
#include <game/PlacementMove.hpp>

namespace Alphalcazar::Strategy::Random {
	RandomStrategy::RandomStrategy() 
		: mRandomDevice {}
		, mRandomEngine { mRandomDevice() }
	{}

	Game::PlacementMove RandomStrategy::Execute(Game::PlayerId, const Utils::StaticVector<Game::PlacementMove, Game::c_MaxLegalMovesCount>& legalMoves, const Game::Game&) {
		std::uniform_int_distribution<std::mt19937::result_type> distribution { 0, static_cast<std::mt19937::result_type>(legalMoves.size() - 1)};
		return legalMoves[distribution(mRandomEngine)];
	}
}
