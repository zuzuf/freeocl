/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef __FREEOCL_MUTEX_H__
#define __FREEOCL_MUTEX_H__

#include <pthread.h>
#include <errno.h>
#include <set>

namespace FreeOCL
{
	class mutex
	{
	public:
		mutex();
		~mutex();

		inline void lock()
		{
			pthread_mutex_lock(&pm);
		}

		inline bool try_lock()
		{
			return pthread_mutex_trylock(&pm) == EBUSY;
		}

		inline void unlock()
		{
			pthread_mutex_unlock(&pm);
		}

	protected:
		pthread_mutex_t pm;
	};

	class unlocker
	{
	public:
		~unlocker();

		void handle(mutex *m);
		inline void handle(mutex &m)	{	handle(&m);	}

		void forget(mutex *m);
		inline void forget(mutex &m)	{	forget(&m);	}

		void unlockall();

	private:
		std::set<mutex*> objects;
	};
}

#endif
