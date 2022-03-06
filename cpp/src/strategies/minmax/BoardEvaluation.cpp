#include "BoardEvaluation.hpp"
#include "config.hpp"

#include <Game.hpp>
#include <Piece.hpp>
#include <algorithm>

namespace Alphalcazar::Strategy::MinMax {
	Score GameResultToScore(Game::PlayerId playerId, Game::GameResult result) {
		switch (result) {
		case Game::GameResult::PLAYER_ONE_WINS:
			return playerId == Game::PlayerId::PLAYER_ONE ? c_WinConditionScore : -c_WinConditionScore;
		case Game::GameResult::PLAYER_TWO_WINS:
			return playerId == Game::PlayerId::PLAYER_TWO ? c_WinConditionScore : -c_WinConditionScore;
		case Game::GameResult::DRAW:
			return 0;
		default:
			throw "Tried to convert invalid game result to score";
		}
	}

	Score EvaluateBoard(Game::PlayerId playerId, const Game::Game& game) {
		Score totalScore = 0;
		auto pieces = game.GetAllPieces();
		for (auto* piece : pieces) {
			auto& coordinates = piece->GetCoordinates();
			if (piece->IsInPlay() && !coordinates.IsPerimeter()) {
				auto direction = piece->GetMovementDirection();
				float coordinatesScoreMultiplier = c_CoordinatesScoreMultiplier.at(std::make_pair(coordinates, direction));
				// The piece on board score array stores all piece types in order, meaning that
				// the score of a certain piece type will be located at index (type - 1)
				Score pieceOnBoardScore = c_PieceOnBoardScores[piece->GetType() - 1];
				Score pieceScore = static_cast<Score>(pieceOnBoardScore * coordinatesScoreMultiplier);

				// Add the score if the piece belongs to the player for which we are evaluating the score
				// or subtract it if it belongs to the opponent
				if (piece->GetOwner() == playerId) {
					totalScore += pieceScore;
				} else {
					totalScore -= pieceScore;
				}
			}
			
		}
		return totalScore;
	}

	Score GetDepthAdjustedScore(Score score) {
		Score penalty = std::min(c_DepthScorePenalty, static_cast<Score>(std::abs(score)));
		if (score > 0) {
			score -= penalty;
		} else {
			score += penalty;
		}
		return score;
	}
}
