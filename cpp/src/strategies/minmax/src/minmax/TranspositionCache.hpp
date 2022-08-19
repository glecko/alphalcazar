#pragma once

#include <game/Game.hpp>
#include <game/aliases.hpp>
#include "minmax/minmax_aliases.hpp"

#include <optional>
#include <shared_mutex>
#include <unordered_map>

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief
	 */
	class TranspositionCache final {
	public:
		TranspositionCache();
		~TranspositionCache();

		std::optional<Score> GetScore(Game::PlayerId evaluatedPlayer, const Game::Game& game, Depth depth, Score alpha, Score beta) const;
		void StoreScore(const Game::Game& game, EvaluationType type, Depth depth, Score score);
	private:
		struct CacheData {
			Score ActivePlayerScore;
			EvaluationType Type = EvaluationType::NONE;
			Depth Depth = 0;
		};

		bool ValidCacheValue(Game::PlayerId evaluatedPlayer, Game::PlayerId activePlayer, const CacheData& cacheData, Score alpha, Score beta) const;

		mutable std::shared_mutex mCacheMutex;
		std::unordered_map<Game::Game, CacheData> mCache;
	};
}
