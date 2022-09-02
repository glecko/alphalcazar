#pragma once

namespace Alphalcazar::Game {
	/*!
	 * \brief Whether the \ref Coordinates struct should check for the integrity of its data.
	 *
	 * If false, some assert-like checks in the \ref Coordinates class will be skipped that could assert
	 * invalid data returned from its function calls.
	 */
#if defined _DEBUG
	constexpr bool c_CoordinatesIntegrityChecks = true;
#else
	constexpr bool c_CoordinatesIntegrityChecks = false;
#endif

	/*!
	 * \brief Whether the integrity of the piece placement information in the \ref Board class should be checked.
	 *
	 * If false, all information in the \ref Board class will be considered to be valid, and some null pointer checks will
	 * be skipped. This is meant to avoid unnecesary code branching in Release mode, but keep those checks while developing in
	 * Debug mode to quickly discover bugs through meaningful error logs rather than crashes.
	 */
#if defined _DEBUG
	constexpr bool c_BoardPiecePlacementIntegrityChecks = true;
#else
	constexpr bool c_BoardPiecePlacementIntegrityChecks = false;
#endif
}
