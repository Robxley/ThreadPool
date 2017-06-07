
#include <iostream>
#include <mutex>

#include "ThreadPool.h"

//Make cout thread safe
std::mutex m_safe_cout;
#define safe_cout(msg) { std::unique_lock<std::mutex> lck(m_safe_cout);  std::cout << msg << std::endl; }

void thread_sleep(int i) {
	std::this_thread::sleep_for(std::chrono::seconds(i));
}

class thread_verbose
{
public:
	thread_verbose(const std::string & cmsg) : smsg(cmsg) {
		auto id = std::this_thread::get_id();
		safe_cout("Start :" << smsg.c_str() << " : " << id);
	};
	~thread_verbose() {
		safe_cout("End :" << smsg.c_str());
	}
	std::string smsg;
};


using Foo = struct
{
	int x, y;
};

using namespace utl;

void SimpleTasks()
{


	std::cout << "Simple Tast" << std::endl;

	//Create a pool thread with a thread size set to 2.
	auto & pool = thread_pool::instance(2);


	//Boring task creation...
	//--------------------------------------------
	using threaded_task_v = threaded_task<void>;
	threaded_task_v task_constructor([] {
		thread_verbose vth("Task constructor");
		thread_sleep(3);
	});
	pool.enqueue(task_constructor);


	using threaded_task_f = threaded_task<float>;
	threaded_task_f task_setter;
	task_setter.set([] {
		thread_verbose vth("Task setter");
		thread_sleep(2);
		return 3.14f;
	});
	pool.enqueue(task_setter);

	//Task creation by pool enqueue, faster - easier
	//--------------------------------------------
	auto task_void = pool.enqueue([]
	{
		thread_verbose vth("Task void");
		thread_sleep(3);
	});

	auto task_int = pool.enqueue([]
	{
		thread_verbose vth("Task int");
		thread_sleep(2);
		return 123;
	});

	auto task_foo = pool.enqueue([]
	{
		thread_verbose vth("Task Foo");
		thread_sleep(1);
		return Foo{ 2,3 };
	});

	auto task_vector = pool.enqueue([]
	{
		thread_verbose vth("Task Foo");
		thread_sleep(1);
		return std::string{ "abcd" };
	});

	//Join party

	task_constructor.get();
	float setter_result = task_setter.get();


	task_void.get();
	int int_result = task_int.get();
	Foo foo_result = task_foo.get();

	std::string str = task_vector.get();

	safe_cout("Tast setter : " << setter_result);
	safe_cout("Task int : " << int_result);
	safe_cout("Task foo : " << foo_result.x << ":" << foo_result.y);
	safe_cout("Task vector : " << str.c_str());

	thread_sleep(5);

	auto task_restart = pool.enqueue([]
	{
		thread_verbose vth("Task restart");
		thread_sleep(3);
	});

	task_restart.get();

}




void TryDeadLock()
{

	std::cout << "\nTryDeadLock Tast" << std::endl;

	auto & pool = thread_pool::instance(2);

	auto thA = pool.enqueue([&pool]
	{
		thread_verbose vth1("thA");
		thread_sleep(1);

		auto thA_1 = pool.enqueue([] {
			thread_verbose vth1("thA_1");
			thread_sleep(3);
		});

		auto thA_2 = pool.enqueue([] {
			thread_verbose vth1("thA_2");
			thread_sleep(2);
		});

		thA_1.get();
		thA_2.get();
	});


	auto thB = pool.enqueue([&pool]
	{
		thread_verbose vth1("thB");
		thread_sleep(1);

		auto thB_1 = pool.enqueue([] {
			thread_verbose vth1("thB_1");
			thread_sleep(3);
		});

		auto thB_2 = pool.enqueue([] {
			thread_verbose vth1("thB_2");
			thread_sleep(2);
		});

		thB_1.get();
		thB_2.get();
	});


	thB.get();
	thA.get();
}



int main()
{


	SimpleTasks();

	TryDeadLock();

	system("Pause");
	return 0;
}