#include <gtest/gtest.h>

#include "LegalMovements.hpp"
#include "config.hpp"

#include <Game.hpp>
#include <Piece.hpp>
#include <Tile.hpp>
#include <Board.hpp>
#include <parameters.hpp>
#include <PlacementMove.hpp>

#include <vector>

namespace Alphalcazar::Strategy::MinMax {
	TEST(LegalMovements, CenterVerticalRowSymmetries) {
		Game::Game game {};

		// An empty board should have both X and Y symmetry
		auto [xSymmetryEmpty, ySymmetryEmpty] = GetBoardSymmetries(game);
		EXPECT_TRUE(xSymmetryEmpty);
		EXPECT_TRUE(ySymmetryEmpty);

		Game::Piece pieceOne { Game::PlayerId::PLAYER_ONE, 1 };
		game.GetBoard().PlacePiece({ 2, 2 }, pieceOne, Game::Direction::NORTH);

		// A piece in the center pointing north should create Y symmetry
		auto [xSymmetryCenterPiece, ySymmetryCenterPiece] = GetBoardSymmetries(game);
		EXPECT_FALSE(xSymmetryCenterPiece);
		EXPECT_TRUE(ySymmetryCenterPiece);

		Game::Piece pieceTwo { Game::PlayerId::PLAYER_ONE, 2 };
		game.GetBoard().PlacePiece({ 2, 3 }, pieceTwo, Game::Direction::EAST);

		// Once we have added a second piece, still in the center row but facing east, all symmetries should be broken
		auto [xSymmetryTwoPieces, ySymmetryTwoPieces] = GetBoardSymmetries(game);
		EXPECT_FALSE(xSymmetryTwoPieces);
		EXPECT_FALSE(ySymmetryTwoPieces);
	}

	TEST(LegalMovements, CornerSymmetries) {
		Game::Game game {};

		Game::Piece pieceOne { Game::PlayerId::PLAYER_ONE, 1 };
		game.GetBoard().GetTile(1, 1)->PlacePiece(pieceOne);
		game.GetBoard().GetTile(1, 1)->GetPiece()->SetMovementDirection(Game::Direction::NORTH);

		// A single piece on a corner breaks X and Y symmetries
		auto [xSymmetry, ySymmetry] = GetBoardSymmetries(game);
		EXPECT_FALSE(xSymmetry);
		EXPECT_FALSE(ySymmetry);
	}

	TEST(LegalMovements, PerimeterSymmetries) {
		Game::Game game {};

		Game::Piece pieceOne { Game::PlayerId::PLAYER_ONE, 1 };
		game.GetBoard().PlacePiece({ 4, 2 }, pieceOne, Game::Direction::WEST);

		// The perimeter piece is on the x-axis and faces west, meaning we should still have
		// x-axis symmetry
		auto [xSymmetry, ySymmetry] = GetBoardSymmetries(game);
		EXPECT_TRUE(xSymmetry);
		EXPECT_FALSE(ySymmetry);

		Game::Piece pieceTwo { Game::PlayerId::PLAYER_ONE, 2 };
		game.GetBoard().PlacePiece({ 2, 4 }, pieceTwo, Game::Direction::SOUTH);

		// After placing a second piece (which on its own would still make the board have x-axis symmetry)
		// we check that the combination of both pieces breaks both symmetries, each piece breaking 1 axis.
		auto [xSymmetryTwoPieces, ySymmetryTwoPieces] = GetBoardSymmetries(game);
		EXPECT_FALSE(xSymmetryTwoPieces);
		EXPECT_FALSE(ySymmetryTwoPieces);
	}

	TEST(LegalMovements, FilterEmptyBoardSymmetricMovements) {
		Game::Game game {};
		auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_ONE);
		// On an empty board, all combinations of piece type and perimeter tile should create a legal move
		EXPECT_EQ(legalMoves.size(), Game::c_PieceTypes * game.GetBoard().GetPerimeterTiles().size());

		FilterSymmetricMovements(legalMoves, game);

		// After filtering symmetric movements on an empty board, we expect to only have 2 options for each piece
		EXPECT_EQ(legalMoves.size(), Game::c_PieceTypes * 2);
	}

	TEST(LegalMovements, FilterXAxisSymmetryMovements) {
		Game::Game game {};

		Game::Piece pieceFour { Game::PlayerId::PLAYER_ONE, 4 };
		game.GetBoard().PlacePiece({ 2, 1 }, pieceFour, Game::Direction::SOUTH);

		auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO);
		FilterSymmetricMovements(legalMoves, game);

		/*
		 * On a board with either x-axis or y-axis symmetry (but not both), the amount of available
		 * perimeter tiles we expect to be available for each piece goes as follows:
		 * 
		 * 1) One full side (of size BoardSize)
		 * 2) Two sides cut in half (rounding up to include the center row in boards with odd sizes)
		 */
		const float expectedPerimeterTiles = Game::c_BoardSize + 2 * (std::ceil(Game::c_BoardSize / 2.f));
		EXPECT_EQ(legalMoves.size(), static_cast<std::size_t>(expectedPerimeterTiles) * Game::c_PieceTypes);
	}

	TEST(LegalMovements, FilterNoSymmetryMovements) {
		Game::Game game {};

		Game::Piece pieceTwo { Game::PlayerId::PLAYER_ONE, 2 };
		game.GetBoard().PlacePiece({ 2, 3 }, pieceTwo, Game::Direction::SOUTH);

		Game::Piece pieceFour { Game::PlayerId::PLAYER_ONE, 4 };
		game.GetBoard().PlacePiece({ 2, 1 }, pieceFour, Game::Direction::EAST);

		auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_ONE);
		auto legalMovesSize = legalMoves.size();
		FilterSymmetricMovements(legalMoves, game);

		// On this board, that has no symmetry, we expect all legal moves to be available after filtering
		EXPECT_EQ(legalMoves.size(), legalMovesSize);
		// The legal moves size should be equal to all perimeter tiles (they are all empty) times
		// al pieces minus 2, as player 1 has already placed 2 pieces on the board
		EXPECT_EQ(legalMoves.size(), (Game::c_PieceTypes - 2) * game.GetBoard().GetPerimeterTiles().size());
	}
}
