#include "minmax/BoardEvaluation.hpp"
#include "minmax/config.hpp"

#include <game/Game.hpp>
#include <game/Piece.hpp>
#include <algorithm>
#include <util/Log.hpp>

namespace Alphalcazar::Strategy::MinMax {
	/*!
	 * \brief Returns a score multiplier for the heuristic score of a piece given its position and direction on the board.
	 *
	 * Grants a higher multiplier for pieces that are well positioned (ex. in the center) or have a long expected
	 * lifetime (ex. facing towards the board interior) and a lower multiplier for pieces that are badly positioned or
	 * with a short expected lifetime (ex. about to exit the board).
	 */
	float GetPieceScoreMultiplier(const Game::Coordinates& coordinates, Game::Direction direction) {
		if (coordinates.IsCenter()) {
			return c_CenterPieceMultiplier;
		}
		
		if (coordinates.IsBoardCorner()) {
			// In the board corners, a piece can only have recently entered the board
			// or be about to leave it
			const auto pieceTargetCoordinate = coordinates.GetCoordinateInDirection(direction, 1);
			if (pieceTargetCoordinate.IsPerimeter()) {
				return c_PieceAboutToExitMultiplier;
			}
			return c_FreshCornerPieceMultiplier;
		}
		
		// The piece is on one of the center lanes, but not in the center tile
		const auto pieceTargetCoordinate = coordinates.GetCoordinateInDirection(direction, 1);
		if (pieceTargetCoordinate.IsPerimeter()) {
			// The piece is about to exit the board
			return c_PieceAboutToExitMultiplier;
		}
		
		if (pieceTargetCoordinate.IsCenter()) {
			// The piece just entered the center lane and wants to move to the center
			return c_FreshCenterLanePieceMultiplier;
		}
		return 1.f;
	}

	Score GameResultToScore(Game::PlayerId playerId, Game::GameResult result) {
		switch (result) {
		case Game::GameResult::PLAYER_ONE_WINS:
			return playerId == Game::PlayerId::PLAYER_ONE ? c_WinConditionScore : -c_WinConditionScore;
		case Game::GameResult::PLAYER_TWO_WINS:
			return playerId == Game::PlayerId::PLAYER_TWO ? c_WinConditionScore : -c_WinConditionScore;
		case Game::GameResult::DRAW:
			return 0;
		default:
			Utils::LogError("Tried to convert invalid game result to score");
		}
		return 0;
	}

	Score EvaluateBoard(Game::PlayerId playerId, const Game::Game& game) {
		Score totalScore = 0;
		auto [pieces, piecesCount] = game.GetBoard().GetPieces();
		for (std::size_t i = 0; i < piecesCount; ++i) {
			auto& [coordinates, piece] = pieces[i];
			if (!coordinates.IsPerimeter()) {
				const Game::Direction direction = piece.GetMovementDirection();
				const float pieceScoreMultiplier = GetPieceScoreMultiplier(coordinates, direction);

				// The piece on board score array stores all piece types in order, meaning that
				// the score of a certain piece type will be located at index (type - 1)
				const Score pieceOnBoardScore = c_PieceOnBoardScores[piece.GetType() - 1];
				const Score pieceScore = static_cast<Score>(pieceOnBoardScore * pieceScoreMultiplier);

				// Add the score if the piece belongs to the player for which we are evaluating the score
				// or subtract it if it belongs to the opponent
				if (piece.GetOwner() == playerId) {
					totalScore += pieceScore;
				} else {
					totalScore -= pieceScore;
				}
			}
		}
		return totalScore;
	}

	Score GetDepthAdjustedScore(Score score, Depth depth) {
		const Score depthPenalty = depth * c_DepthScorePenalty;
		const Score penalty = std::min(depthPenalty, std::abs(score));
		if (score > 0) {
			score -= penalty;
		} else {
			score += penalty;
		}
		return score;
	}
}
