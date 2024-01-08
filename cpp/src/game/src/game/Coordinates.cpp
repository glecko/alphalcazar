#include "game/Coordinates.hpp"

#include <util/Log.hpp>
#include "safety_checks.hpp"

namespace Alphalcazar::Game {

	// Small macro to define the offsets of \ref c_DirectionOffsets. The reason for this is that in order for the
	// array to be a valid constexpr, we must cast the int literals to Coordinate (std::int8_t), as implicit type
	// conversions are not valid for constexprs, and C++17 does not have std::int8_t literals.
	#define DIR_OFFSETS(x, y) { static_cast<Coordinate>(x), static_cast<Coordinate>(y) }

	/*!
	 * \brief The x/y offsets each direction represents.
	 *
	 * \note Each direction offset should be located at the index of its corresponding \ref Direction value. If
	 *       the Direction enum is re-ordered, so should this array.
	 */
	constexpr std::array<std::pair<Coordinate, Coordinate>, static_cast<std::size_t>(Direction::SIZE)> c_DirectionOffsets = {{
		DIR_OFFSETS(0, 0), // NONE
		DIR_OFFSETS(0, 1), // NORTH
		DIR_OFFSETS(0, -1), // SOUTH
		DIR_OFFSETS(1, 0), // EAST
		DIR_OFFSETS(-1, 0), // WEST
		DIR_OFFSETS(1, -1), // SOUTH_EAST
		DIR_OFFSETS(-1, -1), // SOUTH_WEST
		DIR_OFFSETS(1, 1), // NORTH_EAST
		DIR_OFFSETS(-1, 1), // NORTH_WEST
	}};

	Direction Coordinates::GetLegalPlacementDirection() const {
		if (c_CoordinatesIntegrityChecks) {
			if (!IsPerimeter()) {
				// Pieces cannot be placed on non-perimeter tiles.
				return Direction::NONE;
			}
		}
		if (x == 0) {
			return Direction::EAST;
		} else if (x == c_PlayAreaSize - 1) {
			return Direction::WEST;
		} else if (y == 0) {
			return Direction::NORTH;
		} else {
			return Direction::SOUTH;
		}
	}

	Coordinates Coordinates::GetCoordinateInDirection(Direction direction, Coordinate distance) const {
		if constexpr (c_CoordinatesIntegrityChecks) {
			if (!Valid()) {
				Utils::LogError("Tried to get coordinate offset from an invalid coordinate");
				return Invalid();
			}
			if (direction == Direction::NONE) {
				Utils::LogError("Tried getting a coordinate offset into an invalid direction");
				return Invalid();
			}
		}

		const std::size_t directionOffset = static_cast<std::size_t>(direction);
		const auto& offset = c_DirectionOffsets[directionOffset];
		const Coordinate xOffset = offset.first * distance;
		const Coordinate yOffset = offset.second * distance;
		return Coordinates{ x + xOffset, y + yOffset };
	}

	Coordinates Coordinates::Invalid() {
		return Coordinates { c_InvalidCoordinate, c_InvalidCoordinate };
	}
}
