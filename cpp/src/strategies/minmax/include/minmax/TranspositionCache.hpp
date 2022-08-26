#pragma once

#include <game/Game.hpp>
#include <game/aliases.hpp>
#include "minmax/minmax_aliases.hpp"
#include <util/hash.hpp>

#include <optional>
#include <shared_mutex>
#include <unordered_map>

namespace std {
	/*
	 * A hashing function for the \ref Board class to be able to hash the \ref Game class.
	 */
	template <>
	struct hash<Alphalcazar::Game::Board> {
		std::size_t operator()(const Alphalcazar::Game::Board& board) const noexcept {
			// We hash the board by looping through all pieces on the board, creating a hash for each piece/coordinate pair
			// and combining all pair hashes together
			std::size_t result = 0;
			for (auto& [coordinate, piece] : board.GetPieces()) {
				/*
				 * We only care about the first 3 bits of the piece type as it currently will never exceed 5 (0b101)
				 * and the direction, which will never exceed 4 (0b100). For the owner, we only care about the first 2 bits
				 * as it will not exceed the value 2 (0b10).
				 *
				 * Therefore, we can concatenate all the information of a piece in the following 8-bit integer by shifting the bits of their 3 properties.
				 */
				std::uint8_t pieceBitRepresentation = static_cast<std::uint8_t>(piece.GetType()) << 5 & static_cast<std::uint8_t>(piece.GetMovementDirection()) << 2 & static_cast<std::uint8_t>(piece.GetOwner());

				// We can then concatenate the first 3 bits of each piece's coordinate (which will never exceed 4, 0b100)
				// into a 16-bit integer that will have 14 bits of information and 2 spare bits on the left
				std::uint16_t pieceInfoBitRepresentation = coordinate.x << 8 & coordinate.y << 11 & pieceBitRepresentation;
				result = Alphalcazar::Utils::combineHash(result, pieceInfoBitRepresentation);
			}
			return result;
		}
	};

	/*
	 * A hashing function for the \ref Game class to be able to use it as a key for the cache's unordered map.
	 */
	template <>
	struct hash<Alphalcazar::Game::Game> {
		std::size_t operator()(const Alphalcazar::Game::Game& game) const noexcept {
			auto boardHash = hash<Alphalcazar::Game::Board>()(game.GetBoard());
			return Alphalcazar::Utils::combineHash(boardHash, game.GetActivePlayer());
		}
	};
}

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief A thread-safe transposition cache that stores scores of previously evaluated games in memory.
	 */
	class TranspositionCache final {
	public:
		TranspositionCache();
		~TranspositionCache();

		/*!
		 * \brief Returns the cached score for a given game (state and board) for a given player, if this position
		 *        has already been previously evaluated. Returns std::nullopt otherwise.
		 *
		 * \param evaluatedPlayer The ID of the player for which we are evaluating the score.
		 * \param game The game (state and board) for which we want to check if a score has already been evaluated.
		 * \param depth The depth at which we are evaluating the game. A cached score will only be retreieved if it was
		 *              originally evaluated for a depth higher or equal the score we are currently evaluating.
		 * \param alpha The alpha value of the current evaluation.
		 * \param beta The beta value of the current evaluation.
		 */
		std::optional<Score> GetScore(Game::PlayerId evaluatedPlayer, const Game::Game& game, Depth depth, Score alpha, Score beta) const;

		/*!
		 * \brief Stores a score for a given game (state and board) in the transposition cache
		 *
		 * \param game The game (state and board) for which the score has been evaluated.
		 * \param depth The depth for which the score was evaluated.
		 * \param score The score that was evaluated.
		 */
		void StoreScore(const Game::Game& game, EvaluationType type, Depth depth, Score score);

		/*!
		 * \brief Deletes the stored score for the given game (state and board) in the transposition cache,
		 *        if it exists
		 *
		 * \param game The game (state and board) for which we want to delete the stored score
		 */
		void DeleteScore(const Game::Game& game);
	private:
		struct CacheData {
			/// The score evaluated for the active player (who's turn it is to place a piece). The score of the passive player can be obtained by negating this value.
			Score ActivePlayerScore;
			/// The type of evaluation that was used to determine the score (ex. exact or alpha-beta-cutoff)
			EvaluationType Type = EvaluationType::NONE;
			/// The depth for which the score was evaluated
			Depth Depth = 0;
		};

		/*!
		 * \brief Returns whether the matching cache data found for the given position can be used in the given context.
		 *
		 * The reason this function needs to be called is because the cached value might correspond to an alpha-beta-cutoff, in which case it cannot be evaluated
		 * as an exact score. It might be reused if according to the alpha/beta values of the current evaluation the cached value can be used to also determine an
		 * alpha or beta cutoff.
		 *
		 * \param evaluatedPlayer The ID of the player for which we are evaluating the score.
		 * \param activePlayer The ID of the player who's turn it is to play.
		 * \param cacheData The matching cache data found for the current position.
		 * \param alpha The alpha value of the current evaluation.
		 * \param beta The beta value of the current evaluation.
		 */
		bool ValidCacheValue(Game::PlayerId evaluatedPlayer, Game::PlayerId activePlayer, const CacheData& cacheData, Score alpha, Score beta) const;

		mutable std::shared_mutex mCacheMutex;

		/// A dictionary containing the cached data, indexed by the hashed value of the \ref Game object for which it was calculated.
		std::unordered_map<Game::Game, CacheData> mCache;
	};
}
