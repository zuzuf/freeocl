/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
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
		if (b_running)
			return;
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
