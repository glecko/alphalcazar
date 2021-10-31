#include <gtest/gtest.h>

#include "aliases.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include "Piece.hpp"
#include "parameters.hpp"

namespace Alphalcazar::Game {
	TEST(Board, SetupTiles) {
		Board board {};
		EXPECT_EQ(board.IsFull(), false);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);
	}

	TEST(Board, GetCompletRowResult) {
		Board board {};
		Piece pieceOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwo { PlayerId::PLAYER_ONE, 2 };
		Piece pieceThree { PlayerId::PLAYER_ONE, 3 };

		// We complete the south-most row of the board with pieces of 
		// player 1, so that player should win
		board.GetTile(1, 1)->PlacePiece(&pieceOne);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(2, 1)->PlacePiece(&pieceTwo);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(3, 1)->PlacePiece(&pieceThree);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_ONE_WINS);
	}

	TEST(Board, GetMultiplePlayersRowResult) {
		Board board {};
		Piece pieceOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwo { PlayerId::PLAYER_TWO, 2 };
		Piece pieceThree { PlayerId::PLAYER_ONE, 3 };
		Piece pieceFour { PlayerId::PLAYER_TWO, 4 };
		Piece pieceFive { PlayerId::PLAYER_TWO, 5 };

		// In this test we complete a row and a diagonal in a way
		// that both of them have pieces of both players, so no player should win
		board.GetTile(1, 1)->PlacePiece(&pieceOne);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(1, 2)->PlacePiece(&pieceTwo);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(1, 3)->PlacePiece(&pieceThree);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(2, 2)->PlacePiece(&pieceFour);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(3, 3)->PlacePiece(&pieceFive);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);
	}

	TEST(Board, GetCompletDiagonalResult) {
		Board board {};
		Piece pieceOne { PlayerId::PLAYER_TWO, 1 };
		Piece pieceTwo { PlayerId::PLAYER_TWO, 2 };
		Piece pieceThree { PlayerId::PLAYER_TWO, 3 };
		Piece pieceFour { PlayerId::PLAYER_ONE, 4 };
		Piece pieceFive { PlayerId::PLAYER_ONE, 5 };

		// In this test we complete a diagonal with pieces of player 2
		// so they should win. We afterwards place some pieces of player 1
		// at random spots on the board and check that this doesn't alter the result
		board.GetTile(1, 1)->PlacePiece(&pieceOne);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(2, 2)->PlacePiece(&pieceTwo);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(3, 3)->PlacePiece(&pieceThree);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_TWO_WINS);

		board.GetTile(2, 1)->PlacePiece(&pieceFour);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_TWO_WINS);

		board.GetTile(3, 2)->PlacePiece(&pieceFive);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_TWO_WINS);
	}

	TEST(Board, MultipleRowCompletingPlayers) {
		Board board {};
		Piece pieceOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwo { PlayerId::PLAYER_ONE, 2 };
		Piece pieceThree { PlayerId::PLAYER_ONE, 3 };
		Piece pieceFour { PlayerId::PLAYER_TWO, 4 };
		Piece pieceFive { PlayerId::PLAYER_TWO, 5 };
		Piece pieceSix { PlayerId::PLAYER_TWO, 5 };

		// First we set up a board where player 1 has completed the center
		// column and wins. Next we set up pieces so that player 2 also completes a column
		// and the game should result in a draw or in no result depending on if draws are accepted
		board.GetTile(2, 1)->PlacePiece(&pieceOne);
		board.GetTile(2, 2)->PlacePiece(&pieceTwo);
		board.GetTile(2, 3)->PlacePiece(&pieceThree);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_ONE_WINS);

		board.GetTile(3, 1)->PlacePiece(&pieceFour);
		board.GetTile(3, 2)->PlacePiece(&pieceFive);
		board.GetTile(3, 3)->PlacePiece(&pieceSix);
		EXPECT_EQ(board.GetResult(), c_AcceptDraws ? GameResult::DRAW : GameResult::NONE);
	}

	TEST(Board, PiecePushedIntoPerimeter) {
		Board board{};
		Piece pieceOnePlayerOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwoPlayerOne { PlayerId::PLAYER_ONE, 2 };
		Piece pieceFivePlayerTwo { PlayerId::PLAYER_TWO, 5 };

		// Piece 1 on (1,1) will move north to (1,2) (+1 movement)
		// Piece 2 on (3,1) will move south to (3,0). Since it exited the board it is removed from play (+1 movement)
		// Piece 5 on (2,2) will moves east to (1,2) pushing piece 1 outside the board (+2 movements)
		board.GetTile(1, 1)->PlacePiece(&pieceOnePlayerOne);
		pieceOnePlayerOne.SetMovementDirection(Direction::NORTH);
		board.GetTile(3, 1)->PlacePiece(&pieceTwoPlayerOne);
		pieceTwoPlayerOne.SetMovementDirection(Direction::SOUTH);
		board.GetTile(2, 2)->PlacePiece(&pieceFivePlayerTwo);
		pieceFivePlayerTwo.SetMovementDirection(Direction::WEST);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_ONE);
		EXPECT_EQ(executedMoves, 4);

		EXPECT_EQ(*board.GetTile(1, 2)->GetPiece(), pieceFivePlayerTwo);
		EXPECT_EQ(board.GetTile(3, 0)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(0, 2)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(2, 2)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(3, 1)->GetPiece(), nullptr);

		EXPECT_EQ(pieceOnePlayerOne.IsInPlay(), false);
		EXPECT_EQ(pieceTwoPlayerOne.IsInPlay(), false);
		EXPECT_EQ(pieceFivePlayerTwo.IsInPlay(), true);
	}

	TEST(Board, PushablePieceMovements) {
		Board board{};
		Piece pieceOnePlayerOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwoPlayerOne { PlayerId::PLAYER_ONE, 2 };

		Piece pieceOnePlayerTwo { PlayerId::PLAYER_TWO, 1 };
		Piece pieceTwoPlayerTwo { PlayerId::PLAYER_TWO, 2 };
		Piece pieceThreePlayerTwo { PlayerId::PLAYER_TWO, 3 };
		Piece pieceFivePlayerTwo { PlayerId::PLAYER_TWO, 5 };
		

		// The pieces of player 2 move first.
		// Piece 1 of player 2 on (3,2) will move freely to (2,2) (+1 movement)
		// Piece 1 of player 1 on (2,1) will be blocked by piece 3 of player 2 on (3,1)
		// Piece 2 of player 2 on (1,2) will push piece 1 of player 2 back to (3,2) (+2 movements)
		// Piece 2 of player 1 on (2,0) perimeter will be unable to push piece 1 of player 1 on (2,1)
		// because piece 2 of player 2 on (2,2) is behind it. Since it's stuck on the perimeter, it is removed.
		// Piece 3 of player 2 on (3,1) will move north to (3,2) pushing piece 1 of player 2 to (3,3) (+2 movements)
		// Piece 5 of player 2 on (1,4) will freely enter the board to (1,3) (+1 movement)
		board.GetTile(2, 1)->PlacePiece(&pieceOnePlayerOne);
		pieceOnePlayerOne.SetMovementDirection(Direction::EAST);
		board.GetTile(2, 0)->PlacePiece(&pieceTwoPlayerOne);
		pieceTwoPlayerOne.SetMovementDirection(Direction::NORTH);
		board.GetTile(3, 1)->PlacePiece(&pieceThreePlayerTwo);
		pieceThreePlayerTwo.SetMovementDirection(Direction::NORTH);
		board.GetTile(1, 2)->PlacePiece(&pieceTwoPlayerTwo);
		pieceTwoPlayerTwo.SetMovementDirection(Direction::EAST);
		board.GetTile(3, 2)->PlacePiece(&pieceOnePlayerTwo);
		pieceOnePlayerTwo.SetMovementDirection(Direction::WEST);
		board.GetTile(1, 4)->PlacePiece(&pieceFivePlayerTwo);
		pieceFivePlayerTwo.SetMovementDirection(Direction::SOUTH);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
		EXPECT_EQ(executedMoves, 6);

		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), pieceTwoPlayerTwo);
		EXPECT_EQ(board.GetTile(2, 0)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(1, 1)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(1, 2)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(1, 4)->GetPiece(), nullptr);
		EXPECT_EQ(*board.GetTile(3, 2)->GetPiece(), pieceThreePlayerTwo);
		EXPECT_EQ(*board.GetTile(3, 3)->GetPiece(), pieceOnePlayerTwo);
		EXPECT_EQ(*board.GetTile(1, 3)->GetPiece(), pieceFivePlayerTwo);
		EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), pieceOnePlayerOne);

		EXPECT_EQ(pieceOnePlayerOne.IsInPlay(), true);
		EXPECT_EQ(pieceOnePlayerTwo.IsInPlay(), true);
		EXPECT_EQ(pieceTwoPlayerTwo.IsInPlay(), true);
		EXPECT_EQ(pieceThreePlayerTwo.IsInPlay(), true);
		EXPECT_EQ(pieceFivePlayerTwo.IsInPlay(), true);
		EXPECT_EQ(pieceTwoPlayerOne.IsInPlay(), false);
	}
}
