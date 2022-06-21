#include <gtest/gtest.h>

#include "minmax/BoardEvaluation.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>
#include <game/Board.hpp>
#include <game/PlacementMove.hpp>
#include <game/Tile.hpp>
#include <game/Piece.hpp>

namespace Alphalcazar::Strategy::MinMax {
	TEST(BoardEvaluation, EvaluateBoard) {
		Game::Game game {};

		Game::Piece pieceOnePlayerOne { Game::PlayerId::PLAYER_ONE, 1 };
		Game::Piece pieceFivePlayerOne { Game::PlayerId::PLAYER_ONE, 5 };

		game.GetBoard().GetTile(2, 2)->PlacePiece(pieceOnePlayerOne);
		game.GetBoard().GetTile(2, 2)->GetPiece()->SetMovementDirection(Game::Direction::NORTH);
		game.GetBoard().GetTile(1, 1)->PlacePiece(pieceFivePlayerOne);
		game.GetBoard().GetTile(1, 1)->GetPiece()->SetMovementDirection(Game::Direction::WEST);

		Score score = EvaluateBoard(Game::PlayerId::PLAYER_ONE, game);
		Score opponentScore = EvaluateBoard(Game::PlayerId::PLAYER_TWO, game);

		EXPECT_EQ(score, c_PieceOnBoardScores[0] * c_CenterPieceMultiplier + c_PieceOnBoardScores[4] * c_PieceAboutToExitMultiplier);
		EXPECT_EQ(score, -opponentScore);
	}

	TEST(BoardEvaluation, EvaluatePieceLifeTime) {
		Game::Game perimeterGame {};
		Game::Piece pieceTwoPerimeter { Game::PlayerId::PLAYER_ONE, 2 };
		perimeterGame.GetBoard().GetTile(1, 0)->PlacePiece(pieceTwoPerimeter);
		perimeterGame.GetBoard().GetTile(1, 0)->GetPiece()->SetMovementDirection(Game::Direction::NORTH);
		Score perimeterScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, perimeterGame);

		Game::Game justEnteredGame {};
		Game::Piece pieceTwoJustEntered { Game::PlayerId::PLAYER_ONE, 2 };
		justEnteredGame.GetBoard().GetTile(1, 1)->PlacePiece(pieceTwoJustEntered);
		justEnteredGame.GetBoard().GetTile(1, 1)->GetPiece()->SetMovementDirection(Game::Direction::NORTH);
		Score justEnteredScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, justEnteredGame);

		Game::Game centerTileGame {};
		Game::Piece pieceTwoCenterTile { Game::PlayerId::PLAYER_ONE, 2 };
		centerTileGame.GetBoard().GetTile(1, 2)->PlacePiece(pieceTwoCenterTile);
		centerTileGame.GetBoard().GetTile(1, 2)->GetPiece()->SetMovementDirection(Game::Direction::NORTH);
		Score centerTileScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, centerTileGame);

		Game::Game aboutToExitGame {};
		Game::Piece pieceTwoAboutToExit { Game::PlayerId::PLAYER_ONE, 2 };
		aboutToExitGame.GetBoard().GetTile(1, 3)->PlacePiece(pieceTwoAboutToExit);
		aboutToExitGame.GetBoard().GetTile(1, 3)->GetPiece()->SetMovementDirection(Game::Direction::NORTH);
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
