#include <game/Game.hpp>
#include <game/PlacementMove.hpp>
#include <minmax/MinMaxStrategy.hpp>

#include <util/Log.hpp>

#include <chrono>

std::uint64_t executionTime(const std::function<void()>& function) {
	const auto start = std::chrono::high_resolution_clock::now();
	function();
	const auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

void runMinMaxFirstTurnBenchmark(const Alphalcazar::Game::Game& game, Alphalcazar::Strategy::MinMax::Depth depth, bool multithreaded) {
	Alphalcazar::Strategy::MinMax::MinMaxStrategy strategy{ depth, multithreaded };
	const auto executionTimeMs = executionTime([&strategy, &game]() {
		strategy.Execute(Alphalcazar::Game::PlayerId::PLAYER_ONE, game.GetLegalMoves(Alphalcazar::Game::PlayerId::PLAYER_ONE), game);
	});
	const auto score = strategy.GetLastExecutedMoveScore();
	Alphalcazar::Utils::LogInfo("First move at depth {} took {}ms and calculated a score of {}", depth, executionTimeMs, score);
}

void runMinMaxFullGameBenchmark(Alphalcazar::Game::Game& game, Alphalcazar::Strategy::MinMax::Depth depth, bool multithreaded) {
	Alphalcazar::Strategy::MinMax::MinMaxStrategy strategy{ depth, multithreaded };
	Alphalcazar::Game::GameResult result = Alphalcazar::Game::GameResult::NONE;
	const auto executionTimeMs = executionTime([&strategy, &game, &result]() {
		result = game.Play(strategy, strategy);
	});
	Alphalcazar::Utils::LogInfo("Game at depth {} took {}ms ended with result {}", depth, executionTimeMs, static_cast<int>(result));
}

void runMinMaxBenchmarks() {
	constexpr Alphalcazar::Strategy::MinMax::Depth c_MaxDepth = 5;
	constexpr bool c_MultithreadedBenchmark = true;
	for (Alphalcazar::Strategy::MinMax::Depth depth = 1; depth <= c_MaxDepth; depth++) {
		Alphalcazar::Game::Game game{};
		runMinMaxFirstTurnBenchmark(game, depth, c_MultithreadedBenchmark);
		runMinMaxFullGameBenchmark(game, depth, c_MultithreadedBenchmark);
	}
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	runMinMaxBenchmarks();
	return 0;
}
