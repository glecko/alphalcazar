#pragma once

#include "CallbackHandle.hpp"

#include <functional>
#include <algorithm>

namespace Alphalcazar::Utils {
	template <class T>
	class CallbackHandler;

	/*!
	 * \brief Generic callback handler utility class.
	 * Use the Register() method to register new callbacks to it, and the Invoke() method to execute them.
	 */
	template <typename T, typename...Args>
	class CallbackHandler<T(Args...)> {
	public:
		using CallbackType = std::function<T(Args...)>;

		CallbackHandler() {}

		/// Registers a new callback and returns its handle.
		[[nodiscard]] CallbackHandle Register(CallbackType callback) {
			auto handle = ++mNextHandle;
			mCallbacks[handle] = callback;
			return handle;
		}

		/// Removes a previously registered callback given its handle (returned from Register())
		void Remove(CallbackHandle handle) {
			mCallbacks.erase(handle);
		}

		/// Invokes all callbacks registered to this callback handler.
		void Invoke(Args... args) const {
			for (const auto& it : mCallbacks) {
				it.second(args...);
			}
		}
	private:
		std::unordered_map<CallbackHandle, CallbackType> mCallbacks;
		CallbackHandle mNextHandle = 1;
	};

}
