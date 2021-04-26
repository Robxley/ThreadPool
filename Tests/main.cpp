
#include <iostream>
#include <mutex>

#include "ThreadPool.h"

//Make cout thread safe
std::mutex m_safe_cout;
#define safe_cout(msg) { std::unique_lock<std::mutex> lck(m_safe_cout);  std::cout << msg << std::endl; }

void ThreadSleep(int i){
	std::this_thread::sleep_for(std::chrono::seconds(i));
}

class ThreadVerbose
{
public:
	ThreadVerbose(const std::string & cmsg) : smsg(cmsg) {
		auto id = std::this_thread::get_id();
		safe_cout("Start :" << smsg.c_str() << " : " << id);
	};
	~ThreadVerbose() {
		safe_cout("End :" << smsg.c_str());
	}
	std::string smsg;
};


using Foo = struct
{
	int x, y;
};

/// <summary>
/// Simple test function.
/// Create a task and collect results.
/// </summary>
void SimpleTasks()
{
	std::cout << "Simple Tasks:" << std::endl;

	//Create/Get a pool thread (singleton) with a thread size set to 2.
	auto & pool = ThreadPool::singleton(2);


	//Classic task creation - Boring task creation...
	//--------------------------------------------
	using ThreadTaskVoid = ThreadTask<void>;
	ThreadTaskVoid task_constructor([]{
		ThreadVerbose vth("Task constructor");
		ThreadSleep(3);
	});
	pool.enqueue(task_constructor);

	using ThreadTaskFloat = ThreadTask<float>;
	ThreadTaskFloat task_setter;
	task_setter.set( [] {
		ThreadVerbose vth("Task setter");
		ThreadSleep(2);
		return 3.14f;
	});
	pool.enqueue(task_setter);

	//Task creation by pool enqueue - faster / easier way
	//--------------------------------------------
	auto task_void = pool.enqueue([]
	{
		ThreadVerbose vth("Task void");
		ThreadSleep(3);
	});

	auto task_int = pool.enqueue([]
	{
		ThreadVerbose vth("Task int");
		ThreadSleep(2);
		return 123;
	});

	auto task_foo = pool.enqueue([]
	{
		ThreadVerbose vth("Task Foo");
		ThreadSleep(1);
		return Foo{ 2,3 };
	});

	auto task_vector = pool.enqueue([]
	{
		ThreadVerbose vth("Task Foo");
		ThreadSleep(1);
		return std::string{"abcd"};
	});

	//Join party
	{
		task_constructor.join();					// task returning a "void value"
		float setter_result = task_setter.join();	// task returning a float value

		task_void.join();							// task returning a void value
		int int_result = task_int.join();			// task returning a int value
		Foo foo_result = task_foo.join();			// task returning a custom structure value
		std::string str = task_vector.join();       // task returning a string structure value

		safe_cout("Task using setter : "			<< setter_result);
		safe_cout("Task int result: "				<< int_result);
		safe_cout("Task foo structure result: "		<< foo_result.x << ":" << foo_result.y);
		safe_cout("Task vector result: "			<< str.c_str());
	}

}



/// <summary>
/// Advanced tests
/// Try a dead lock in the cascading task call.
///	Of course, we expect that to be not possible.
/// </summary>
void TryDeadLock()
{
	std::cout << "Try dead locks:" << std::endl;

	//Create/Get a pool thread (singleton) with a thread size set to 2.
	auto & pool = ThreadPool::singleton(2);

	//Fill completely the thread pool (of size 2) with two tasks.
	//Each of these tasks creates also two new tasks.
	//At the end, the thread pool is fed with 6 tasks, 4 of them are inside 2 of them.

	//Create a task containing two tasks
	auto thA = pool.enqueue([&pool]
	{
		ThreadVerbose vth1("thA");
		ThreadSleep(1);

		auto thA_1 = pool.enqueue([]{
			ThreadVerbose vth1("thA_1");
			ThreadSleep(3);
		});

		auto thA_2 = pool.enqueue([] {
			ThreadVerbose vth1("thA_2");
			ThreadSleep(2);
		});

		thA_1.join();
		thA_2.join();
	});
	
	//Create a task containing two tasks
	auto thB = pool.enqueue([&pool]
	{
		ThreadVerbose vth1("thB");
		ThreadSleep(1);

		auto thB_1 = pool.enqueue([] {
			ThreadVerbose vth1("thB_1");
			ThreadSleep(3);
		});

		auto thB_2 = pool.enqueue([] {
			ThreadVerbose vth1("thB_2");
			ThreadSleep(2);
		});

		thB_1.join();
		thB_2.join();
	});


	thB.join();
	thA.join();
}



int main()
{
	//Simple task testing
	SimpleTasks();

	//Some task create new tasks. Check if deadlock is avoided
	TryDeadLock();

	system("Pause");
	return 0;
}