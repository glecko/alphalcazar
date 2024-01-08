#pragma once

#include "game/aliases.hpp"
#include "game/parameters.hpp"
#include "minmax/minmax_aliases.hpp"
#include <util/StaticVector.hpp>
#include <game/PlacementMove.hpp>

namespace Alphalcazar::Game {
	class Board;
}

namespace Alphalcazar::Strategy::MinMax {
	/// A small extension of the \ref PlacementMove data structure adding a heuristic score for quicker sorting
	struct ScoredPlacementMove final : public Game::PlacementMove {
		ScoredPlacementMove() = default;
		ScoredPlacementMove(const PlacementMove& placementMove) 
			: PlacementMove{ placementMove }
		{}

		/// The heuristic score calculated for this placement move
		Score Score = 0;
	};

	/*!
	 * \brief Calculates the symmetry axes of the board of a given board.
	 *
	 * A board can only have x-axis or y-axis symmetry, as pieces can only move along these axis. Pieces cannot
	 * have directions like south-west or north-east, making diagonal symmetries impossible.
	 *
	 * \returns A pair with 2 boolean values, the first being the x-axis symmetry of the board and the second the y-axis symmetry.
	 */
	std::pair<bool, bool> GetBoardSymmetries(const Game::Board& board);

	/*!
	 * \brief Filters a list of legal movements based on board symmetries and sorts them by their heuristic score.
	 * 
	 * The filtering is done with the criteria that if several moves would cause the resulting board states to be identical with some symmetry
	 * (ex. x-axis symmetry) between, only one of those moves (the first one) is kept in the list.
	 * 
	 * Alpha-beta-pruning algorithms are most efficient when the best moves are explored first. Since this is
	 * not possible until the branch is actually explore, we use this heuristic to try to approximate it.
	 *
	 * \param playerId The ID of the player who's turn it is to play.
	 * \param legalMoves The list of legal moves to filter and sort.
	 * \param board The board of the game for which the legal movements are valid.
	 * 
	 * \returns A filtered and sorted list of \ref ScoredPlacementMove.
	 */
	Utils::StaticVector<ScoredPlacementMove, Game::c_MaxLegalMovesCount> SortAndFilterMovements(Game::PlayerId playerId, const Utils::StaticVector<Game::PlacementMove, Game::c_MaxLegalMovesCount>& legalMoves, const Game::Board& board);
}

// We define the specializations of parse & format to make \ref ScoredPlacementMove formattable. Based on: https://fmt.dev/latest/api.html#format-api
template <> struct fmt::formatter<Alphalcazar::Strategy::MinMax::ScoredPlacementMove> {
	constexpr auto parse(const format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const Alphalcazar::Strategy::MinMax::ScoredPlacementMove& move, FormatContext& ctx) -> decltype(ctx.out()) {
		return format_to(ctx.out(), "Piece {} -> {}", move.PieceType, move.Coordinates);
	}
};
