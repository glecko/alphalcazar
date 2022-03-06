#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "aliases.hpp"
#include "util/CallbackHandler.hpp"

namespace Alphalcazar::Game {
	class Board;
	class Player;
	class Strategy;
	class Piece;
	struct PlacementMove;

	struct GameState {
		GameState();
		GameState(const GameState& other);

		/// What turn the game is currently on
		std::uint16_t Turn = 0;
		/// The player that plays the move first this turn (the one that has the "innitiative" token)
		PlayerId PlayerWithInitiative = PlayerId::PLAYER_ONE;
		/// If the first move this turn has already been played
		bool FirstMoveExecuted = false;

		/// Exchange the player with initiative
		void SwapPlayerWithInitiative();
	};


	/*!
	 * \brief Manages an instance of the game being played.
	 * Handles the structure of a game turn and takes care of which player needs to play next.
	 */
	class Game {
	public:
		Game();
		Game(const Game& other);
		~Game();

		/// Play the game and return the result
		GameResult Play(Strategy& firstPlayerStrategy, Strategy& secondPlayerStrategy);
		/// Plays out a single round of the game, given the player strategies to execute
		GameResult PlayTurn(Strategy& firstPlayerStrategy, Strategy& secondPlayerStrategy);
		/*!
		 * \brief Plays out the next step of a turn
		 * 
		 * \param move The placement move of the active player.
		 * 
		 * \returns The game result after the end of the turn if this function executed
		 *          the second placement move of the turn, or GameState::NONE otherwise.
		 */
		GameResult PlayNextPlacementMove(const PlacementMove& move);

		/// Returns a player instance given its ID
		Player* GetPlayer(PlayerId player) const;
		/// Returns the player that needs to play next
		Player* GetActivePlayer() const;
		/// Returns the player has the corresponding initiative this turn (goes first or goes second, depending on the value of "initiative")
		Player* GetPlayerByInitiative(bool initiative) const;

		/// Returns the current state of the game
		GameState& GetState();
		const GameState& GetState() const;

		/// Returns the board of game
		Board& GetBoard();
		const Board& GetBoard() const;

		/// Returns a list of all pieces that exist in the current game (of both players)
		std::vector<Piece*> GetAllPieces() const;

		/*!
		 * \brief Returns a list of legal placement moves for the current player.
		 * 
		 * A legal placement moves is defined as any combination of free perimeter tile and 
		 * available piece (in hand) the player has available.
		 */
		std::vector<PlacementMove> GetLegalMoves(PlayerId player) const;

		using PlayerMovesExecutedCallback = void();
		/// Callback executed after both players have placed their piece on the board, and before board movements are executed
		Utils::CallbackHandler<PlayerMovesExecutedCallback>& OnPlayerMovesExecuted();
	private:
		/// Checks if the game is currently in a stalemate position
		bool IsStalemate(BoardMovesCount executedMoves) const;
		/// Makes both players play their moves
		void ExecutePlayerMoves(Strategy& firstPlayerStrategy, Strategy& secondPlayerStrategy);
		/// Makes a given player play his move
		void ExecutePlayerMove(PlayerId playerId, Strategy& strategy);
		/// Executes a single placement move on the board
		void ExecutePlacementMove(PlayerId playerId, const PlacementMove& move);
		/*!
		 * \brief Executes the turn end operations after both players have played their placement move
		 * 
		 * \returns The game result at the end of this turn.
		 */
		GameResult EvaluateTurnEndPhase();
		/// Returns what state the board is currently in.
		GameResult EvaluateGameResult(BoardMovesCount executedMoves);

		Utils::CallbackHandler<PlayerMovesExecutedCallback> mPlayerMovesExecutedCallbacks;

		GameState mState;
		std::unique_ptr<Board> mBoard;
		std::unique_ptr<Player> mPlayerOne;
		std::unique_ptr<Player> mPlayerTwo;
	};
}
