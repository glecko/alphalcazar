#include <gtest/gtest.h>

#include "game/aliases.hpp"
#include "game/Board.hpp"
#include "game/Tile.hpp"
#include "game/Piece.hpp"
#include "game/parameters.hpp"

#include "testhelpers.hpp"

#include <array>

namespace Alphalcazar::Game {
	TEST(Board, SetupTiles) {
		Board board {};
		EXPECT_EQ(board.IsFull(), false);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);
		// We expect a square of c_PlayAreaSize size without the 4 corners
		auto tiles = board.GetTiles();
		EXPECT_EQ(tiles.size(), c_PlayAreaSize * c_PlayAreaSize - 4);
		EXPECT_EQ(board.GetPerimeterTiles().size(), c_PerimeterTileCount);
	}

	TEST(Board, PlacePiece) {
		Board board {};
		std::size_t pieceIndex = 0;
		for (auto& coordinates : Coordinates::GetPerimeterCoordinates()) {
			Piece piece = c_AllPieces[pieceIndex];
			pieceIndex++;
			if (pieceIndex >= (c_PieceTypes * 2) - 1) {
				pieceIndex = 0;
			}
			board.PlacePiece(coordinates, piece);
			EXPECT_EQ(*board.GetTile(coordinates)->GetPiece(), piece);
		}
		// Board should still be empty as we have only placed on perimeter tiles
		EXPECT_EQ(board.IsFull(), false);
	}

	TEST(Board, BoardIsFull) {
		Board board {};

		std::size_t pieceIndex = 0;
		for (Coordinate x = 1; x <= c_BoardSize; x++) {
			for (Coordinate y = 1; y <= c_BoardSize; y++) {
				Piece piece = c_AllPieces[pieceIndex];
				pieceIndex++;
				board.GetTile(x, y)->PlacePiece(piece);
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
		board.GetTile(1, 1)->PlacePiece(pieceOne);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(2, 1)->PlacePiece(pieceTwo);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(3, 1)->PlacePiece(pieceThree);
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
		board.GetTile(1, 1)->PlacePiece(pieceOne);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(1, 2)->PlacePiece(pieceTwo);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(1, 3)->PlacePiece(pieceThree);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(2, 2)->PlacePiece(pieceFour);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(3, 3)->PlacePiece(pieceFive);
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
		board.GetTile(1, 1)->PlacePiece(pieceOne);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(2, 2)->PlacePiece(pieceTwo);
		EXPECT_EQ(board.GetResult(), GameResult::NONE);

		board.GetTile(3, 3)->PlacePiece(pieceThree);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_TWO_WINS);

		board.GetTile(2, 1)->PlacePiece(pieceFour);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_TWO_WINS);

		board.GetTile(3, 2)->PlacePiece(pieceFive);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_TWO_WINS);
	}

	TEST(Board, MultipleRowCompletingPlayers) {
		// First we set up a board where player 1 has completed the center
		// column and wins. Next we set up pieces so that player 2 also completes a column
		// and the game should result in a draw or in no result depending on if draws are accepted
		std::vector<PieceSetup> pieceSetups {
			{ PlayerId::PLAYER_ONE, 1, Direction::NORTH, { 2, 1 } },
			{ PlayerId::PLAYER_ONE, 2, Direction::NORTH, { 2, 2 } },
			{ PlayerId::PLAYER_ONE, 3, Direction::SOUTH, { 2, 3 } },
		};
		Board board = SetupBoardForTesting(pieceSetups);
		EXPECT_EQ(board.GetResult(), GameResult::PLAYER_ONE_WINS);

		Piece pieceFour { PlayerId::PLAYER_TWO, 4 };
		Piece pieceFive { PlayerId::PLAYER_TWO, 5 };
		Piece pieceSix { PlayerId::PLAYER_TWO, 5 };
		board.GetTile(3, 1)->PlacePiece(pieceFour);
		board.GetTile(3, 2)->PlacePiece(pieceFive);
		board.GetTile(3, 3)->PlacePiece(pieceSix);
		EXPECT_EQ(board.GetResult(), c_AcceptDraws ? GameResult::DRAW : GameResult::NONE);
	}

	TEST(Board, PiecePushedIntoPerimeter) {
		// Piece 1 on (1,1) will move north to (1,2) (+1 movement)
		// Piece 2 on (3,1) will move south to (3,0). Since it exited the board it is removed from play (+1 movement)
		// Piece 5 on (2,2) will moves east to (1,2) pushing piece 1 outside the board (+2 movements)
		std::vector<PieceSetup> pieceSetups {
			{ PlayerId::PLAYER_ONE, c_PushablePieceType, Direction::NORTH, { 1, 1 } },
			{ PlayerId::PLAYER_ONE, 2, Direction::SOUTH, { 3, 1 } },
			{ PlayerId::PLAYER_TWO, 5, Direction::WEST, { 2, 2 } }
		};
		Board board = SetupBoardForTesting(pieceSetups);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_ONE);
		EXPECT_EQ(executedMoves, 4);

		EXPECT_EQ(*board.GetTile(1, 2)->GetPiece(), Piece(PlayerId::PLAYER_TWO, 5));
		EXPECT_EQ(board.GetTile(3, 0)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(0, 2)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(2, 2)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(3, 1)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetPieces().size(), 1);
	}

	TEST(Board, PushablePiecesDontPushEachOther) {
		// Two pushable pieces are facing each other. As pushable pieces cannot
		// push another pushable piece, we expect no movements to happen in this setup
		std::vector<PieceSetup> pieceSetups {
			{ PlayerId::PLAYER_ONE, c_PushablePieceType, Direction::NORTH, { 2, 2 } },
			{ PlayerId::PLAYER_TWO, c_PushablePieceType, Direction::SOUTH, { 2, 3 } }
		};
		Board board = SetupBoardForTesting(pieceSetups);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_ONE);
		EXPECT_EQ(executedMoves, 0);

		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), Piece(PlayerId::PLAYER_ONE, c_PushablePieceType));
		EXPECT_EQ(*board.GetTile(2, 3)->GetPiece(), Piece(PlayerId::PLAYER_TWO, c_PushablePieceType));
		EXPECT_EQ(board.GetPieces().size(), 2);
	}

	TEST(Board, PushablePieceMovements) {
		/*
		 * The pieces of player 2 move first.
		 *
		 * - Piece 1 of player 2 on (3,2) will move freely to (2,2) (+1 movement)
		 * - Piece 2 of player 2 on (1,2) will push piece 1 of player 2 back to (3,2) (+2 movements)
		 * - Piece 2 of player 1 on (2,0) perimeter will be unable to push piece 1 of player 1 on (2,1)
		 *   because piece 2 of player 2 on (2,2) is behind it. Since it's stuck on the perimeter, it is removed.
		 * - Piece 3 of player 2 on (3,1) will move north to (3,2) pushing piece 1 of player 2 to (3,3) (+2 movements)
		 * - Piece 5 of player 2 on (1,4) will freely enter the board to (1,3) (+1 movement)
		 */
		std::vector<PieceSetup> pieceSetups {
			{ PlayerId::PLAYER_ONE, c_PushablePieceType, Direction::EAST, { 2, 1 } },
			{ PlayerId::PLAYER_ONE, 2, Direction::NORTH, { 2, 0 } },

			{ PlayerId::PLAYER_TWO, c_PushablePieceType, Direction::WEST, { 3, 2 } },
			{ PlayerId::PLAYER_TWO, 2, Direction::EAST, { 1, 2 } },
			{ PlayerId::PLAYER_TWO, 3, Direction::NORTH, { 3, 1 } },
			{ PlayerId::PLAYER_TWO, 5, Direction::SOUTH, { 1, 4 } },
		};
		Board board = SetupBoardForTesting(pieceSetups);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
		EXPECT_EQ(executedMoves, 6);

		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), Piece(PlayerId::PLAYER_TWO, 2));
		EXPECT_EQ(board.GetTile(2, 0)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(1, 1)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(1, 2)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(1, 4)->GetPiece(), nullptr);
		EXPECT_EQ(*board.GetTile(3, 2)->GetPiece(), Piece(PlayerId::PLAYER_TWO, 3));
		EXPECT_EQ(*board.GetTile(3, 3)->GetPiece(), Piece(PlayerId::PLAYER_TWO, c_PushablePieceType));
		EXPECT_EQ(*board.GetTile(1, 3)->GetPiece(), Piece(PlayerId::PLAYER_TWO, 5));
		EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), Piece(PlayerId::PLAYER_ONE, c_PushablePieceType));
		EXPECT_EQ(board.GetPieces().size(), 5);
	}

	TEST(Board, PushingPieceMovements) {
		// Piece 2 (3,1) moves freely to (3,2) (+1 movement)
		// Piece 3 (1,2) moves freely to (2,2) (+1 movement)
		// The pushing piece (1,2) moves to (2,2), pushing piece 3 to (3,2)
		// and piece 2 outside the board (+3 movements)
		std::vector<PieceSetup> pieceSetups {
			{ PlayerId::PLAYER_ONE, 2, Direction::NORTH, { 3, 1 } },
			{ PlayerId::PLAYER_ONE, 3, Direction::SOUTH, { 2, 3 } },
			{ PlayerId::PLAYER_ONE, c_PusherPieceType, Direction::EAST, { 1, 2 } },
		};
		Board board = SetupBoardForTesting(pieceSetups);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
		EXPECT_EQ(executedMoves, 5);

		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), Piece(PlayerId::PLAYER_ONE, c_PusherPieceType));
		EXPECT_EQ(*board.GetTile(3, 2)->GetPiece(), Piece(PlayerId::PLAYER_ONE, 3));
		EXPECT_EQ(board.GetTile(4, 2)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetPieces().size(), 2);
	}

	TEST(Board, PushingPiecesPushingEachOther) {
		// The two pushing pieces are facing each other
		std::vector<PieceSetup> pieceSetups {
			{ PlayerId::PLAYER_ONE, c_PusherPieceType, Direction::SOUTH, { 2, 2 } },
			{ PlayerId::PLAYER_TWO, c_PusherPieceType, Direction::NORTH, { 2, 1 } },
		};
		Board board = SetupBoardForTesting(pieceSetups);

		{
			// If player 2 is the starting player, we expect both pieces to push each other
			// and end up in exactly the same positions, resulting in 4 movements (2 for each push)
			auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
			EXPECT_EQ(executedMoves, 4);

			EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), Piece(PlayerId::PLAYER_ONE, c_PusherPieceType));
			EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), Piece(PlayerId::PLAYER_TWO, c_PusherPieceType));

			EXPECT_EQ(board.GetPieces().size(), 2);
		}

		{
			// However, on the next turn, when player 1 moves first, their pushing piece on (2,2)
			// will push the pusher of player 2 off the board, after which the pusher of player 2
			// will not execute any movement because it will have been removed from play
			auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_ONE);
			EXPECT_EQ(executedMoves, 2);

			EXPECT_EQ(board.GetTile(2, 2)->GetPiece(), nullptr);
			EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), Piece(PlayerId::PLAYER_ONE, c_PusherPieceType));

			EXPECT_EQ(board.GetPieces().size(), 1);
		}
	}

	TEST(Board, ComplexBoardMovements) {
		/*
		 * Player 2 moves first
		 *
		 * - Piece 1 of player 1 (1,2) will move freely to (2,2) (+1 movement)
		 * - Piece 2 of player 1 (3,2) will move to (2,2) pushing piece 1 back to (1,2) (+2 movements)
		 * - Piece 3 of player 2 (4,1) will fail to move into the board from the perimetter as its
		 *   target tile (3,1) is blocked by piece 5 of player 1 which hasn't moved yet.
		 * - Piece 4 of player 2 (2,1) will move north to (2,2), pushing piece 2 to (2,3), piece 4
		 *   of player 1 outside the board to (2,4) and piece 5 of player 2 (2,4 perimeter)
		 *   even further to a non-existing tile, also removing it from play (+4 movements)
		 * - Piece 4 of player 1 will not move as it has been pushed outside the board
		 * - Piece 5 of player 1 (3,1) will move freely to (2,1) (+1 movement)
		 */
		std::vector<PieceSetup> pieceSetups {
			{ PlayerId::PLAYER_ONE, c_PushablePieceType, Direction::EAST, { 1, 2 } },
			{ PlayerId::PLAYER_ONE, 2, Direction::WEST, { 3, 2 } },
			{ PlayerId::PLAYER_ONE, c_PusherPieceType, Direction::SOUTH, { 2, 3 } },
			{ PlayerId::PLAYER_ONE, 5, Direction::WEST, { 3, 1 } },

			{ PlayerId::PLAYER_TWO, 3, Direction::WEST, { 4, 1 } },
			{ PlayerId::PLAYER_TWO, c_PusherPieceType, Direction::NORTH, { 2, 1 } },
			{ PlayerId::PLAYER_TWO, 5, Direction::SOUTH, { 2, 4 } },
		};
		Board board = SetupBoardForTesting(pieceSetups);

		auto executedMoves = board.ExecuteMoves(PlayerId::PLAYER_TWO);
		EXPECT_EQ(executedMoves, 8);

		EXPECT_EQ(*board.GetTile(1, 2)->GetPiece(), Piece(PlayerId::PLAYER_ONE, 1));
		EXPECT_EQ(*board.GetTile(2, 3)->GetPiece(), Piece(PlayerId::PLAYER_ONE, 2));
		EXPECT_EQ(*board.GetTile(2, 1)->GetPiece(), Piece(PlayerId::PLAYER_ONE, 5));
		EXPECT_EQ(*board.GetTile(2, 2)->GetPiece(), Piece(PlayerId::PLAYER_TWO, c_PusherPieceType));
		EXPECT_EQ(board.GetTile(2, 4)->GetPiece(), nullptr);
		EXPECT_EQ(board.GetTile(4, 1)->GetPiece(), nullptr);

		EXPECT_EQ(board.GetPieces().size(), 4);
	}

	TEST(Board, LegalPlacementTiles) {
		Board board {};
		Piece pieceOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwo { PlayerId::PLAYER_ONE, 2 };

		auto legalPlacementTiles = board.GetLegalPlacementCoordinates();
		// On an empty board we should have as many options as the size of the perimeter
		// which is the play area size square minus the board size square minus the 4 corners
		constexpr Coordinate perimeterSize = c_PlayAreaSize * c_PlayAreaSize - c_BoardSize * c_BoardSize - 4;
		EXPECT_EQ(legalPlacementTiles.size(), perimeterSize);

		board.PlacePiece(legalPlacementTiles[0], pieceOne);
		board.PlacePiece(legalPlacementTiles[1], pieceTwo);

		auto legalTilesAfterPlacement = board.GetLegalPlacementCoordinates();
		EXPECT_EQ(legalTilesAfterPlacement.size(), perimeterSize - 2);
	}

	TEST(Board, CopyPreservesState) {
		Board board {};
		Piece pieceOne { PlayerId::PLAYER_ONE, 1 };
		Piece pieceTwo { PlayerId::PLAYER_ONE, 2 };
		Piece pieceThree { PlayerId::PLAYER_TWO, 3 };
		Piece pieceFour { PlayerId::PLAYER_TWO, 4 };

		board.PlacePiece({ 0, 1 }, pieceOne);
		board.PlacePiece({ 1, 0 }, pieceTwo);
		board.PlacePiece({ 2, 0 }, pieceThree);
		board.PlacePiece({ 2, 2 }, pieceFour, Direction::EAST);

		Board boardCopy = board;
		EXPECT_NE(boardCopy.GetTile(0, 1), nullptr);
		EXPECT_NE(boardCopy.GetTile(1, 0), nullptr);
		EXPECT_NE(boardCopy.GetTile(2, 0), nullptr);
		EXPECT_NE(boardCopy.GetTile(2, 2), nullptr);

		EXPECT_EQ(*boardCopy.GetTile(0, 1)->GetPiece(), pieceOne);
		EXPECT_EQ(*boardCopy.GetTile(1, 0)->GetPiece(), pieceTwo);
		EXPECT_EQ(*boardCopy.GetTile(2, 0)->GetPiece(), pieceThree);
		EXPECT_EQ(*boardCopy.GetTile(2, 2)->GetPiece(), pieceFour);
	}

	TEST(Board, GetBoardPieces) {
		Board board{};
		Piece pieceOnePlayerOne{ PlayerId::PLAYER_ONE, 1 };
		Piece pieceOnePlayerTwo{ PlayerId::PLAYER_TWO, 1 };
		Piece pieceFourPlayerOne{ PlayerId::PLAYER_ONE, 4 };
		Piece pieceFourPlayerTwo{ PlayerId::PLAYER_TWO, 4 };

		EXPECT_EQ(board.GetPieces().size(), 0);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_ONE).size(), 0);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_ONE), 0);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_TWO).size(), 0);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_TWO), 0);
		board.PlacePiece({ 0, 1 }, pieceOnePlayerOne);
		board.PlacePiece({ 1, 0 }, pieceOnePlayerTwo);
		board.PlacePiece({ 2, 2 }, pieceFourPlayerOne, Direction::EAST);
		board.PlacePiece({ 3, 3 }, pieceFourPlayerTwo, Direction::WEST);

		{
			auto pieces = board.GetPieces();
			auto playerOnePieces = board.GetPieces(PlayerId::PLAYER_ONE);
			auto playerTwoPieces = board.GetPieces(PlayerId::PLAYER_TWO);
			EXPECT_EQ(pieces.size(), 4);
			EXPECT_EQ(playerOnePieces.size(), 2);
			EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_ONE), 2);
			EXPECT_EQ(playerTwoPieces.size(), 2);
			EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_TWO), 2);
			for (auto& [coordinates, piece] : pieces) {
				// All pieces should be either type 1 or 4
				EXPECT_TRUE(piece.GetType() == 4 || piece.GetType() == 1);
			}
		}


		Piece pieceTwoPlayerTwo{ PlayerId::PLAYER_TWO, 2 };
		Piece pieceThreePlayerTwo{ PlayerId::PLAYER_TWO, 3 };
		board.PlacePiece({ 1, 1 }, pieceTwoPlayerTwo, Direction::EAST);
		board.PlacePiece({ 2, 1 }, pieceThreePlayerTwo, Direction::WEST);

		{
			auto pieces = board.GetPieces();
			auto playerOnePieces = board.GetPieces(PlayerId::PLAYER_ONE);
			auto playerTwoPieces = board.GetPieces(PlayerId::PLAYER_TWO);
			EXPECT_EQ(pieces.size(), 6);
			EXPECT_EQ(playerOnePieces.size(), 2);
			EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_ONE), 2);
			EXPECT_EQ(playerTwoPieces.size(), 4);
			EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_TWO), 4);
		}
	}

	TEST(Board, GetBoardPiecesWithoutPerimeter) {
		Board board{};
		Piece pieceOnePlayerOne{ PlayerId::PLAYER_ONE, 1 };
		Piece pieceOnePlayerTwo{ PlayerId::PLAYER_TWO, 1 };
		Piece pieceTwoPlayerOne{ PlayerId::PLAYER_ONE, 2 };
		Piece pieceTwoPlayerTwo{ PlayerId::PLAYER_TWO, 2 };

		// Place a player 1 piece on the perimeter
		board.PlacePiece({ 0, 1 }, pieceOnePlayerOne);
		EXPECT_EQ(board.GetPieces(PlayerId::NONE, true).size(), 0);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_ONE, false).size(), 1);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_ONE, false), 1);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_ONE, true).size(), 0);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_ONE, true), 0);

		// Place a player 2 piece on the perimeter
		board.PlacePiece({ 2, 0 }, pieceOnePlayerTwo);
		EXPECT_EQ(board.GetPieces(PlayerId::NONE, true).size(), 0);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_TWO, false).size(), 1);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_TWO, false), 1);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_TWO, true).size(), 0);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_TWO, true), 0);

		// Place a player 1 piece on the center
		board.PlacePiece({ 2, 2 }, pieceTwoPlayerOne, Direction::NORTH);
		EXPECT_EQ(board.GetPieces(PlayerId::NONE, true).size(), 1);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_ONE, false).size(), 2);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_ONE, false), 2);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_ONE, true).size(), 1);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_ONE, true), 1);

		// Place a player 2 piece on a corner
		board.PlacePiece({ 3, 3 }, pieceTwoPlayerTwo, Direction::NORTH);
		EXPECT_EQ(board.GetPieces(PlayerId::NONE, true).size(), 2);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_TWO, false).size(), 2);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_TWO, false), 2);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_TWO, true).size(), 1);
		EXPECT_EQ(board.GetPieceCount(PlayerId::PLAYER_TWO, true), 1);
		EXPECT_EQ(board.GetPieces(PlayerId::NONE, false).size(), 4);
	}

	TEST(Board, GetPiecePlacements) {
		Board board{};
		Piece pieceOnePlayerOne{ PlayerId::PLAYER_ONE, 1 };
		Piece pieceOnePlayerTwo{ PlayerId::PLAYER_TWO, 1 };

		// Place a player 1 piece on the board
		board.PlacePiece({ 2, 2 }, pieceOnePlayerOne, Direction::NORTH);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_ONE).size(), 1);
		auto playerOnePlacements = board.GetPiecePlacements(PlayerId::PLAYER_ONE);
		for (std::size_t i = 0; i < playerOnePlacements.size(); i++) {
			EXPECT_TRUE(i == 0 ? playerOnePlacements[i] : !playerOnePlacements[i]);
		}

		// Place a player 2 piece on the board
		board.PlacePiece({ 2, 3 }, pieceOnePlayerTwo, Direction::NORTH);
		EXPECT_EQ(board.GetPieces(PlayerId::PLAYER_TWO).size(), 1);
		auto playerTwoPlacements = board.GetPiecePlacements(PlayerId::PLAYER_TWO);
		for (std::size_t i = 0; i < playerTwoPlacements.size(); i++) {
			EXPECT_TRUE(i == 0 ? playerTwoPlacements[i] : !playerTwoPlacements[i]);
		}
	}
}
