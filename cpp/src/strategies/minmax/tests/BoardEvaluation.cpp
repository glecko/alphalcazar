#include <gtest/gtest.h>

#include "minmax/BoardEvaluation.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>

#include "setuphelpers.hpp"

namespace Alphalcazar::Strategy::MinMax {
	TEST(BoardEvaluation, EvaluateBoard) {
		const std::vector<PieceSetup> pieceSetups {
			{ Game::PlayerId::PLAYER_ONE, 1, Game::Direction::NORTH, { 2, 2 } },
			{ Game::PlayerId::PLAYER_ONE, 5, Game::Direction::WEST, { 1, 1 } },
		};
		const Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		const Score score = EvaluateBoard(Game::PlayerId::PLAYER_ONE, game);
		const Score opponentScore = EvaluateBoard(Game::PlayerId::PLAYER_TWO, game);

		EXPECT_EQ(score, c_PieceOnBoardScores[0] * c_CenterPieceMultiplier + c_PieceOnBoardScores[4] * c_PieceAboutToExitMultiplier);
		EXPECT_EQ(score, -opponentScore);
	}

	TEST(BoardEvaluation, EvaluatePieceLifeTime) {
		const std::vector<PieceSetup> perimeterPieceSetups {
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 0 } },
		};
		const Game::Game perimeterGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, perimeterPieceSetups);
		const Score perimeterScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, perimeterGame);

		const std::vector<PieceSetup> justEnteredPieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 1 } },
		};
		const Game::Game justEnteredGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, justEnteredPieceSetups);
		const Score justEnteredScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, justEnteredGame);

		const std::vector<PieceSetup> centerTilePieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 2 } },
		};
		const Game::Game centerTileGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, centerTilePieceSetups);
		const Score centerTileScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, centerTileGame);

		const std::vector<PieceSetup> aboutToExitPieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 3 } },
		};
		const Game::Game aboutToExitGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, aboutToExitPieceSetups);
		const Score aboutToExitScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, aboutToExitGame);

		// Pieces on the perimeter don't contribute to score
		EXPECT_EQ(perimeterScore, 0);
		// All scores are positive for the player that owns the piece
		EXPECT_TRUE(justEnteredScore > 0 && centerTileScore > 0 && aboutToExitScore > 0);
		// THe highest score should be when the piece just entered and lowest when it's about to exit
		EXPECT_TRUE(justEnteredScore > centerTileScore);
		EXPECT_TRUE(centerTileScore > aboutToExitScore);
	}
}
