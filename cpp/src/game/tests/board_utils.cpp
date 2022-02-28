#include <gtest/gtest.h>

#include "board_utils.hpp"

namespace Alphalcazar::Game {
	TEST(BoardUtils, RowIterationDirections) {
		constexpr auto directions = GetAllRowIterationDirections();
		EXPECT_EQ(directions.size(), c_RowIterationDirectionsCount);
		for (auto& [x, y, direction, distance] : directions) {
			EXPECT_NE(direction, Direction::NONE);
			EXPECT_EQ(distance, c_BoardSize);
		}
	}
}
