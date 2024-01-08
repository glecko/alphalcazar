#include <gtest/gtest.h>

#include "game/aliases.hpp"
#include "game/Game.hpp"
#include "game/Strategy.hpp"
#include "game/Tile.hpp"
#include "game/Board.hpp"
#include "game/PlacementMove.hpp"
#include "game/parameters.hpp"

namespace Alphalcazar::Game {
	class MockStrategy final : public Strategy {
	public:
		PlacementMove Execute(PlayerId playerId, const Utils::StaticVector<PlacementMove, c_MaxLegalMovesCount>&, const Game&) override {
			if (playerId == PlayerId::PLAYER_ONE) {
				return { { 0, 2 }, 3 };
			}
			return { { 0, 3 }, 2 };
		}
	};

	TEST(Game, InitialGameState) {
		const Game game{};

		EXPECT_EQ(game.GetActivePlayer(), PlayerId::PLAYER_ONE);
		EXPECT_EQ(game.GetPlayerByInitiative(true), PlayerId::PLAYER_ONE);
		EXPECT_EQ(game.GetPlayerByInitiative(false), PlayerId::PLAYER_TWO);

		EXPECT_EQ(game.GetLegalMoves(PlayerId::PLAYER_ONE).size(), c_PieceTypes * c_PerimeterTileCount);
		EXPECT_EQ(game.GetLegalMoves(PlayerId::PLAYER_TWO).size(), c_PieceTypes * c_PerimeterTileCount);
		EXPECT_EQ(game.GetPiecesInHand(PlayerId::PLAYER_ONE).size(), c_PieceTypes);
		EXPECT_EQ(game.GetPiecesInHand(PlayerId::PLAYER_TWO).size(), c_PieceTypes);
	}

	/*!
	 * \brief Checks if the amount of legal moves is consistent with the pieces in hand of a player
	 *
	 * This condition should be true at with any state of the game.
	 */
	void CheckHandPiecesLegalMovesConsistency(const Game& game, PlayerId playerId) {
		EXPECT_EQ(game.GetPiecesInHand(playerId).size() * c_PerimeterTileCount, game.GetLegalMoves(playerId).size());
	}

	TEST(Game, TestLegalMovements) {
		Game game{};

		CheckHandPiecesLegalMovesConsistency(game, PlayerId::PLAYER_ONE);
		CheckHandPiecesLegalMovesConsistency(game, PlayerId::PLAYER_TWO);

		game.PlayNextPlacementMove({ { 0, 2 }, 5 });
		game.PlayNextPlacementMove({ { 0, 3 }, 5 });

		CheckHandPiecesLegalMovesConsistency(game, PlayerId::PLAYER_ONE);
		CheckHandPiecesLegalMovesConsistency(game, PlayerId::PLAYER_TWO);

		MockStrategy strategy;
		game.PlayTurn(strategy, strategy);

		CheckHandPiecesLegalMovesConsistency(game, PlayerId::PLAYER_ONE);
		CheckHandPiecesLegalMovesConsistency(game, PlayerId::PLAYER_TWO);
	}

	TEST(Game, TestPlayNextPlacementMove) {
		Game game{};

		// After playing one placement move, the turn is still unfinished
		// We expect the movement to have been played by player 1
		game.PlayNextPlacementMove({ { 0, 3 }, 3 });
		EXPECT_EQ(game.GetState().Turn, 0);
		EXPECT_EQ(game.GetState().FirstMoveExecuted, true);
		EXPECT_EQ(game.GetActivePlayer(), PlayerId::PLAYER_TWO);
		EXPECT_EQ(game.GetBoard().GetTile(0, 3)->GetPiece().GetOwner(), PlayerId::PLAYER_ONE);
		EXPECT_EQ(game.GetBoard().GetTile(0, 3)->GetPiece().GetType(), 3);

		// After playing the second placement move, we expect the end turn phase to have been
		// evaluated and the pieces to have moved.
		game.PlayNextPlacementMove({ { 2, 0 }, 5 });
		EXPECT_EQ(game.GetState().Turn, 1);
		EXPECT_EQ(game.GetState().FirstMoveExecuted, false);
		EXPECT_EQ(game.GetActivePlayer(), PlayerId::PLAYER_TWO);
		EXPECT_FALSE(game.GetBoard().GetTile(0, 3)->HasPiece());
		EXPECT_FALSE(game.GetBoard().GetTile(2, 0)->HasPiece());
		EXPECT_EQ(game.GetBoard().GetTile(1, 3)->GetPiece().GetType(), 3);
		EXPECT_EQ(game.GetBoard().GetTile(2, 1)->GetPiece().GetType(), 5);

		// We also expect the piece spawned from move 2 to belong to player 2
		EXPECT_EQ(game.GetBoard().GetTile(2, 1)->GetPiece().GetOwner(), PlayerId::PLAYER_TWO);

		// On the next turn, we expect the first placement move to be played by player 2,
		// as the player with initiative should have switched
		game.PlayNextPlacementMove({ { 0, 1 }, 2 });
		EXPECT_EQ(game.GetState().Turn, 1);
		EXPECT_EQ(game.GetActivePlayer(), PlayerId::PLAYER_ONE);
		EXPECT_EQ(game.GetState().FirstMoveExecuted, true);
		EXPECT_EQ(game.GetBoard().GetTile(0, 1)->GetPiece().GetOwner(), PlayerId::PLAYER_TWO);
		EXPECT_EQ(game.GetState().Turn, 1);
		EXPECT_EQ(game.GetState().FirstMoveExecuted, true);
	}

	TEST(Game, PlayMockStrategy) {
		Game game{};
		MockStrategy strategy;

		// Player 1 plays piece 3 at (0,2) and player 2 plays piece 2 at (0,3)
		// Afer this, the pieces will have moved to (1,2) and (1,3) respectively
		auto result = game.PlayTurn(strategy, strategy);
		EXPECT_EQ(result, GameResult::NONE);

		// We expect the active & initiative player to have changed to player 2
		// as we should be at the beginning (no moves played) of the next turn
		EXPECT_EQ(game.GetActivePlayer(), PlayerId::PLAYER_TWO);
		EXPECT_EQ(game.GetPlayerByInitiative(true), PlayerId::PLAYER_TWO);
		EXPECT_EQ(game.GetState().Turn, 1);
		EXPECT_EQ(game.GetState().FirstMoveExecuted, false);

		// We expect player 1 to be missing piece 3 in their hand
		auto playerOnePieces = game.GetPiecesInHand(PlayerId::PLAYER_ONE);
		EXPECT_EQ(playerOnePieces.size(), c_PieceTypes - 1);
		auto pieceThreeIter = std::find(playerOnePieces.begin(), playerOnePieces.end(), Piece{ PlayerId::PLAYER_ONE, 3 });
		EXPECT_EQ(pieceThreeIter, playerOnePieces.end());

		// We expect player 2 to be missing piece 2 in their hand
		auto playerTwoPieces = game.GetPiecesInHand(PlayerId::PLAYER_TWO);
		EXPECT_EQ(playerTwoPieces.size(), c_PieceTypes - 1);
		auto pieceTwoIter = std::find(playerTwoPieces.begin(), playerTwoPieces.end(), Piece{ PlayerId::PLAYER_TWO, 2 });
		EXPECT_EQ(pieceTwoIter, playerTwoPieces.end());

		auto boardPieces = game.GetBoard().GetPieces();
		EXPECT_EQ(boardPieces.size(), 2);

		// We expect piece 3 of player 1 to be on (1,2)
		auto pieceThreeBoardIter = std::find_if(boardPieces.begin(), boardPieces.end(), [](const std::pair<Coordinates, Piece>& pieceInfo) {
			return pieceInfo.second.GetType() == 3 && pieceInfo.second.GetOwner() == PlayerId::PLAYER_ONE;
		});
		EXPECT_NE(pieceThreeBoardIter, boardPieces.end());
		EXPECT_TRUE(pieceThreeBoardIter->first.x == 1 && pieceThreeBoardIter->first.y == 2);

		// We expect piece 2 of player 2 to be on (1,3)
		auto pieceTwoBoardIter = std::find_if(boardPieces.begin(), boardPieces.end(), [](const std::pair<Coordinates, Piece>& pieceInfo) {
			return pieceInfo.second.GetType() == 2 && pieceInfo.second.GetOwner() == PlayerId::PLAYER_TWO;
		});
		EXPECT_NE(pieceTwoBoardIter, boardPieces.end());
		EXPECT_TRUE(pieceTwoBoardIter->first.x == 1 && pieceTwoBoardIter->first.y == 3);
	}
}
