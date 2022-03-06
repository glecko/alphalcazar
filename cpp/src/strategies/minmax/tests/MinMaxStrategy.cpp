#include <gtest/gtest.h>

#include "MinMaxStrategy.hpp"

#include <Game.hpp>
#include <Board.hpp>
#include <Player.hpp>
#include <Tile.hpp>
#include <Piece.hpp>
#include <PlacementMove.hpp>

namespace Alphalcazar::Strategy::MinMax {
	TEST(MinMaxStrategy, TestObviousFirstMovement) {
		Game::Game game {};
		game.GetState().FirstMoveExecuted = false;
		game.GetState().PlayerWithInitiative = Game::PlayerId::PLAYER_TWO;

		/* 
		* Here we are going to set up a game where player 1 is about to win the game, except if
		* player 2 (who goes first) prevents it by placing a piece on the square player 1 needs to use
		* to win (2,4)
		* 
		* Piece 4 of player 1 will inevitabelty move to the center at (2,2).
		* Piece 5 of player 1 will attempt to move east to (2,1).
		* 
		* Player 2 does not have their pushing piece available. They can attempt to block the movement
		* of player 1's five piece, but then player 2 can win by playing anything that enters the (3,3) square.
		* If the movement is not blocked, player 2 can win by playing anything on (2,4). The only way for player 2 to
		* avoid an immediate loss is to play any piece on (2,4).
		*/
		auto* playerOnePieceFive = game.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(5);
		playerOnePieceFive->SetMovementDirection(Game::Direction::EAST);
		game.GetBoard().GetTile(1, 1)->PlacePiece(playerOnePieceFive);

		auto* playerOnePieceFour = game.GetPlayer(Game::PlayerId::PLAYER_ONE)->GetPiece(4);
		playerOnePieceFour->SetMovementDirection(Game::Direction::WEST);
		game.GetBoard().GetTile(3, 2)->PlacePiece(playerOnePieceFour);

		auto* playerTwoPieceFour = game.GetPlayer(Game::PlayerId::PLAYER_TWO)->GetPiece(4);
		playerTwoPieceFour->SetMovementDirection(Game::Direction::WEST);
		game.GetBoard().GetTile(1, 3)->PlacePiece(playerTwoPieceFour);

		auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO);

		// Player 2 should decide to play a piece on the (2,4) square regardless of the search depth
		for (Depth depth = 1; depth <= 2; depth++) {
			MinMaxStrategy strategy { depth };
			auto moveIndex = strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);
			auto move = legalMoves[moveIndex];

			EXPECT_TRUE(move.Coordinates.x == 2);
			EXPECT_TRUE(move.Coordinates.y == 4);
		}
	}
}
