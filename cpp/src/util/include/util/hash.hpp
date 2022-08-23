#pragma once

#include <cstddef>

namespace Alphalcazar::Utils {
	template<typename T>
	std::size_t combineHash(std::size_t hashValue, T value) {
		hashValue ^= std::hash<T>()(value) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
		return hashValue;
	}
}
