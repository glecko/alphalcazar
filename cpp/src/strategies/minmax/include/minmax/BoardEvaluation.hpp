#pragma once

#include "game/aliases.hpp"
#include "minmax/minmax_aliases.hpp"

namespace Alphalcazar::Game {
	class Game;
}

namespace Alphalcazar::Strategy::MinMax {
	Score GameResultToScore(Game::PlayerId playerId, Game::GameResult result);
	Score EvaluateBoard(Game::PlayerId playerId, const Game::Game& game);
	Score GetDepthAdjustedScore(Score score);
}
