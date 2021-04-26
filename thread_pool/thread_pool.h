#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <type_traits>

namespace bhd
{
	namespace details
	{
		//Result trick when result is 'void' type
		template<class T>
		struct threaded_task_result { 
			protected: T result;
			using TRef = T&;
			using TCRef = const T&;
		};
		template <>
		struct threaded_task_result<void> {
			using TRef = void;
			using TCRef = void;
		};
	}

	template<class T>
	class threaded_task : public details::threaded_task_result<T>
	{
		friend class thread_pool;
		using TRef = threaded_task_result<T>::TRef;	//reference on the result T or void

		std::function<void()> fct;
		std::future<T> future;

		//private constructor
		threaded_task(const threaded_task&) = delete;
		threaded_task& operator=(const threaded_task&) = delete;

	public:

		threaded_task() {};

		template<class F, class... Args>
		threaded_task(F&& f, Args&&... args) {
			set(std::forward<F>(f), std::forward<Args>(args)...);
		}

		threaded_task(threaded_task&& task)
		{
			this->fct = std::move(task.fct);
			this->future = std::move(task.future);
		}

		void operator=(threaded_task&& task)
		{
			this->fct = std::move(task.fct);
			this->future = std::move(task.future);
		}

		template<class F, class... Args>
		void set(F&& f, Args&&... args)
		{
			using result_t = std::invoke_result_t<F, Args...>;
			using packaged_tr = std::packaged_task<result_t()>;

			auto task = packaged_tr(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

			this->future = task.get_future();

			using atomic_task = std::pair<std::atomic_bool, packaged_tr >;
			auto atask = std::make_shared<atomic_task>(false, std::move(task));

			this->fct = [atask]() mutable
			{
				if (atask->first.exchange(true) == false)
					atask->second();
			};
		}

		auto get() -> TRef
		{
			fct(); //Start the function in the current thread if the function is not yet called
			if constexpr (std::is_void_v<T>)
			{
				if (future.valid()) future.get();
			}
			else
			{
				if (future.valid()) result = future.get();
				return result;
			}
		}
	};

	class thread_pool
	{
		// need to keep track of threads so we can join them
		std::vector< std::thread > workers;

		// the task queue
		std::queue< std::function<void()> > tasks;

		// synchronization
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool stop = false;

		//Number of workers (threads)
		size_t pool_size = 0;

	public:

		~thread_pool();
		thread_pool(size_t);
		thread_pool() : thread_pool(std::thread::hardware_concurrency()) {};

		static thread_pool & instance(size_t size = std::thread::hardware_concurrency())
		{
			static thread_pool singleton(size);
			return singleton;
		}


		template<class T>
		void enqueue(const threaded_task<T> & task)
		{
			// don't allow enqueue after stopping the pool
			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				tasks.emplace(task.fct);
			}
			condition.notify_one();
		}


		template<class F, class... Args>
		auto enqueue(F&& f, Args&&... args) -> threaded_task<std::invoke_result_t<F, Args...>>
		{
			using result_t = std::invoke_result_t<F, Args...>;
			threaded_task<result_t> new_task(std::forward<F>(f), std::forward<Args>(args)...);
			this->enqueue<result_t>(new_task);
			return new_task;
		}

	};

}