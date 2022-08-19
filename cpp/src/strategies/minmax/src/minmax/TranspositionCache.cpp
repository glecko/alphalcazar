#include "TranspositionCache.hpp"

namespace Alphalcazar::Strategy::MinMax {
	TranspositionCache::TranspositionCache() {}
	TranspositionCache::~TranspositionCache() {}

	std::optional<Score> TranspositionCache::GetScore(Game::PlayerId evaluatedPlayer, const Game::Game& game, Depth depth, Score alpha, Score beta) const {
        std::shared_lock lock{ mCacheMutex };
        auto cacheMatch = mCache.find(game);
        if (cacheMatch != mCache.end()) {
            auto& cacheData = cacheMatch->second;
            Game::PlayerId activePlayer = game.GetActivePlayer();
            if (cacheData.Depth >= depth && ValidCacheValue(evaluatedPlayer, activePlayer, cacheData, alpha, beta)) {
                Score cachedScore = cacheData.ActivePlayerScore;
                if (evaluatedPlayer == activePlayer) {
                    cachedScore = -cachedScore;
                }
                return cachedScore;
            }
        }
        return std::nullopt;
	}

	void TranspositionCache::StoreScore(const Game::Game& game, EvaluationType type, Depth depth, Score score) {
        if (depth <= 2) {
            return;
        }
        std::unique_lock lock{ mCacheMutex };
        auto& cacheData = mCache[game];
        // We only overwrite the value in cache if our depth value is higher than the max explored depth
        // already present in the cache.
        // In case of equal depths, we only overwrite alpha / beta cutoff evaluations with exact evaluations
        if (depth > cacheData.Depth || (depth == cacheData.Depth && type == EvaluationType::EXACT && cacheData.Type != EvaluationType::EXACT)) {
            cacheData.Depth = depth;
            cacheData.Type = type;
            cacheData.ActivePlayerScore = score;
        }
	}

	bool TranspositionCache::ValidCacheValue(Game::PlayerId evaluatedPlayer, Game::PlayerId activePlayer, const CacheData& cacheData, Score alpha, Score beta) const {
        Score cachedScore = cacheData.ActivePlayerScore;
        if (evaluatedPlayer != activePlayer) {
            cachedScore = -cachedScore;
        }
        switch (cacheData.Type) {
        case EvaluationType::EXACT:
            return true;
        case EvaluationType::ALPHA_CUTOFF:
            // We know that the node was not evaluated completely because 1 of the nodes had a value lower than alpha
            // Which means the "real" value of the node is "score" or lower
            // We can use this value if we are under similar circumstances (score is lower than our alpha)
            return cachedScore < alpha;
        case EvaluationType::BETA_CUTOFF:
            // We know that the node was not evaluated completely because 1 of the nodes had a value higher than beta
            // Which means the "real" value of the node is "score" or higher
            // We can use this value if we are under similar circumstances(score is higher than our beta)
            return cachedScore > beta;
        }
        return false;
	}
}
