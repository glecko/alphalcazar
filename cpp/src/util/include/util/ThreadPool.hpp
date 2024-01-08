#pragma once

#include <vector>
#include <thread>
#include <future>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <type_traits>

namespace Alphalcazar::Utils {
    /*!
     * \brief Basic implementation of a thread pool.
     * 
     * Runs a fixed amount of worker threads.
     * Queue tasks with Execute() wait on the returned std::future to use it.
     * Tasks are guaranteed be picked up by the worker threads in the order they are queued.
     */
    class ThreadPool {
    public:
        ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
        ~ThreadPool();

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        template <typename F>
        /// Runs a task on the thread pool and returns a future for that task's result
        auto Execute(F function);
    private:
        /// A virtual class to allow storing multiple specialized \ref TaskContainerBase in the same queue
        class TaskContainerBase {
        public:
            virtual ~TaskContainerBase() = default;
            virtual void Execute() = 0;
        };

        template <typename F>
        /// A simple wrapper around a task that allows executing it
        class TaskContainer : public TaskContainerBase {
        public:
            TaskContainer(F&& func) 
                : mFunction(std::forward<F>(func)) 
            {}

            /// Runs the task stored in the container
            void Execute() override {
                mFunction();
            }

        private:
            F mFunction;
        };

        template <typename F>
        static std::unique_ptr<TaskContainerBase> AllocateTaskContainer(F&& function) {
            return std::unique_ptr<TaskContainerBase>(
                new TaskContainer<F>(std::forward<F>(function))
            );
        }

        /// A list of all running worker threads managed by this 
        std::vector<std::thread> mThreads;
        /// The task queue of the thread pool
        std::queue<std::unique_ptr<TaskContainerBase>> mTasks;
        /// A mutex for safe accessing the mTasks member across threads
        std::mutex mTaskMutex;
        /// A condition variable to notify/wake up threads when tasks are available for them to pick up
        std::condition_variable mTasksConditionVariable;
        /// Represents if the threads of this pool are being stopped (ex. due to the pool being destroyed). Will prevent threads from picking up new tasks.
        bool mThreadsStopping = false;
    };

    template <typename F>
    auto ThreadPool::Execute(F function) {
        static_assert(!std::is_function_v<F>, "ThreadPool::Execute function type needs to be callable.");

        std::unique_lock queueLock{ mTaskMutex, std::defer_lock };
        std::packaged_task<std::invoke_result_t<F>()> packagedTask{ std::bind(function) };
        std::future<std::invoke_result_t<F>> future = packagedTask.get_future();

        queueLock.lock();
        mTasks.emplace(AllocateTaskContainer(std::move(packagedTask)));
        queueLock.unlock();

        mTasksConditionVariable.notify_one();

        return std::move(future);
    }
}
