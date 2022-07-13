#include "testhelpers.hpp"

namespace Alphalcazar::Game {
	/// Populates a board's initial position with a list of \ref PieceSetup
	void PopulateBoardWithPieceSetups(Board& board, const std::vector<PieceSetup>& pieceSetups) {
		for (auto& pieceSetup : pieceSetups) {
			Piece piece{ pieceSetup.PlayerId, pieceSetup.PieceType };
			board.PlacePiece(pieceSetup.Coordinates, piece, pieceSetup.Direction);
		}
	}

	Game SetupGameForTesting(PlayerId playerWithInitiative, bool firstMoveExecuted, const std::vector<PieceSetup>& pieceSetups) {
		Game game{};
		game.GetState().FirstMoveExecuted = firstMoveExecuted;
		game.GetState().PlayerWithInitiative = playerWithInitiative;
		PopulateBoardWithPieceSetups(game.GetBoard(), pieceSetups);
		return game;
	}

	
	Board SetupBoardForTesting(const std::vector<PieceSetup>& pieceSetups) {
		Board board{};
		PopulateBoardWithPieceSetups(board, pieceSetups);
		return board;
	}
}
