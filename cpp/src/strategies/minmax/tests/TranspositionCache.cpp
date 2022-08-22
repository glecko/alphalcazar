#include <gtest/gtest.h>

#include "minmax/TranspositionCache.hpp"
#include "minmax/config.hpp"

#include "setuphelpers.hpp"

namespace Alphalcazar::Strategy::MinMax {
	class TranspositionCacheBasicFixture : public ::testing::Test {
	protected:
		std::vector<PieceSetup> pieceSetups{
			{ Game::PlayerId::PLAYER_TWO, 3, Game::Direction::NORTH, { 1, 1 } },
			{ Game::PlayerId::PLAYER_TWO, 4, Game::Direction::NORTH, { 2, 1 } },

			{ Game::PlayerId::PLAYER_ONE, 3, Game::Direction::SOUTH, { 3, 3 } },
			{ Game::PlayerId::PLAYER_ONE, 4, Game::Direction::EAST, { 0, 3 } }
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);
		TranspositionCache cache;
		Score testPositionScore = 200;
		Depth storedDepth = 3;

		virtual void SetUp() {
			// We store an exact evaluation of 200 at this game's position for depth 3
			cache.StoreScore(game, EvaluationType::EXACT, storedDepth, testPositionScore);
		}

		virtual void TearDown() {
			cache.DeleteScore(game);
		}
	};

	TEST_F(TranspositionCacheBasicFixture, TestBasicExactEvaluationCache) {
		// When fetching the score at depth 3 for the same position, we expect a score of 200 for player 1
		// and a score of -200 for player 2 (player 1 is the active player)
		EXPECT_TRUE(cache.GetScore(Game::PlayerId::PLAYER_ONE, game, storedDepth, 0, 0).has_value());
		EXPECT_TRUE(cache.GetScore(Game::PlayerId::PLAYER_TWO, game, storedDepth, 0, 0).has_value());
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_ONE, game, storedDepth, 0, 0).value(), testPositionScore);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_TWO, game, storedDepth, 0, 0).value(), -testPositionScore);
	}

	TEST_F(TranspositionCacheBasicFixture, TestCacheDepth) {
		// We expect no result returned for higher depths than 3
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_ONE, game, storedDepth + 1, 0, 0), std::nullopt);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_TWO, game, storedDepth + 1, 0, 0), std::nullopt);

		// We expect the same results when searching for depths lower than 3
		EXPECT_TRUE(cache.GetScore(Game::PlayerId::PLAYER_ONE, game, storedDepth - 1, 0, 0).has_value());
		EXPECT_TRUE(cache.GetScore(Game::PlayerId::PLAYER_TWO, game, storedDepth - 1, 0, 0).has_value());
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_ONE, game, storedDepth - 1, 0, 0).value(), testPositionScore);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_TWO, game, storedDepth - 1, 0, 0).value(), -testPositionScore);
	}

	TEST_F(TranspositionCacheBasicFixture, TestDifferentTurnStateMatch) {
		// If the board has the same position, but the first move has not been executed,
		// the previously stored game should not match a cache lookup
		Game::Game firstMoveNotExecutedGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, false, pieceSetups);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_ONE, firstMoveNotExecutedGame, storedDepth, 0, 0), std::nullopt);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_TWO, firstMoveNotExecutedGame, storedDepth, 0, 0), std::nullopt);
	}

	TEST_F(TranspositionCacheBasicFixture, TestDifferentInnitiativeMatch) {
		// If the board position (and turn) is the same, but the other player is to move
		// the previously stored game should not match a cache lookup
		Game::Game opponentInitiativeGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_TWO, true, pieceSetups);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_ONE, opponentInitiativeGame, storedDepth, 0, 0), std::nullopt);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_TWO, opponentInitiativeGame, storedDepth, 0, 0), std::nullopt);
	}

	TEST_F(TranspositionCacheBasicFixture, TestDifferentBoardPositionMatch) {
		// We set up a game with a slightly different board position (adding a piece in the center)
		// The previously stored game should not match a cache lookup
		auto pieceSetupsVariant = pieceSetups;
		pieceSetupsVariant.emplace_back(Game::PlayerId::PLAYER_TWO, static_cast<Game::PieceType>(2), Game::Direction::SOUTH, Game::Coordinates{ 2, 2 });

		Game::Game changedBoardPositionGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetupsVariant);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_ONE, changedBoardPositionGame, storedDepth, 0, 0), std::nullopt);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_TWO, changedBoardPositionGame, storedDepth, 0, 0), std::nullopt);
	}

	TEST_F(TranspositionCacheBasicFixture, TestDifferentBoardPerimeterMatch) {
		// We set up a game with the same main board position, but with an extra piece in the perimeter
		// The previously stored game should not match a cache lookup
		auto pieceSetupsVariant = pieceSetups;
		pieceSetupsVariant.emplace_back(Game::PlayerId::PLAYER_ONE, static_cast<Game::PieceType>(5), Game::Direction::SOUTH, Game::Coordinates{ 4, 3 });

		Game::Game changedBoardPositionGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetupsVariant);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_ONE, changedBoardPositionGame, storedDepth, 0, 0), std::nullopt);
		EXPECT_EQ(cache.GetScore(Game::PlayerId::PLAYER_TWO, changedBoardPositionGame, storedDepth, 0, 0), std::nullopt);
	}

	TEST(TranspositionCache, TestAlphaBetaCutoffCaches) {
		std::vector<PieceSetup> pieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 1, Game::Direction::EAST, { 1, 1 } },
			{ Game::PlayerId::PLAYER_TWO, 2, Game::Direction::NORTH, { 2, 2 } },
		};

		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);
	}
}
