#include <gtest/gtest.h>

#include "minmax/LegalMovements.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>
#include <game/Piece.hpp>
#include <game/Board.hpp>
#include <game/parameters.hpp>
#include <game/PlacementMove.hpp>

#include "setuphelpers.hpp"

namespace Alphalcazar::Strategy::MinMax {
	TEST(LegalMovements, CenterVerticalRowSymmetries) {
		Game::Game game {};

		// An empty board should have both X and Y symmetry
		auto [xSymmetryEmpty, ySymmetryEmpty] = GetBoardSymmetries(game.GetBoard());
		EXPECT_TRUE(xSymmetryEmpty);
		EXPECT_TRUE(ySymmetryEmpty);

		const Game::Piece pieceOne { Game::PlayerId::PLAYER_ONE, 1 };
		game.GetBoard().PlacePiece({ 2, 2 }, pieceOne, Game::Direction::NORTH);

		// A piece in the center pointing north should create Y symmetry
		auto [xSymmetryCenterPiece, ySymmetryCenterPiece] = GetBoardSymmetries(game.GetBoard());
		EXPECT_FALSE(xSymmetryCenterPiece);
		EXPECT_TRUE(ySymmetryCenterPiece);

		const Game::Piece pieceTwo { Game::PlayerId::PLAYER_ONE, 2 };
		game.GetBoard().PlacePiece({ 2, 3 }, pieceTwo, Game::Direction::EAST);

		// Once we have added a second piece, still in the center row but facing east, all symmetries should be broken
		auto [xSymmetryTwoPieces, ySymmetryTwoPieces] = GetBoardSymmetries(game.GetBoard());
		EXPECT_FALSE(xSymmetryTwoPieces);
		EXPECT_FALSE(ySymmetryTwoPieces);
	}

	TEST(LegalMovements, CornerSymmetries) {
		const std::vector<PieceSetup> pieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 1, Game::Direction::NORTH, { 1, 1 } },
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		// A single piece on a corner breaks X and Y symmetries
		auto [xSymmetry, ySymmetry] = GetBoardSymmetries(game.GetBoard());
		EXPECT_FALSE(xSymmetry);
		EXPECT_FALSE(ySymmetry);
	}

	TEST(LegalMovements, PerimeterSymmetries) {
		std::vector<PieceSetup> pieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 1, Game::Direction::WEST, { 4, 2 } },
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		// The perimeter piece is on the x-axis and faces west, meaning we should still have
		// x-axis symmetry
		auto [xSymmetry, ySymmetry] = GetBoardSymmetries(game.GetBoard());
		EXPECT_TRUE(xSymmetry);
		EXPECT_FALSE(ySymmetry);

		Game::Piece pieceTwo { Game::PlayerId::PLAYER_ONE, 2 };
		game.GetBoard().PlacePiece({ 2, 4 }, pieceTwo, Game::Direction::SOUTH);

		// After placing a second piece (which on its own would still make the board have x-axis symmetry)
		// we check that the combination of both pieces breaks both symmetries, each piece breaking 1 axis.
		auto [xSymmetryTwoPieces, ySymmetryTwoPieces] = GetBoardSymmetries(game.GetBoard());
		EXPECT_FALSE(xSymmetryTwoPieces);
		EXPECT_FALSE(ySymmetryTwoPieces);
	}

	TEST(LegalMovements, FilterEmptyBoardSymmetricMovements) {
		Game::Game game {};
		auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_ONE);
		// On an empty board, all combinations of piece type and perimeter tile should create a legal move
		EXPECT_EQ(legalMoves.size(), Game::c_PieceTypes * game.GetBoard().GetPerimeterTiles().size());

		auto filteredMoves = SortAndFilterMovements(Game::PlayerId::PLAYER_ONE, legalMoves, game.GetBoard());

		// After filtering symmetric movements on an empty board, we expect to only have 2 options for each piece
		EXPECT_EQ(filteredMoves.size(), Game::c_PieceTypes * 2);
	}

	TEST(LegalMovements, FilterXAxisSymmetryMovements) {
		std::vector<PieceSetup> pieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 4, Game::Direction::SOUTH, { 2, 1 } },
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO);
		auto filteredMoves = SortAndFilterMovements(Game::PlayerId::PLAYER_ONE, legalMoves, game.GetBoard());

		/*
		 * On a board with either x-axis or y-axis symmetry (but not both), the amount of available
		 * perimeter tiles we expect to be available for each piece goes as follows:
		 *
		 * 1) One full side (of size BoardSize)
		 * 2) Two sides cut in half (rounding up to include the center row in boards with odd sizes)
		 */
		const float expectedPerimeterTiles = Game::c_BoardSize + 2 * (std::ceil(Game::c_BoardSize / 2.f));
		EXPECT_EQ(filteredMoves.size(), static_cast<std::size_t>(expectedPerimeterTiles) * Game::c_PieceTypes);
	}

	TEST(LegalMovements, FilterNoSymmetryMovements) {
		std::vector<PieceSetup> pieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::SOUTH, { 2, 3 } },
			{ Game::PlayerId::PLAYER_ONE, 4, Game::Direction::EAST, { 2, 1 } },
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_ONE);
		auto filteredMoves = SortAndFilterMovements(Game::PlayerId::PLAYER_ONE, legalMoves, game.GetBoard());

		// On this board, that has no symmetry, we expect all legal moves to be available after filtering
		EXPECT_EQ(filteredMoves.size(), legalMoves.size());
		// The legal moves size should be equal to all perimeter tiles (they are all empty) times
		// al pieces minus 2, as player 1 has already placed 2 pieces on the board
		EXPECT_EQ(filteredMoves.size(), (Game::c_PieceTypes - 2) * game.GetBoard().GetPerimeterTiles().size());
	}

	TEST(LegalMovements, SortLegalMovements) {
		std::vector<PieceSetup> pieceSetups{
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::SOUTH, { 1, 1 } },
			{ Game::PlayerId::PLAYER_TWO, 2, Game::Direction::EAST, { 2, 2 } },
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		Utils::StaticVector<Game::PlacementMove, Game::c_MaxLegalMovesCount> legalMoves;
		// This movement will result in the piece not entering the board
		legalMoves.insert({ { 1, 0 }, 3 });
		// This movement will enter in the center lane
		legalMoves.insert({ { 2, 0 }, 3 });
		// This movement will enter in a lateral lane
		legalMoves.insert({ { 3, 0 }, 3 });
		const auto sortedMoves = SortAndFilterMovements(Game::PlayerId::PLAYER_ONE, legalMoves, game.GetBoard());

		EXPECT_EQ(legalMoves.size(), sortedMoves.size());

		// We expect the movement that causes the piece to enter in the center lane
		// to be first, followed by the movement that causes the piece to enter the lateral lane,
		// and the movement that causes the piece not to enter at all last
		EXPECT_EQ(sortedMoves[0].Coordinates.x, 2);
		EXPECT_EQ(sortedMoves[1].Coordinates.x, 3);
		EXPECT_EQ(sortedMoves[2].Coordinates.x, 1);
	}
}
