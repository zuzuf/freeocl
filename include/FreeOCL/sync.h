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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_SYNC_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_SYNC_H__

#include "FreeOCL/config.h"
#include "atomic.h"
#include "workitem.h"
#include <time.h>
#include <unistd.h>

typedef int	cl_mem_fence_flags;
#define	CLK_LOCAL_MEM_FENCE		0x0
#define CLK_GLOBAL_MEM_FENCE	0x1

static volatile __uint __barrier_mutex = 0;
static volatile __uint __barrier_exit = 0;

inline void __yield()
{
#if _POSIX_C_SOURCE >= 199309L
	const struct timespec ts = { 0, 0 };
	nanosleep(&ts, NULL);
#else
	usleep(0);
#endif
}

// Built-in synchronization functions
inline void barrier(cl_mem_fence_flags /*flags*/)
{
	const __uint nb_threads = FreeOCL::local_size[0] * FreeOCL::local_size[1] * FreeOCL::local_size[2];

	AO_int_fetch_and_add1(&__barrier_mutex);
	while(__barrier_mutex < nb_threads && !__barrier_exit)
	{
		__yield();
	}
	AO_int_store(&__barrier_exit, 1);
	AO_int_fetch_and_sub1(&__barrier_mutex);
	while(__barrier_mutex > 0 && __barrier_exit)
	{
		__yield();
	}
	AO_int_store(&__barrier_exit, 0);
}

#endif
