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
#include "mutex.h"

namespace FreeOCL
{
	mutex::mutex()
	{
		// Create a default mutex
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&pm, &attr);
		pthread_mutexattr_destroy(&attr);
	}

	mutex::~mutex()
	{
		// Destroy the mutex
		pthread_mutex_destroy(&pm);
	}

	unlocker::~unlocker()
	{
		unlockall();
	}

	void unlocker::handle(mutex *m)
	{
		objects.insert(m);
	}

	void unlocker::forget(mutex *m)
	{
		objects.erase(m);
	}

	void unlocker::unlockall()
	{
		for(std::set<mutex*>::const_iterator i = objects.begin() ; i != objects.end() ; ++i)
			(*i)->unlock();
		objects.clear();
	}
}
