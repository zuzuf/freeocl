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
		enum { b_is_lockable = true };
	public:
		mutex();
		~mutex();

		inline void lock() const
		{
			pthread_mutex_lock(&pm);
		}

		inline bool try_lock() const
		{
			return pthread_mutex_trylock(&pm) == EBUSY;
		}

		inline void unlock() const
		{
			pthread_mutex_unlock(&pm);
		}

	protected:
		mutable pthread_mutex_t pm;
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
