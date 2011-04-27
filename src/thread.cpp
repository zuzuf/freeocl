#include "thread.h"

namespace FreeOCL
{
	thread::thread() : b_running(false)
	{

	}

	thread::~thread()
	{
		join();
	}

	void thread::start()
	{
		lock();
		if (!b_running)
		{
			b_running = true;
			pthread_create(&pt, NULL, thread::exec, (void*)this);
		}
		unlock();
	}

	void *thread::exec(void *p)
	{
		thread *t = static_cast<thread*>(p);
		const int r = t->proc();
		t->lock();
		t->b_running = false;
		t->unlock();
		return (void*)r;
	}

	void thread::kill()
	{
		lock();
		if (b_running)
			pthread_cancel(pt);
		b_running = false;
		unlock();
	}

	void thread::join()
	{
		lock();
		if (b_running)
		{
			unlock();
			pthread_join(pt, NULL);
			lock();
		}
		b_running = false;
		unlock();
	}
}
