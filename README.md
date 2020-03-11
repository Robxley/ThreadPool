# Thread pool c++ 17

A simple C++17 Thread Pool implementation supporting "internal" tasks without interlock.

# Philosophy

1. A task is added to the pool thread.     
2. The pool thread is running on the task list.   
3. When the result of a task is required, if the task is not completed and has not been started by the thread pool, this task is executed thread-locally and removed from the thread pool.

One rule :
The first arrived on the task, runs the task !

# Exemple :

```cpp
void main()
{
	auto & pool = ThreadPool::singleton(2);
	
	auto thA = pool.enqueue([&pool]
	{
		auto thA_1 = pool.enqueue([]{
		    // Do something awesome!
		});
		auto thA_2 = pool.enqueue([]{
		    // Do something crasy!
		});

		thA_1.join();
		thA_2.join();
	});
	
	auto thB = pool.enqueue([&pool]
	{

		auto thB_1 = pool.enqueue([] {
			// your dream here
		});

		auto thB_2 = pool.enqueue([] {
			// something
		});

		thB_1.join();
		thB_2.join();
	});


	thB.join();
	thA.join();
}
```
