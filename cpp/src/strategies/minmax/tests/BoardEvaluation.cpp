#include <gtest/gtest.h>

#include "minmax/BoardEvaluation.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>
#include <game/Tile.hpp>
#include <game/Piece.hpp>

#include "setuphelpers.hpp"

namespace Alphalcazar::Strategy::MinMax {
	TEST(BoardEvaluation, EvaluateBoard) {
		std::vector<PieceSetup> pieceSetups {
			{ Game::PlayerId::PLAYER_ONE, 1, Game::Direction::NORTH, { 2, 2 } },
			{ Game::PlayerId::PLAYER_ONE, 5, Game::Direction::WEST, { 1, 1 } },
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		Score score = EvaluateBoard(Game::PlayerId::PLAYER_ONE, game);
		Score opponentScore = EvaluateBoard(Game::PlayerId::PLAYER_TWO, game);

		EXPECT_EQ(score, c_PieceOnBoardScores[0] * c_CenterPieceMultiplier + c_PieceOnBoardScores[4] * c_PieceAboutToExitMultiplier);
		EXPECT_EQ(score, -opponentScore);
	}

	TEST(BoardEvaluation, EvaluatePieceLifeTime) {
		std::vector<PieceSetup> perimeterPieceSetups {
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 0 } },
		};
		Game::Game perimeterGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, perimeterPieceSetups);
		Score perimeterScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, perimeterGame);

		std::vector<PieceSetup> justEnteredPieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 1 } },
		};
		Game::Game justEnteredGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, justEnteredPieceSetups);
		Score justEnteredScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, justEnteredGame);

		std::vector<PieceSetup> centerTilePieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 2 } },
		};
		Game::Game centerTileGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, centerTilePieceSetups);
		Score centerTileScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, centerTileGame);

		std::vector<PieceSetup> aboutToExitPieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::NORTH, { 1, 3 } },
		};
		Game::Game aboutToExitGame = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, aboutToExitPieceSetups);
		Score aboutToExitScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, aboutToExitGame);

		// Pieces on the perimeter don't contribute to score
		EXPECT_EQ(perimeterScore, 0);
		// All scores are positive for the player that owns the piece
		EXPECT_TRUE(justEnteredScore > 0 && centerTileScore > 0 && aboutToExitScore > 0);
		// THe highest score should be when the piece just entered and lowest when it's about to exit
		EXPECT_TRUE(justEnteredScore > centerTileScore);
		EXPECT_TRUE(centerTileScore > aboutToExitScore);
	}
}
