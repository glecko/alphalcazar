#pragma once

#include <cstdint>

namespace Alphalcazar::Strategy::MinMax {
	using Depth = std::uint8_t;
	using Score = std::int32_t;

	enum class EvaluationType {
		NONE = 0,
		EXACT,
		ALPHA_CUTOFF,
		BETA_CUTOFF
	};
}
