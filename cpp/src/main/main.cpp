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
[2022-08-23 12:58:19.136] [info] First move at depth 1 took 3ms and calculated a score of 0
[2022-08-23 12:58:19.143] [info] Game at depth 1 took 1ms ended with result 3
[2022-08-23 12:58:19.213] [info] First move at depth 2 took 62ms and calculated a score of -34
[2022-08-23 12:58:19.408] [info] Game at depth 2 took 191ms ended with result 3
[2022-08-23 12:58:20.635] [info] First move at depth 3 took 1217ms and calculated a score of 35
[2022-08-23 12:58:33.355] [info] Game at depth 3 took 12714ms ended with result 2
[2022-08-23 13:00:34.915] [info] First move at depth 4 took 121551ms and calculated a score of -64
[2022-08-23 13:23:13.233] [info] Game at depth 4 took 1358312ms ended with result 3 --> PC went to sleep, check again?
*/

/* With c_MinDepthToStoreInTranspositionCache = 4

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
