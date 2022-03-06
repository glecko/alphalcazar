#include <gtest/gtest.h>

#include "BoardEvaluation.hpp"
#include "config.hpp"

#include <Game.hpp>
#include <Board.hpp>
#include <PlacementMove.hpp>
#include <Tile.hpp>
#include <Player.hpp>
#include <Piece.hpp>

namespace Alphalcazar::Strategy::MinMax {
	TEST(BoardEvaluation, EvaluateBoard) {
		Game::Game game {};

		Game::Piece* pieceOnePlayerOne = game.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(1);
		pieceOnePlayerOne->SetMovementDirection(Game::Direction::NORTH);

		Game::Piece* pieceFivePlayerOne = game.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(5);
		pieceFivePlayerOne->SetMovementDirection(Game::Direction::WEST);

		game.GetBoard().GetTile(2, 2)->PlacePiece(pieceOnePlayerOne);
		game.GetBoard().GetTile(1, 1)->PlacePiece(pieceFivePlayerOne);

		Score score = EvaluateBoard(Game::PlayerId::PLAYER_ONE, game);
		Score opponentScore = EvaluateBoard(Game::PlayerId::PLAYER_TWO, game);

		EXPECT_EQ(score, c_PieceOnBoardScores[0] * c_CenterPieceMultiplier + c_PieceOnBoardScores[4] * c_PieceAboutToExitMultiplier);
		EXPECT_EQ(score, -opponentScore);
	}

	TEST(BoardEvaluation, EvaluatePieceLifeTime) {
		Game::Game perimeterGame {};
		Game::Piece* pieceTwoPerimeter = perimeterGame.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(2);
		pieceTwoPerimeter->SetMovementDirection(Game::Direction::NORTH);
		perimeterGame.GetBoard().GetTile(1, 0)->PlacePiece(pieceTwoPerimeter);
		Score perimeterScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, perimeterGame);

		Game::Game justEnteredGame {};
		Game::Piece* pieceTwoJustEntered = justEnteredGame.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(2);
		pieceTwoJustEntered->SetMovementDirection(Game::Direction::NORTH);
		justEnteredGame.GetBoard().GetTile(1, 1)->PlacePiece(pieceTwoJustEntered);
		Score justEnteredScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, justEnteredGame);

		Game::Game centerTileGame {};
		Game::Piece* pieceTwoCenterTile = centerTileGame.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(2);
		pieceTwoCenterTile->SetMovementDirection(Game::Direction::NORTH);
		centerTileGame.GetBoard().GetTile(1, 2)->PlacePiece(pieceTwoCenterTile);
		Score centerTileScore = EvaluateBoard(Game::PlayerId::PLAYER_ONE, centerTileGame);

		Game::Game aboutToExitGame {};
		Game::Piece* pieceTwoAboutToExit = aboutToExitGame.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(2);
		pieceTwoAboutToExit->SetMovementDirection(Game::Direction::NORTH);
		aboutToExitGame.GetBoard().GetTile(1, 3)->PlacePiece(pieceTwoAboutToExit);
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
