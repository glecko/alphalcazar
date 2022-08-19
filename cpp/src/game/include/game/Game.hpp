#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "aliases.hpp"
#include "util/CallbackHandler.hpp"
#include "Board.hpp"

namespace Alphalcazar::Game {
	class Strategy;
	class Piece;
	struct PlacementMove;

	/*!
	 * \brief Contains the state of a given game.
	 */
	struct GameState {
		/// The player that plays the move first this turn (the one that has the "innitiative" token)
		PlayerId PlayerWithInitiative = PlayerId::PLAYER_ONE;
		/// If the first move this turn has already been played
		bool FirstMoveExecuted = false;
		/// What turn the game is currently on
		std::uint16_t Turn = 0;
	};


	/*!
	 * \brief Manages an instance of the game being played.
	 * Handles the structure of a game turn and takes care of which player needs to play next.
	 */
	class Game {
	public:
		Game();
		~Game();

		bool operator==(const Game& other) const;

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

		/// Returns the ID of the player that needs to play next
		PlayerId GetActivePlayer() const;
		/// Returns the ID of the player has the corresponding initiative this turn (goes first or goes second, depending on the value of \param initiative)
		PlayerId GetPlayerByInitiative(bool initiative) const;

		/// Returns the current state of the game
		GameState& GetState();
		const GameState& GetState() const;

		/// Returns the board of game
		Board& GetBoard();
		const Board& GetBoard() const;

		/*!
		 * \brief Returns a list of legal placement moves for the current player.
		 *
		 * A legal placement moves is defined as any combination of free perimeter tile and
		 * available piece (in hand) the player has available.
		 */
		std::vector<PlacementMove> GetLegalMoves(PlayerId player) const;

		/// Returns a list of all pieces that the specified player has in hand (are not placed on the board)
		std::vector<Piece> GetPiecesInHand(PlayerId player) const;
	private:
		/// Exchange the player with initiative
		void SwapPlayerWithInitiative();
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
		/*!
		 * \brief Returns what state the game is currently in.
		 *
		 * \param executedMoves The amount of moves executed on the last turn.
		 */
		GameResult EvaluateGameResult(BoardMovesCount executedMoves);

		/// The board on which the game is being played
		Board mBoard;
		/// The state of the game. See \ref GameState for more info
		GameState mState;
	};
}

namespace std {
	/*
	* The \ref Game class implements a hashing function to be able to be used as a key
	* for unordered maps.
	*/
	template <>
	struct hash<Alphalcazar::Game::Game> {
		std::size_t operator()(const Alphalcazar::Game::Game& game) const noexcept {
			auto activePlayerHash = hash<Alphalcazar::Game::PlayerId>()(game.GetActivePlayer());
			auto boardHash = hash<Alphalcazar::Game::Board>()(game.GetBoard());
			return ((boardHash ^ (activePlayerHash << 1)) >> 1);
		}
	};
}
