#include "thread_pool.h"

namespace bhd
{

	// the constructor just launches some amount of workers
	thread_pool::thread_pool(size_t threads)
		: stop(false), pool_size(threads)
	{
		workers.reserve(threads);

		for (size_t i = 0; i < threads; ++i)
		{
			workers.emplace_back(
				[this, i]
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

	thread_pool::~thread_pool()
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}
		condition.notify_all();
		for (std::thread &worker : workers)
			worker.join();
	}

}