#include "util/ThreadPool.hpp"

namespace Alphalcazar::Utils {
    ThreadPool::ThreadPool(size_t threadCount) {
        mThreads.reserve(threadCount);
        for (size_t i = 0; i < threadCount; i++) {
            mThreads.emplace_back(
                std::thread([this]() {
                    std::unique_lock<std::mutex> queueLock{ mTaskMutex, std::defer_lock };

                    while (true) {
                        queueLock.lock();
                        mTasksConditionVariable.wait(queueLock, [this]() -> bool {
                            return !mTasks.empty() || mThreadsStopping;
                        });

                        if (mThreadsStopping && mTasks.empty()) return;

                        std::unique_ptr<TaskContainerBase> task = std::move(mTasks.front());

                        mTasks.pop();
                        queueLock.unlock();

                        task->Execute();
                    }
                })
            );
        }
    }

    ThreadPool::~ThreadPool() {
        mThreadsStopping = true;
        mTasksConditionVariable.notify_all();

        for (std::thread& thread : mThreads) {
            thread.join();
        }
    }
}
