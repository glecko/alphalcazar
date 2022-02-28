#pragma once

#include <cstdint>
#include <memory>
#include "aliases.hpp"
#include "util/CallbackHandler.hpp"

namespace Alphalcazar::Game {
	class Board;
	class Player;
	class Strategy;

	struct GameState {
		GameState();
		GameState(const GameState& other);

		/// What turn the game is currently on
		std::uint16_t Turn = 0;
		/// The player that plays the move first this turn (the one that has the "innitiative" token)
		PlayerId PlayerWithInitiative = PlayerId::PLAYER_ONE;
		/// If the first move this turn has already been played
		bool FirstMoveExecuted = false;

		/// Exchange the player wiuth initiative
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
		GameResult Play(const Strategy& firstPlayerStrategy, const Strategy& secondPlayerStrategy);
		/// Plays out a single round of the game, given the player strategies to execute
		GameResult PlayTurn(const Strategy& firstPlayerStrategy, const Strategy& secondPlayerStrategy);

		/// Returns a player instance given its ID
		Player* GetPlayer(PlayerId player) const;
		/// Returns the player that needs to play next
		Player* GetActivePlayer() const;
		/// Returns the player has the corresponding initiative this turn (goes first or goes second, depending on the value of "initiative")
		Player* GetPlayerByInitiative(bool initiative) const;

		using PlayerMovesExecutedCallback = void();
		/// Callback executed after both players have placed their piece on the board, and before board movements are executed
		Utils::CallbackHandler<PlayerMovesExecutedCallback>& OnPlayerMovesExecuted();
	private:
		/// Checks if the game is currently in a stalemate position
		bool IsStalemate(BoardMovesCount executedMoves) const;
		/// Makes both players play their moves
		void ExecutePlayerMoves(const Strategy& firstPlayerStrategy, const Strategy& secondPlayerStrategy);
		/// Makes a given player play his move
		void ExecutePlayerMove(PlayerId playerId, const Strategy& strategy);
		/// Returns what state the board is currently in.
		GameResult EvaluateGameResult(BoardMovesCount executedMoves);

		Utils::CallbackHandler<PlayerMovesExecutedCallback> mPlayerMovesExecutedCallbacks;

		GameState mState;
		std::unique_ptr<Board> mBoard;
		std::unique_ptr<Player> mPlayerOne;
		std::unique_ptr<Player> mPlayerTwo;
	};
}