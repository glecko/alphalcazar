#pragma once

#include "Coordinates.hpp"
#include "aliases.hpp"

#include <fmt/format.h>

namespace Alphalcazar::Game {
	/*!
  	 * \brief Data structure with the minimnal information needed to uniquely represent a placement move.
	 * 
	 * A placement move is defined as the action a player takes at the start of each turn,
	 * where they take a piece they have in hand and place it on the perimeter of the board, facing
	 * inwards.
	 */
	struct PlacementMove {
		PlacementMove(const Coordinates& coordinates, PieceType pieceType)
			: Coordinates{ coordinates }
			, PieceType{ pieceType }
		{}

		Coordinates Coordinates;
		PieceType PieceType;

		bool operator==(const PlacementMove& other) const {
			return PieceType == other.PieceType && Coordinates == other.Coordinates;
		}
	};

}

// We define the specializations of parse & format to make \ref PlacementMove formattable. Based on: https://fmt.dev/latest/api.html#format-api
template <> struct fmt::formatter<Alphalcazar::Game::PlacementMove> {
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		return ctx.begin();
	}

    template <typename FormatContext>
    auto format(const Alphalcazar::Game::PlacementMove& move, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "Piece {} -> {}", move.PieceType, move.Coordinates);
    }
};
