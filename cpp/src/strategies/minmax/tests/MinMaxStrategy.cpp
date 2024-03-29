#include <gtest/gtest.h>

#include "minmax/MinMaxStrategy.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>
#include <game/parameters.hpp>
#include <game/PlacementMove.hpp>

#include "setuphelpers.hpp"

namespace Alphalcazar::Strategy::MinMax {
	TEST(MinMaxStrategy, TestWinningSecondMoveDepthOne) {
		/*
		 * Player 2 goes second and has the opportunity to immediatelly win the game
		 * by placing their 2 piece on (4,2) that will move to (3,2).
		 * 
		 * The 2 and 4 pieces of player 2 will move to (1,2) and (2,2) respectively, completing the
		 * horizontal center row and winning the game.
		 * 
		 * It's the only winning move as the (3,2) is about to be occupied by the opponent's 3,
		 * and the 2 piece is the only non-pushable piece player 2 has that moves before that 3 piece.
		 */
		const std::vector<PieceSetup> pieceSetups {
			{ Game::PlayerId::PLAYER_TWO, 3, Game::Direction::NORTH, { 1, 1 } },
			{ Game::PlayerId::PLAYER_TWO, 4, Game::Direction::NORTH, { 2, 1 } },

			{ Game::PlayerId::PLAYER_ONE, 3, Game::Direction::SOUTH, { 3, 3 } },
			{ Game::PlayerId::PLAYER_ONE, 4, Game::Direction::EAST, { 0, 3 } }
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		// Results should be the same independently of depth
		const auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO);
		Game::Coordinates winningCoordinates { 4, 2 };
		for (Depth depth = 1; depth <= 2; depth++) {
			MinMaxStrategy strategy { depth };
			auto move = strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);

			EXPECT_EQ(move.Coordinates, winningCoordinates);
			EXPECT_EQ(move.PieceType, 2);
			EXPECT_EQ(strategy.GetLastExecutedMoveScore(), c_WinConditionScore);
		}
	}

	TEST(MinMaxStrategy, TestObviousFirstMovement) {
		/* 
		 * Here we are going to set up a game where player 1 is about to win the game, except if
		 * player 2 (who goes first) prevents it by placing a piece on the square player 1 needs to use
		 * to win (2,4)
		 * 
		 * Piece 4 of player 1 will inevitably move to the center at (2,2).
		 * Piece 5 of player 1 will attempt to move east to (2,1).
		 * 
		 * Player 2 does not have their pushing piece available. They can attempt to block the movement
		 * of player 1's five piece, but then player 2 can win by playing anything that enters the (3,3) square.
		 * If the movement is not blocked, player 2 can win by playing anything on (2,4). The only way for player 2 to
		 * avoid an immediate loss is to play any piece on (2,4).
		 */
		const std::vector<PieceSetup> pieceSetups {
			{ Game::PlayerId::PLAYER_ONE, 5, Game::Direction::EAST, { 1, 1 } },
			{ Game::PlayerId::PLAYER_ONE, 4, Game::Direction::WEST, { 3, 2 } },

			{ Game::PlayerId::PLAYER_TWO, 4, Game::Direction::WEST, { 1, 2 } }
		};
		const Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_TWO, false, pieceSetups);

		const auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO);

		// Player 2 should decide to play a piece on the (2,4) square regardless of the search depth
		// and regardless of if the strategy is executed on multiple or a single thread
		for (Depth depth = 1; depth <= 2; depth++) {
			{
				MinMaxStrategy strategy{ depth, true };
				auto move = strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);

				EXPECT_EQ(move.Coordinates.x, 2);
				EXPECT_EQ(move.Coordinates.y, 4);
			}

			{
				MinMaxStrategy strategy{ depth, false };
				auto move = strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);

				EXPECT_EQ(move.Coordinates.x, 2);
				EXPECT_EQ(move.Coordinates.y, 4);
			}
		}
	}

	TEST(MinMaxStrategy, TestPlayerMustUserPusherPiece) {
		/*
		 * Player 2 has moved first and has played a useless move (a 1 piece that won't enter the board)
		 * However, 3 of their pieces are about to complete the center horizontal row.
		 * 
		 * Player 1 can only avoid losing this turn by player the pushing piece on either
		 * the (2,0) or the (2,4) tile or on the (0,3) tile.
		 * 
		 * No other piece can prevent a loss at the end of this turn. Player 1 does not have the 2 piece available,
		 * which could block the 3 piece from moving to the winning position at (1,2). Their 3 piece does not move quick
		 * enough as player 1 does not have the innitiative this turn. The only piece that can move before the opponent's
		 * 2 and 3 is the 1 piece, which will be pushed and won't prevent the loss.
		 */
		const std::vector<PieceSetup> pieceSetups {
			{ Game::PlayerId::PLAYER_TWO, 5, Game::Direction::SOUTH, { 2, 3 } },
			{ Game::PlayerId::PLAYER_TWO, 2, Game::Direction::SOUTH, { 3, 3 } },
			{ Game::PlayerId::PLAYER_TWO, 3, Game::Direction::NORTH, { 1, 1 } },
			// Useless move (a 1 piece that won't enter the board because the tile is blocked) played by player 2 this turn
			{ Game::PlayerId::PLAYER_TWO, 1, Game::Direction::WEST, { 4, 3 } },

			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::WEST, { 1, 3 } }
		};
		const Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_TWO, true, pieceSetups);

		const auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_ONE);

		MinMaxStrategy strategy { 1 };
		const auto move = strategy.Execute(Game::PlayerId::PLAYER_ONE, legalMoves, game);

		// Positions at which the pushing piece can be played to avoid an immediate loss
		std::vector<Game::Coordinates> expectedCoordinates {
			{ 2, 0 },
			{ 2, 4 },
			{ 0, 3 }
		};
		EXPECT_EQ(move.PieceType, Game::c_PusherPieceType);
		EXPECT_NE(std::find(expectedCoordinates.begin(), expectedCoordinates.end(), move.Coordinates), expectedCoordinates.end());
	}

	TEST(MinMaxStrategy, TestGameLostOnDepthTwo) {
		/*
		 * At this position, player 2 is completely losing at depth 2
		 * They can avoid losing this round by playing the pushing piece at (2,0), but next round they will
		 * be unable to block both squares where P1 can mate, and will not have the pushing piece in hand.
		 */
		const std::vector<PieceSetup> pieceSetups {
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::SOUTH, { 2, 3 } },
			{ Game::PlayerId::PLAYER_ONE, 3, Game::Direction::EAST, { 1, 2 } },
			// The first move played this turn
			{ Game::PlayerId::PLAYER_ONE, 4, Game::Direction::EAST, { 0, 2 } },

			{ Game::PlayerId::PLAYER_TWO, 1, Game::Direction::NORTH, { 2, 2 } },
			{ Game::PlayerId::PLAYER_TWO, 2, Game::Direction::WEST, { 3, 2 } }
		};
		Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		const auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO);

		constexpr Game::Coordinates expectedCoordinates = { 2, 0 };
		constexpr Game::PieceType expectedPieceType = Game::c_PusherPieceType;
		constexpr Score expectedScore = -c_WinConditionScore + c_DepthScorePenalty;
		{
			MinMaxStrategy strategy{ 2 };
			auto move = strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);

			// Player 2 should postpone defeat this turn by playing the pusher piece at (2,0)
			EXPECT_EQ(move.PieceType, expectedPieceType);
			EXPECT_TRUE(move.Coordinates.x == expectedCoordinates.x && move.Coordinates.y == expectedCoordinates.y);
			// But should be aware that the move implies a loss in 2 turns
			EXPECT_EQ(strategy.GetLastExecutedMoveScore(), expectedScore);
		}

		// At depth 3, the strategy should return exactly the same result
		{
			MinMaxStrategy strategy{ 3 };
			const auto move = strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);

			EXPECT_EQ(move.PieceType, expectedPieceType);
			EXPECT_TRUE(move.Coordinates.x == expectedCoordinates.x && move.Coordinates.y == expectedCoordinates.y);
			EXPECT_EQ(strategy.GetLastExecutedMoveScore(), expectedScore);
		}

		// At depth 1, the strategy should not detect the loss
		{
			MinMaxStrategy strategy{ 1 };
			strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);
			EXPECT_TRUE(strategy.GetLastExecutedMoveScore() > -c_WinConditionScore + c_DepthScorePenalty * 10);
		}
	}

	TEST(MinMaxStrategy, BlackWidowTest) {
		/*
		 * The following situation is hard to see for a human, but completely winning for player 2 at depth 2.
		 * They only has piece 5 in hand, and as long as they keeps it for the next move (makes it not enter the board)
		 * they will be able to use it next round to win the game, no reply from player 1 possible.
		 */
		const std::vector<PieceSetup> pieceSetups {
			{ Game::PlayerId::PLAYER_ONE, 1, Game::Direction::EAST, { 2, 3 } },
			{ Game::PlayerId::PLAYER_ONE, 2, Game::Direction::WEST, { 3, 2 } },
			// The first move played this turn
			{ Game::PlayerId::PLAYER_ONE, 3, Game::Direction::EAST, { 0, 3 } },
			{ Game::PlayerId::PLAYER_ONE, 4, Game::Direction::EAST, { 1, 3 } },
			{ Game::PlayerId::PLAYER_ONE, 5, Game::Direction::EAST, { 1, 1 } },

			{ Game::PlayerId::PLAYER_TWO, 1, Game::Direction::NORTH, { 2, 1 } },
			{ Game::PlayerId::PLAYER_TWO, 2, Game::Direction::WEST, { 3, 1 } },
			{ Game::PlayerId::PLAYER_TWO, 3, Game::Direction::WEST, { 3, 3 } },
			{ Game::PlayerId::PLAYER_TWO, 4, Game::Direction::EAST, { 2, 2 } }
		};
		const Game::Game game = SetupGameForMinMaxTesting(Game::PlayerId::PLAYER_ONE, true, pieceSetups);

		MinMaxStrategy strategy { 2 };
		const auto legalMoves = game.GetLegalMoves(Game::PlayerId::PLAYER_TWO);
		const auto move = strategy.Execute(Game::PlayerId::PLAYER_TWO, legalMoves, game);

		// These are the perimeter tiles at which placing the 5 piece would cause it to enter the board
		// Player 2 must place the piece on any tile except these ones to be able to win next round.
		std::vector<Game::Coordinates> tilesWhereFiveWouldEnter {
			{ 0, 2 },
			{ 1, 4 },
			{ 0, 3 },
			{ 2, 0 }
		};
		EXPECT_EQ(move.PieceType, 5);
		EXPECT_EQ(std::find(tilesWhereFiveWouldEnter.begin(), tilesWhereFiveWouldEnter.end(), move.Coordinates), tilesWhereFiveWouldEnter.end());
		EXPECT_EQ(strategy.GetLastExecutedMoveScore(), c_WinConditionScore - c_DepthScorePenalty);
	}
}
