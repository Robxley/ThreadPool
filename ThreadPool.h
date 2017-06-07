
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <atomic>
#include <map>

#include "ThreadTask.h"



class ThreadPool {
	ThreadPool(size_t);
public:

	static ThreadPool & singleton(size_t size = 8)
	{
		static ThreadPool singleton(size);
		return singleton;
	}

	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)
		->ThreadTask<typename std::result_of<F(Args...)>::type>;

	template<class F>
	void enqueue(const ThreadTask<F> & task);

	~ThreadPool();

private:

	// need to keep track of threads so we can join them
	std::vector< std::thread > workers;
	
	// the task queue
	std::queue< std::function<void()> > tasks;

	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;

	size_t pool_size;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
	: stop(false), pool_size(threads)
{
	workers.reserve(threads);

	for (size_t i = 0; i < threads; ++i)
	{
		workers.emplace_back(
			[this,i]
			{
				for (;;)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(this->queue_mutex);
						this->condition.wait(lock,
							[this] { return this->stop || !this->tasks.empty(); });
						
						if (this->stop && this->tasks.empty())
							return;

						task = std::move(this->tasks.front());
						this->tasks.pop();
					}

					task();
				}
			}
		);
	}


}

template<class F>
void ThreadPool::enqueue(const ThreadTask<F> & task)
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		// don't allow enqueueing after stopping the pool
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");
		tasks.emplace(task.fct);
	}

	condition.notify_one();
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
	->ThreadTask<typename std::result_of<F(Args...)>::type>
{
	auto task = MakeTask(std::forward<F>(f), std::forward<Args>(args)...);
	enqueue(task);
	return task;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread &worker : workers)
		worker.join();
}

#endif