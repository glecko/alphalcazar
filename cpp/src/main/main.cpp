#include <game/Game.hpp>
#include <game/PlacementMove.hpp>
#include <minmax/MinMaxStrategy.hpp>

#include <util/Log.hpp>

#include <chrono>
#include <functional>

/* With c_MinDepthToStoreInTranspositionCache = 10 (no cache)
[2022-08-22 11:06:11.776] [info] First move at depth 1 took 3ms and calculated a score of 0
[2022-08-22 11:06:11.782] [info] Game at depth 1 took 2ms ended with result 3
[2022-08-22 11:06:11.843] [info] First move at depth 2 took 52ms and calculated a score of -34
[2022-08-22 11:06:12.052] [info] Game at depth 2 took 204ms ended with result 3
[2022-08-22 11:06:13.277] [info] First move at depth 3 took 1214ms and calculated a score of 35
[2022-08-22 11:06:25.277] [info] Game at depth 3 took 11994ms ended with result 2
[2022-08-22 11:09:09.034] [info] First move at depth 4 took 163748ms and calculated a score of -64
[2022-08-22 11:24:04.154] [info] Game at depth 4 took 895115ms ended with result 2
[2022-08-22 12:50:51.475] [info] First move at depth 5 took 5207322ms and calculated a score of -21
*/

/* With c_MinDepthToStoreInTranspositionCache = 3
[2022-08-22 15:47:51.869] [info] First move at depth 1 took 4ms and calculated a score of 0
[2022-08-22 15:47:51.877] [info] Game at depth 1 took 1ms ended with result 3
[2022-08-22 15:47:51.940] [info] First move at depth 2 took 56ms and calculated a score of -34
[2022-08-22 15:47:52.139] [info] Game at depth 2 took 194ms ended with result 3
[2022-08-22 15:47:53.372] [info] First move at depth 3 took 1224ms and calculated a score of 35
[2022-08-22 15:48:05.345] [info] Game at depth 3 took 11967ms ended with result 2
[2022-08-22 15:50:59.554] [info] First move at depth 4 took 174200ms and calculated a score of -64
[2022-08-22 15:59:35.800] [info] Game at depth 4 took 516240ms ended with result 3
                                 |--> How can the other player win?? There's some wonky shit here
								      Unless a depth 4 calculation was used for a depth 3 branch that made the player make a better move.
*/

std::uint64_t executionTime(std::function<void()> function) {
	auto start = std::chrono::high_resolution_clock::now();
	function();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	constexpr Alphalcazar::Strategy::MinMax::Depth c_MaxDepth = 5;
	for (Alphalcazar::Strategy::MinMax::Depth depth = 1; depth <= c_MaxDepth; depth++) {
		Alphalcazar::Strategy::MinMax::MinMaxStrategy strategy { depth, true };
		Alphalcazar::Game::Game game {};
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
