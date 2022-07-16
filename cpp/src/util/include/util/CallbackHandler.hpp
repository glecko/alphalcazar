#pragma once

#include "CallbackHandle.hpp"

#include <functional>
#include <algorithm>

namespace Alphalcazar::Utils {
	template <class T>
	class CallbackHandler;

	/*!
	 * \brief Generic callback handler utility class.
	 *
	 * Use the \ref Register method to register new callbacks to it, and the \ref Invoke method to execute them.
	 */
	template <typename T, typename...Args>
	class CallbackHandler<T(Args...)> {
	public:
		using CallbackType = std::function<T(Args...)>;

		CallbackHandler() {}

		/// Registers a new callback and returns its handle.
		[[nodiscard]] CallbackHandle Register(CallbackType callback) {
			auto handle = ++mHandle;
			mCallbacks.emplace_back(handle, callback);
			return handle;
		}

		/// Removes a previously registered callback given its handle returned from \ref Register
		void Remove(CallbackHandle handle) {
			mCallbacks.erase(
				std::remove_if(
					mCallbacks.begin(),
					mCallbacks.end(),
					[handle](const auto& callbackInfo) {
						return callbackInfo.Handle == handle;
					}
				),
				mCallbacks.end()
			);
		}

		/// Invokes all callbacks registered to this callback handler.
		void Invoke(Args... args) const {
			for (const auto& callbackInfo : mCallbacks) {
				callbackInfo.Callback(args...);
			}
		}
	private:
		struct CallbackInfo {
			CallbackInfo(CallbackHandle handle, CallbackType callback) : Handle{ handle }, Callback{ callback } {}

			CallbackHandle Handle;
			CallbackType Callback;
		};

		std::vector<CallbackInfo> mCallbacks;
		CallbackHandle mHandle = 1;
	};

}
