#include <game/Game.hpp>
#include <game/PlacementMove.hpp>
#include <minmax/MinMaxStrategy.hpp>

#include <util/Log.hpp>

#include <chrono>

std::uint64_t executionTime(std::function<void()> function) {
	auto start = std::chrono::high_resolution_clock::now();
	function();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	auto start = std::chrono::high_resolution_clock::now();

	constexpr Alphalcazar::Strategy::MinMax::Depth c_MaxDepth = 5;
	for (Alphalcazar::Strategy::MinMax::Depth depth = 1; depth <= c_MaxDepth; depth++) {
		Alphalcazar::Strategy::MinMax::MinMaxStrategy strategy{ depth, true };
		Alphalcazar::Game::Game game{};
		auto firstTurnExecutionTimeMs = executionTime([&strategy, &game]() {
			strategy.Execute(Alphalcazar::Game::PlayerId::PLAYER_ONE, game.GetLegalMoves(Alphalcazar::Game::PlayerId::PLAYER_ONE), game);
		});
		auto score = strategy.GetLastExecutedMoveScore();
		Alphalcazar::Utils::LogInfo("First move at depth {} took {}ms and calculated a score of {}", depth, firstTurnExecutionTimeMs, score);

		Alphalcazar::Strategy::MinMax::MinMaxStrategy fullGameStrategy{ depth, true };
		Alphalcazar::Game::GameResult result = Alphalcazar::Game::GameResult::NONE;
		auto fullGameExecutionTime = executionTime([&fullGameStrategy, &game, &result]() {
			result = game.Play(fullGameStrategy, fullGameStrategy);
		});
		Alphalcazar::Utils::LogInfo("Game at depth {} took {}ms ended with result {}", depth, fullGameExecutionTime, static_cast<int>(result));
	}

	return 0;
}
