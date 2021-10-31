#include "Coordinates.hpp"
#include "parameters.hpp"

namespace Alphalcazar::Game {
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
		if (x == coord.x && y == coord.y) {
			return true;
		}
		return false;
	}

	bool Coordinates::IsPerimeter() const {
		return x == 0 || x == c_PlayAreaSize - 1 || y == 0 || y == c_PlayAreaSize - 1;
	}

	bool Coordinates::IsCorner() const {
		return (x == 0 || x == c_PlayAreaSize - 1) && (y == 0 || y == c_PlayAreaSize - 1);
	}

	Coordinates Coordinates::GetCoordinateInDirection(Direction direction, Coordinate distance) const {
		Coordinates& offset = c_DirectionOffsets.at(direction);
		return Coordinates(x + offset.x * distance, y + offset.y * distance);
	}
}
