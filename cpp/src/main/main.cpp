#include <game/Game.hpp>
#include <game/PlacementMove.hpp>
#include <minmax/MinMaxStrategy.hpp>

#include <util/Log.hpp>

#include <chrono>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	auto start = std::chrono::high_resolution_clock::now();

	constexpr Alphalcazar::Strategy::MinMax::Depth c_MaxDepth = 5;
	for (Alphalcazar::Strategy::MinMax::Depth depth = 1; depth <= c_MaxDepth; depth++) {
		Alphalcazar::Strategy::MinMax::MinMaxStrategy strategy { depth };
		Alphalcazar::Game::Game game {};
		auto gameStart = std::chrono::high_resolution_clock::now();
		// auto result = game.Play(strategy, strategy);
		strategy.Execute(Alphalcazar::Game::PlayerId::PLAYER_ONE, game.GetLegalMoves(Alphalcazar::Game::PlayerId::PLAYER_ONE), game);
		auto score = strategy.GetLastExecutedMoveScore();
		auto gameEnd = std::chrono::high_resolution_clock::now();
		auto gameExecutionTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(gameEnd - gameStart);
		Alphalcazar::Utils::LogInfo("First move at depth {} took {}ms and calculated a score of {}", depth, gameExecutionTimeMs.count(), score);
	}

	auto executionTime = std::chrono::high_resolution_clock::now() - start;
	auto executionTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(executionTime);

	Alphalcazar::Utils::LogInfo("Function execution took {}ms", executionTimeMs.count());

	return 0;
}
