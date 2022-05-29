#pragma once

#include "ThreadPool.hpp"

namespace Alphalcazar::Utils {
	ThreadPool::ThreadPool() {
		int numThreads = std::thread::hardware_concurrency();
		mWorkers.reserve(numThreads);

		// The function that each thread will execute on an infinite loop
		auto threadExecuteFunction = [this]() {
			while (true) {
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(mQueueMutex);

					mCondition.wait(lock, [this]() {
						return !mJobsQueue.empty() || mTerminatePool;
					});
					job = mJobsQueue.front();
					mJobsQueue.pop();
				}

				// Unlock the queue mutex before executing the job
				job();
			}
		};

		for (int i = 0; i < numThreads; i++) {
			mWorkers.emplace_back(std::thread(threadExecuteFunction));
		}
	}

	ThreadPool::~ThreadPool() {
		mTerminatePool = true;

		// wake up all threads.
		mCondition.notify_all();

		// Wait for all threads to finish
		for (std::thread& thread : mWorkers) {
			thread.join();
		}
	}
}
