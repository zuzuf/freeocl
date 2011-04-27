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
#include "condition.h"

namespace FreeOCL
{
	condition::condition() : i(0)
	{
		pthread_cond_init(&pc, NULL);
	}

	condition::~condition()
	{
		wakeup();
		pthread_cond_destroy(&pc);
	}

	void condition::wait()
	{
		pthread_mutex_lock(&pm);
		const size_t ref = i;
		while(pthread_cond_wait(&pc, &pm) == 0 && ref == i)	{}

		pthread_mutex_unlock(&pm);
	}

	void condition::wait_locked()
	{
		const size_t ref = i;
		while(pthread_cond_wait(&pc, &pm) == 0 && ref == i)	{}
	}
}
