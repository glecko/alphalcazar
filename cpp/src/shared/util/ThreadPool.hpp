#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <future>

namespace Alphalcazar::Utils {
	class ThreadPool {
	public:
		ThreadPool();
		~ThreadPool();

		ThreadPool(const ThreadPool& other) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;

        template <typename Func, typename... Args>
        auto Execute(Func&& f, Args&&... args);

	private:
		std::vector<std::thread> mWorkers;
		bool mTerminatePool = false;
		std::mutex mQueueMutex;
		std::condition_variable mCondition;
		std::queue<std::function<void()>> mJobsQueue;
	};

    template <typename Func, typename... Args>
    auto ThreadPool::Execute(Func&& task, Args&&... args) {
        //get return type of the function
        typedef decltype(task(args...)) retType;

        std::unique_lock<std::mutex> lock(mQueueMutex);

        std::shared_ptr<std::promise<retType>> promisePtr = std::make_shared();
        std::future<retType> future = promisePtr->get().get_future();

        mJobsQueue.emplace([promisePtr, task, args...] {
            retType result = task(args...);
            promisePtr->set_value(result);
        });

        mCondition.notify_one();

        return std::move(future);
    }
}
