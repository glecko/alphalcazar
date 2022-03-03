#include <gtest/gtest.h>

#include "aliases.hpp"
#include "Board.hpp"
#include "Tile.hpp"
#include "Piece.hpp"
#include "parameters.hpp"

#include <array>

namespace Alphalcazar::Game {

	static Piece c_AllPieces[] = {
		{ PlayerId::PLAYER_ONE, 1 },
		{ PlayerId::PLAYER_ONE, 2 },
		{ PlayerId::PLAYER_ONE, 3 },
		{ PlayerId::PLAYER_ONE, 4 },
		{ PlayerId::PLAYER_ONE, 5 },
		{ PlayerId::PLAYER_TWO, 1 },
		{ PlayerId::PLAYER_TWO, 2 },
		{ PlayerId::PLAYER_TWO, 3 },
		{ PlayerId::PLAYER_TWO, 4 },
		{ PlayerId::PLAYER_TWO, 5 },
	};

	TEST(Board, SetupTiles) {
		Board board {};
		EXPECT_EQ(board.IsFull(), false);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);
		// We expect a square of c_PlayAreaSize size without the 4 corners
		auto tiles = board.GetTiles();
		EXPECT_EQ(tiles.size(), c_PlayAreaSize * c_PlayAreaSize - 4);

		// Check that all perimeter tiles can be found with GetPerimeterTile(), and only
		// perimeter tiles can be found with that function.
		for (auto* tile : tiles) {
			if (tile->GetCoordinates().IsPerimeter()) {
				EXPECT_NE(board.GetPerimeterTile(tile->GetCoordinates()), nullptr);
			} else {
				EXPECT_EQ(board.GetPerimeterTile(tile->GetCoordinates()), nullptr);
			}
		}		
	}

	TEST(Board, PlacePiece) {
		Board board {};

		std::size_t pieceIndex = 0;
		for (auto* tile : board.GetTiles()) {
			auto& coordinates = tile->GetCoordinates();
			if (coordinates.IsPerimeter()) {
				Piece* piece = &c_AllPieces[pieceIndex];
				pieceIndex++;
				board.PlacePiece(coordinates, piece);
				EXPECT_EQ(board.GetTile(coordinates)->GetPiece(), piece);
			}
		}
		// Board should still be empty as we have only placed on non-perimeter tiles
		EXPECT_EQ(board.IsFull(), false);
	}

	TEST(Board, BoardIsFull) {
		Board board {};

		std::size_t pieceIndex = 0;
		for (auto* tile : board.GetTiles()) {
			auto& coordinates = tile->GetCoordinates();
			if (!coordinates.IsPerimeter()) {
				Piece* piece = &c_AllPieces[pieceIndex];
				pieceIndex++;
				board.GetTile(coordinates)->PlacePiece(piece);
			}
		}
		EXPECT_EQ(board.IsFull(), true);
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
		Piece pieceOnePlayerOne { PlayerId::PLAYER_ONE, c_PushablePieceType };
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

	TEST(Board, PushablePiecesDontPushEachOther) {
		Board board {};
		Piece pieceOnePlayerOne { PlayerId::PLAYER_ONE, c_PushablePieceType };
		Piece pieceOnePlayerTwo { PlayerId::PLAYER_TWO, c_PushablePieceType };

		// Two pushable pieces are facing each other. As pushable pieces cannot
		// push another pushable piece, we expect no movements to happen in this setup
		board.GetTile(2, 2)->PlacePiece(&pieceOnePlayerOne);
		pieceOnePlayerOne.SetMovementDirection(Direction::NORTH);
		board.GetTile(2, 3)->PlacePiece(&pieceOnePlayerTwo);
		pieceOnePlayerTwo.SetMovementDirection(Direction::SOUTH);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_ONE);
		EXPECT_EQ(executedMoves, 0);

		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), pieceOnePlayerOne);
		EXPECT_EQ(*board.GetTile(2, 3)->GetPiece(), pieceOnePlayerTwo);

		EXPECT_EQ(pieceOnePlayerOne.IsInPlay(), true);
		EXPECT_EQ(pieceOnePlayerTwo.IsInPlay(), true);
	}

	TEST(Board, PushablePieceMovements) {
		Board board{};
		Piece pieceOnePlayerOne { PlayerId::PLAYER_ONE, c_PushablePieceType };
		Piece pieceTwoPlayerOne { PlayerId::PLAYER_ONE, 2 };

		Piece pieceOnePlayerTwo { PlayerId::PLAYER_TWO, c_PushablePieceType };
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

	TEST(Board, PushingPieceMovements) {
		Board board {};
		Piece pieceTwo { PlayerId::PLAYER_ONE, 2 };
		Piece pieceThree { PlayerId::PLAYER_ONE, 3 };
		Piece pushingPiece { PlayerId::PLAYER_ONE, c_PusherPieceType };

		// Piece 2 (3,1) moves freely to (3,2) (+1 movement)
		// Piece 3 (1,2) moves freely to (2,2) (+1 movement)
		// The pushing piece (1,2) moves to (2,2), pushing piece 3 to (3,2)
		// and piece 2 outside the board (+3 movements)
		board.GetTile(3, 1)->PlacePiece(&pieceTwo);
		pieceTwo.SetMovementDirection(Direction::NORTH);
		board.GetTile(2, 3)->PlacePiece(&pieceThree);
		pieceThree.SetMovementDirection(Direction::SOUTH);
		board.GetTile(1, 2)->PlacePiece(&pushingPiece);
		pushingPiece.SetMovementDirection(Direction::EAST);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
		EXPECT_EQ(executedMoves, 5);

		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), pushingPiece);
		EXPECT_EQ(*board.GetTile(3, 2)->GetPiece(), pieceThree);
		EXPECT_EQ(board.GetTile(4, 2)->GetPiece(), nullptr);

		EXPECT_EQ(pieceTwo.IsInPlay(), false);
		EXPECT_EQ(pieceThree.IsInPlay(), true);
		EXPECT_EQ(pushingPiece.IsInPlay(), true);
	}

	TEST(Board, PushingPiecesPushingEachOther) {
		Board board{};
		Piece pushingPieceOne{ PlayerId::PLAYER_ONE, c_PusherPieceType };
		Piece pushingPieceTwo{ PlayerId::PLAYER_TWO, c_PusherPieceType };

		// The two pushing pieces are facing each other
		board.GetTile(2, 2)->PlacePiece(&pushingPieceOne);
		pushingPieceOne.SetMovementDirection(Direction::SOUTH);
		board.GetTile(2, 1)->PlacePiece(&pushingPieceTwo);
		pushingPieceTwo.SetMovementDirection(Direction::NORTH);

		{
			// If player 2 is the starting player, we expect both pieces to push each other
			// and end up in exactly the same positions, resulting in 4 movements (2 for each push)
			auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
			EXPECT_EQ(executedMoves, 4);

			EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), pushingPieceOne);
			EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), pushingPieceTwo);

			EXPECT_EQ(pushingPieceOne.IsInPlay(), true);
			EXPECT_EQ(pushingPieceTwo.IsInPlay(), true);
		}

		{
			// However, on the next turn, when player 1 moves first, their pushing piece on (2,2)
			// will push the pusher of player 2 off the board, after which the pusher of player 2
			// will not execute any movement because it will have been removed from play
			auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_ONE);
			EXPECT_EQ(executedMoves, 2);

			EXPECT_EQ(board.GetTile(2, 2)->GetPiece(), nullptr);
			EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), pushingPieceOne);

			EXPECT_EQ(pushingPieceOne.IsInPlay(), true);
			EXPECT_EQ(pushingPieceTwo.IsInPlay(), false);
		}
	}

	TEST(Board, ComplexBoardMovements) {
		Board board{};
		Piece pieceOnePlayerOne { PlayerId::PLAYER_ONE, c_PushablePieceType };
		Piece pieceTwoPlayerOne { PlayerId::PLAYER_ONE, 2 };
		Piece pieceFourPlayerOne { PlayerId::PLAYER_ONE, c_PusherPieceType };
		Piece pieceFivePlayerOne { PlayerId::PLAYER_ONE, 5 };

		Piece pieceThreePlayerTwo{ PlayerId::PLAYER_TWO, 3 };
		Piece pieceFourPlayerTwo{ PlayerId::PLAYER_TWO, c_PusherPieceType };
		Piece pieceFivePlayerTwo{ PlayerId::PLAYER_TWO, 5 };

		// Player 2 moves first
		// Piece 1 of player 1 (1,2) will move freely to (2,2) (+1 movement)
		// Piece 2 of player 1 (3,2) will move to (2,2) pushing piece 1 back to (1,2) (+2 movements)
		// Piece 3 of player 2 (4,1) will fail to move into the board from the perimetter as its
		// target tile (3,1) is blocked by piece 5 of player 1 which hasn't moved yet.
		// Piece 4 of player 2 (2,1) will move north to (2,2), pushing piece 2 to (2,3), piece 4
		// of player 1 outside the board to (2,4) and piece 5 of player 2 (2,4 perimeter) 
		// even further to a non-existing tile, also removing it from play (+4 movements)
		// Piece 4 of player 1 will not move as it has been pushed outside the board
		// Piece 5 of player 1 (3,1) will move freely to (2,1) (+1 movement)
		board.GetTile(1, 2)->PlacePiece(&pieceOnePlayerOne);
		pieceOnePlayerOne.SetMovementDirection(Direction::EAST);
		board.GetTile(3, 2)->PlacePiece(&pieceTwoPlayerOne);
		pieceTwoPlayerOne.SetMovementDirection(Direction::WEST);
		board.GetTile(2, 3)->PlacePiece(&pieceFourPlayerOne);
		pieceFourPlayerOne.SetMovementDirection(Direction::SOUTH);
		board.GetTile(3, 1)->PlacePiece(&pieceFivePlayerOne);
		pieceFivePlayerOne.SetMovementDirection(Direction::WEST);

		board.GetTile(4, 1)->PlacePiece(&pieceThreePlayerTwo);
		pieceThreePlayerTwo.SetMovementDirection(Direction::WEST);
		board.GetTile(2, 1)->PlacePiece(&pieceFourPlayerTwo);
		pieceFourPlayerTwo.SetMovementDirection(Direction::NORTH);
		board.GetTile(2, 4)->PlacePiece(&pieceFivePlayerTwo);
		pieceFivePlayerTwo.SetMovementDirection(Direction::SOUTH);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
		EXPECT_EQ(executedMoves, 8);

		EXPECT_EQ(*board.GetTile(1, 2)->GetPiece(), pieceOnePlayerOne);
		EXPECT_EQ(*board.GetTile(2, 3)->GetPiece(), pieceTwoPlayerOne);
		EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), pieceFivePlayerOne);
		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), pieceFourPlayerTwo);
		EXPECT_EQ(board.GetTile(2, 4)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(4, 1)->GetPiece(), nullptr);

		EXPECT_EQ(pieceOnePlayerOne.IsInPlay(), true);
		EXPECT_EQ(pieceTwoPlayerOne.IsInPlay(), true);
		EXPECT_EQ(pieceFourPlayerOne.IsInPlay(), false);
		EXPECT_EQ(pieceFivePlayerOne.IsInPlay(), true);
		EXPECT_EQ(pieceThreePlayerTwo.IsInPlay(), false);
		EXPECT_EQ(pieceFourPlayerTwo.IsInPlay(), true);
		EXPECT_EQ(pieceFivePlayerTwo.IsInPlay(), false);
	}

	TEST(Board, LegalPlacementTiles) {
		Board board {};
		Piece pieceOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwo { PlayerId::PLAYER_ONE, 2 };

		auto legalPlacementTiles = board.GetLegalPlacementTiles();
		// On an empty board we should have as many options as the size of the perimeter
		// which is the play area size square minus the board size square minus the 4 corners
		constexpr Coordinate perimeterSize = c_PlayAreaSize * c_PlayAreaSize - c_BoardSize * c_BoardSize - 4;
		EXPECT_EQ(legalPlacementTiles.size(), perimeterSize);

		board.PlacePiece(legalPlacementTiles[0]->GetCoordinates(), &pieceOne);
		board.PlacePiece(legalPlacementTiles[1]->GetCoordinates(), &pieceTwo);

		auto legalTilesAfterPlacement = board.GetLegalPlacementTiles();
		EXPECT_EQ(legalTilesAfterPlacement.size(), perimeterSize - 2);
	}
}
