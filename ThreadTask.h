
#ifndef THREAD_TASK_H
#define THREAD_TASK_H

#pragma once

#include <memory>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <atomic>


template<class R = void>
class ThreadTask
{
	//Friend function maker. To create a task from a lambda
	template<class F, class... Args> friend
		auto MakeTask(F&& f, Args&&... args)
		->ThreadTask<typename std::result_of<F(Args...)>::type>;

	friend class ThreadPool;

private:
	//private constructeur
	ThreadTask(const ThreadTask&) = delete;
	ThreadTask& operator=(const ThreadTask&) = delete;

	std::function<void()> fct;
	std::future<R> future;
	
	//Generic type  (trick when result can be a 'void' type)
	template <class R>	struct AnyType {
		R any;
	};
	template <> struct AnyType<void> {};

	AnyType<R> result;

public:

	ThreadTask() {};

	template<class F, class... Args>
	ThreadTask(F&& f, Args&&... args){
		set(std::forward<F>(f), std::forward<Args>(args)...);
	}

	ThreadTask(ThreadTask&& task)
	{
		this->fct = std::move(task.fct);
		this->future = std::move(task.future);
	}

	void operator=(ThreadTask&& task)
	{
		this->fct = std::move(task.fct);
		this->future = std::move(task.future);
	}

	template<class F, class... Args>
	void set(F&& f, Args&&... args)
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

		future = task->get_future();

		auto inprogress = std::make_shared<std::atomic<int>>(0);

		fct = [task, inprogress]() {
			if ((*inprogress)++ == 0)
				(*task)();
		};
	}

	R join()
	{
		fct();
		if (future.valid())
			result.any = future.get();
		return result.any;
	}
};

//void specialisation
template <>
void ThreadTask<void>::join()
{
	fct();
	if (future.valid())
		future.get();
};


template<class F, class... Args>
auto MakeTask(F&& f, Args&&... args)
-> ThreadTask<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;
	ThreadTask<return_type> task;
	task.set(std::forward<F>(f), std::forward<Args>(args)...);
	return task;
}


#endif