#pragma once

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace Alphalcazar::Utils {
	template<typename... Args>
	void LogInfo(fmt::format_string<Args...> fmt, Args &&...args) {
		spdlog::info(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void LogDebug(fmt::format_string<Args...> fmt, Args &&...args) {
		spdlog::debug(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void LogWarn(fmt::format_string<Args...> fmt, Args &&...args) {
		spdlog::warn(fmt, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void LogError(fmt::format_string<Args...> fmt, Args &&...args) {
		spdlog::error(fmt, std::forward<Args>(args)...);
	}
}
