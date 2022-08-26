#pragma once

#include <cstddef>

namespace Alphalcazar::Utils {
	constexpr std::size_t c_HashBitCombiner32Bits = 0x9e3779b9;
	constexpr std::size_t c_HashBitCombiner64Bits = 0x9e3779b97f4a7c15;

	/*!
	 * \brief A number with size std::size_t (64 or 32 bits) with half of its bits set to 0 and half of its bit set to 1.
	 *
	 * Obtained as the result of floor((phi - 1) * 2^N), with phi being the golden ratio and N the number of bits needed to represent SIZE_MAX.
	 */
#if (SIZE_MAX == 0xFFFFFFFFFFFFFFFF)
	constexpr std::size_t c_HashBitCombiner = c_HashBitCombiner64Bits;
#else
	constexpr std::size_t c_HashBitCombiner = c_HashBitCombiner32Bits;
#endif

	/*!
	 * \brief Creates a combined hash from an existing hash and a hashable value.
	 *
	 * The method for combining the hashes is obtained from the boost::hash_combine implementation:
	 * https://www.boost.org/doc/libs/1_55_0/doc/html/hash/combine.html
	 *
	 * \param hash The existing hash
	 * \param value The value (must be a hashable type T) to combine with the hash.
	 */
	template<typename T>
	std::size_t combineHash(std::size_t hash, T value) {
		// We select the hash bit combiner depending on the size of std::size_t on this machine.
		hash ^= std::hash<T>()(value) + c_HashBitCombiner + (hash << 6) + (hash >> 2);
		return hash;
	}
}
