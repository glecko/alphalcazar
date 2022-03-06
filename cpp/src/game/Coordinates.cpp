#include "Coordinates.hpp"
#include "parameters.hpp"

namespace Alphalcazar::Game {
	static constexpr Coordinate c_InvalidCoordinate = std::numeric_limits<Coordinate>::max();
	/// The coordinate offset each direction represents
	static std::unordered_map<Direction, Coordinates> c_DirectionOffsets = {
		{ Direction::NORTH,  { 0, 1 } },
		{ Direction::SOUTH, { 0, -1 } },
		{ Direction::EAST, { 1, 0 } },
		{ Direction::WEST, { -1, 0 } },
		{ Direction::SOUTH_EAST, { 1, -1 } },
		{ Direction::SOUTH_WEST, { -1, -1 } },
		{ Direction::NORTH_EAST, { 1, 1 } },
		{ Direction::NORTH_WEST, { -1, 1 } },
	};

	Coordinates::Coordinates(Coordinate x, Coordinate y)
		: x(x)
		, y(y)
	{}

	Coordinates::~Coordinates() {}

	bool Coordinates::operator==(const Coordinates& coord) const {
		return x == coord.x && y == coord.y;
	}

	bool Coordinates::IsPerimeter() const {
		return x == 0 || x == c_PlayAreaSize - 1 || y == 0 || y == c_PlayAreaSize - 1;
	}

	bool Coordinates::IsCorner() const {
		return (x == 0 || x == c_PlayAreaSize - 1) && (y == 0 || y == c_PlayAreaSize - 1);
	}

	Direction Coordinates::GetLegalPlacementDirection() const {
		if (IsPerimeter()) {
			if (x == 0) {
				return Direction::EAST;
			} else if (x == c_PlayAreaSize - 1) {
				return Direction::WEST;
			} else if (y == 0) {
				return Direction::NORTH;
			} else if (y == c_PlayAreaSize - 1) {
				return Direction::SOUTH;
			}
		}
		// Pieces cannot be placed on non-perimeter tiles.
		return Direction::NONE;
	}

	Coordinates Coordinates::GetCoordinateInDirection(Direction direction, Coordinate distance) const {
		if (!Valid()) {
			throw "Tried to get coordinate offset from an invalid coordinate";
		}
		if (direction == Direction::NONE) {
			throw "Tried getting a coordinate offset into an invalid direction";
		}

		Coordinates& offset = c_DirectionOffsets.at(direction);
		return Coordinates(x + offset.x * distance, y + offset.y * distance);
	}

	bool Coordinates::Valid() const {
		return x != c_InvalidCoordinate && y != c_InvalidCoordinate;
	}

	Coordinates Coordinates::Invalid() {
		return Coordinates { c_InvalidCoordinate, c_InvalidCoordinate };
	}
}
