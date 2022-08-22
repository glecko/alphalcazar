#pragma once

#include <game/Game.hpp>
#include <game/aliases.hpp>
#include "minmax/minmax_aliases.hpp"

#include <optional>
#include <shared_mutex>
#include <unordered_map>

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
