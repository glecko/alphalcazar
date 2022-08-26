#include <game/Game.hpp>
#include <game/PlacementMove.hpp>
#include <minmax/MinMaxStrategy.hpp>

#include <util/log.hpp>

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
[2022-08-26 11:22:20.166] [info] First move at depth 1 took 3ms and calculated a score of 0
[2022-08-26 11:22:20.174] [info] Game at depth 1 took 2ms ended with result 3
[2022-08-26 11:22:20.238] [info] First move at depth 2 took 56ms and calculated a score of -34
[2022-08-26 11:22:20.437] [info] Game at depth 2 took 193ms ended with result 3
[2022-08-26 11:22:21.813] [info] First move at depth 3 took 1367ms and calculated a score of 35
[2022-08-26 11:22:35.480] [info] Game at depth 3 took 13659ms ended with result 2
[2022-08-26 11:24:36.753] [info] First move at depth 4 took 121263ms and calculated a score of -64
[2022-08-26 11:50:35.163] [info] Game at depth 4 took 1558404ms ended with result 3
*/

/* With c_MinDepthToStoreInTranspositionCache = 4
[2022-08-26 14:00:46.837] [info] First move at depth 1 took 5ms and calculated a score of 0
[2022-08-26 14:00:46.844] [info] Game at depth 1 took 2ms ended with result 3
[2022-08-26 14:00:46.909] [info] First move at depth 2 took 56ms and calculated a score of -34
[2022-08-26 14:00:47.112] [info] Game at depth 2 took 198ms ended with result 3
[2022-08-26 14:00:48.382] [info] First move at depth 3 took 1261ms and calculated a score of 35
[2022-08-26 14:01:00.694] [info] Game at depth 3 took 12306ms ended with result 2
[2022-08-26 14:03:47.215] [info] First move at depth 4 took 166511ms and calculated a score of -64
[2022-08-26 14:19:31.539] [info] Game at depth 4 took 944318ms ended with result 2
*/

std::uint64_t executionTime(std::function<void()> function) {
	auto start = std::chrono::high_resolution_clock::now();
	function();
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	constexpr Alphalcazar::Strategy::MinMax::Depth c_MaxDepth = 2;
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
