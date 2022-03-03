#pragma once

#include "Coordinates.hpp"
#include "aliases.hpp"

namespace Alphalcazar::Game {
	/*!
  	 * \brief Data structure with the minimnal information needed to uniquely represent a placement move.
	 * 
	 * A placement move is defined as the action a player takes at the start of each turn,
	 * where they take a piece they have in hand and place it on the perimeter of the board, facing
	 * inwards.
	 */
	struct PlacementMove {
		Coordinates Coordinates;
		PieceType PieceType;
		PlayerId PlayerId;
	};
}
