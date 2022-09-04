#include <gtest/gtest.h>

#include <util/StaticVector.hpp>
#include <string>
#include <cstdint>

namespace Alphalcazar::Utils {
	TEST(StaticVector, InsertElements) {
		StaticVector<int, 3> vector;
		EXPECT_EQ(vector.size(), 0);
		vector.insert(1);
		EXPECT_EQ(vector.size(), 1);
		vector.insert(2);
		EXPECT_EQ(vector.size(), 2);
	}

	TEST(StaticVector, IteratorValues) {
		StaticVector<std::string, 5> vector;
		vector.insert("Hello!");
		vector.insert("...");
		vector.insert("Bye!");
		EXPECT_EQ(*vector.begin(), "Hello!");
		EXPECT_EQ(*(vector.end() - 1), "Bye!");

		// An extra insert should affect the end, but not the beginning
		vector.insert("Bye bye!");
		EXPECT_EQ(*vector.begin(), "Hello!");
		EXPECT_EQ(*(vector.end() - 1), "Bye bye!");
	}

	TEST(StaticVector, ValueAccess) {
		StaticVector<float, 8> vector;
		vector.insert(1.f);
		vector.insert(2.f);
		const StaticVector<float, 8> vectorCopy = vector;
		EXPECT_EQ(vectorCopy[0], 1.f);
		EXPECT_EQ(vectorCopy[1], 2.f);
	}

	TEST(ReversedStaticVector, InsertElements) {
		ReversedStaticVector<std::uint32_t, 4> vector;
		EXPECT_EQ(vector.size(), 0);
		vector.insert(1);
		EXPECT_EQ(vector.size(), 1);
		vector.insert(2);
		EXPECT_EQ(vector.size(), 2);
	}

	TEST(ReversedStaticVector, IteratorValues) {
		struct TestStruct {
			float x;
			int y;
		};

		ReversedStaticVector<TestStruct, 10> vector;
		vector.insert({1.f, 1});
		vector.insert({2.f, 2});
		vector.insert({3.f, 3});
		EXPECT_EQ(vector.begin()->y, 3);
		EXPECT_EQ((vector.end() - 1)->y, 1);

		// An extra insert should affect the beginning, but not the end
		vector.insert({ 4.f, 4 });
		EXPECT_EQ(vector.begin()->y, 4);
		EXPECT_EQ((vector.end() - 1)->y, 1);
	}

	TEST(ReversedStaticVector, ValueAccess) {
		ReversedStaticVector<float, 8> vector;
		vector.insert(1.f);
		vector.insert(2.f);

		const ReversedStaticVector<float, 8> vectorCopy = vector;
		EXPECT_EQ(vectorCopy[7], 1.f);
		EXPECT_EQ(vectorCopy[6], 2.f);
	}
}
