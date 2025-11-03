#include "random/RandomStrategy.hpp"

#include <game/Game.hpp>
#include <game/PlacementMove.hpp>

#include <random>

namespace Alphalcazar::Strategy::Random {
	RandomStrategy::RandomStrategy() 
		: mRandomEngine { pcg_extras::seed_seq_from<std::random_device> {} }
	{}

	Game::PlacementMove RandomStrategy::Execute(Game::PlayerId, const Utils::StaticVector<Game::PlacementMove, Game::c_MaxLegalMovesCount>& legalMoves, const Game::Game&) {
		std::uniform_int_distribution<pcg32::result_type> distribution { 0, static_cast<pcg32::result_type>(legalMoves.size() - 1)};
		return legalMoves[distribution(mRandomEngine)];
	}
}
