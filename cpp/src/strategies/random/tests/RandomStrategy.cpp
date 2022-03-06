#include <gtest/gtest.h>

#include "Game.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include "parameters.hpp"
#include "RandomStrategy.hpp"
#include "PlacementMove.hpp"

namespace Alphalcazar::Strategy::Random {
	TEST(RandomStrategy, TestSingleTurn) {
		RandomStrategy strategy {};
		// We run the test 100 times to make sure the results are consistent across several random moves
		for (std::size_t i = 0; i <= 100; i++) {
			Game::Game game {};
			const auto& board = game.GetBoard();
			const auto perimeterTilesCount = game.GetBoard().GetPerimeterTiles().size();
			auto initialLegalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_ONE).size();

			auto result = game.PlayTurn(strategy, strategy);
			EXPECT_EQ(result, Game::GameResult::NONE);
			EXPECT_EQ(board.IsFull(), false);
			// The amount of legal moves for at least one player will have decreased by the amount of perimeter tiles there are
			// as they will have 1 piece less in hand
			auto playerOneMovesCount = game.GetLegalMoves(Game::PlayerId::PLAYER_ONE).size();
			auto playerTwoMovesCount = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO).size();
			auto expectedLegalMoves = initialLegalMoves - perimeterTilesCount;
			EXPECT_TRUE(playerOneMovesCount == expectedLegalMoves || playerTwoMovesCount == expectedLegalMoves);

			// After a single turn has been played, we expect either 2 or 1 pieces on the board 
			// (1 if the entering piece of the first player blocked the entrance of the piece of the second player)
			auto pieces = 0;
			for (auto& tile : board.GetTiles()) {
				if (tile->GetPiece()) {
					pieces += 1;
				}
			}
			EXPECT_TRUE(pieces == 2 || pieces == 1);
		}
	}

	TEST(RandomStrategy, TestRandomGames) {
		RandomStrategy strategy {};
		// We run a 100 random games as a santity check that the game works correctly
		for (std::size_t i = 0; i <= 100; i++) {
			Game::Game game {};
			auto result = game.Play(strategy, strategy);
			const auto& state = game.GetState();
			// Validate that the strategy ends up with a valid game result
			EXPECT_TRUE(result == Game::GameResult::DRAW || result == Game::GameResult::PLAYER_ONE_WINS || result == Game::GameResult::PLAYER_TWO_WINS);
			// Any game that ends needs to have been at least as many turns long as the board size
			// (so as the winning player has had the chance to place enough pieces to complete a row)
			EXPECT_TRUE(state.Turn >= Game::c_BoardSize);
		}
	}
}
