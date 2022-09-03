#pragma once

#include <array>
#include <type_traits>
#include <assert.h>

namespace Alphalcazar::Utils {
	/*!
	 * \brief A list of variable size with an upper-bound capacity.
	 *
	 * \note The type T must be default-constructible and the capacity of the static vector must be greater than 0.
	 * 
	 * Usage example:
	 * ```
	 * StaticVector<int, 3> numbersVector;
	 * numbersVector.Insert(2);
	 * ...
	 * auto [numbers, numbersCount] = numbersVector;
	 * for(std::size_t i = 0; i < numbersCount; ++i) {
	 *		int number = numbers[i];
	 *		...
	 * }
	 * ```
	 */
	template<typename T, std::size_t Capacity>
	struct StaticVector {
		static_assert(Capacity > 0, "Capacity of StaticVector must be greater than 0");
		static_assert(std::is_default_constructible_v<T>, "Value type of StaticVector must be default-constructible");
		std::array<T, Capacity> mValues;
		std::size_t mSize = 0;

		typename std::array<T, Capacity>::iterator begin() {
			return mValues.begin();
		}

		typename std::array<T, Capacity>::iterator end() {
			return mValues.begin() + mSize;
		}

		typename std::array<T, Capacity>::const_iterator begin() const {
			return mValues.begin();
		}

		typename std::array<T, Capacity>::const_iterator end() const {
			return mValues.begin() + mSize;
		}

		void insert(const T& value) {
			assert(mSize < Capacity);
			mValues[mSize++] = value;
		}

		std::size_t size() const {
			return mSize;
		}

		std::size_t empty() const {
			return mSize == 0;
		}

		T& operator[](std::size_t index) {
			assert(index < mSize);
			return mValues[index];
		}

		const T& operator[](std::size_t index) const {
			assert(index < mSize);
			return mValues[index];
		}
	};

	/*!
	 * \brief Like \ref StaticVector but elements are added back-to-front instead of front-to-back.
	 *
	 * Usage example:
	 * ```
	 * StaticVector<int, 3> numbersVector;
	 * numbersVector.Insert(2);
	 * ...
	 * auto [numbers, numbersCount] = numbersVector;
	 * for(std::size_t i = numbers.size() - numbersCount; i < numbers.size(); ++i) {
	 *		int number = numbers[i];
	 *		...
	 * }
	 * ```
	 */
	template<typename T, std::size_t Capacity>
	struct ReversedStaticVector {
		static_assert(Capacity > 0, "Capacity of StaticVector must be greater than 0");
		static_assert(std::is_default_constructible_v<T>, "Value type of ReversedStaticVector must be default-constructible");
		std::array<T, Capacity> mValues;
		std::size_t mSize = 0;

		typename std::array<T, Capacity>::iterator begin() {
			return mValues.end() - mSize;
		}

		typename std::array<T, Capacity>::iterator end() {
			return mValues.end();
		}

		typename std::array<T, Capacity>::const_iterator begin() const {
			return mValues.end() - mSize;
		}

		typename std::array<T, Capacity>::const_iterator end() const {
			return mValues.end();
		}

		void insert(const T& value) {
			assert(mSize < Capacity);
			mValues[Capacity - mSize - 1] = value;
			++mSize;
		}

		std::size_t size() const {
			return mSize;
		}

		std::size_t empty() const {
			return mSize == 0;
		}

		T& operator[](std::size_t index) {
			assert(index < Capacity && index > (Capacity - mSize));
			return mValues[index];
		}

		const T& operator[](std::size_t index) const {
			return mValues[index];
		}
	};
}
