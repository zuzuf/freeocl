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
#include "threadpool.h"
#include <FreeOCL/config.h>
#include <sched.h>
#ifdef FREEOCL_USE_OPENMP
#include <omp.h>
#endif
#ifdef __SSE__
#include <xmmintrin.h>
#endif

namespace FreeOCL
{
	threadpool::threadpool()
	{

	}

	threadpool::~threadpool()
	{
		for(size_t i = 0 ; i < pool.size() ; ++i)
			pool[i].stop();
	}

	void threadpool::wait_for_all()
	{
		for(size_t i = 1 ; i < pool.size() ; ++i)
		{
			while (pool[i].is_working())
			{
#ifdef __SSE__
				_mm_pause();
#endif
			}
		}
	}

	void threadpool::set_num_groups(const size_t *num_groups)
	{
		for(size_t i = 0 ; i < 3 ; ++i)
			this->num_groups[i] = num_groups[i];
	}

	void threadpool::set_local_size(const size_t *local_size)
	{
		for(size_t i = 0 ; i < 3 ; ++i)
			this->local_size[i] = local_size[i];
	}

	void threadpool::set_thread_num(const size_t nb_threads)
	{
		this->nb_threads = nb_threads;
		if (nb_threads > pool.size())
		{
			pool.resize(nb_threads);
			for(size_t i = 0 ; i < nb_threads ; ++i)
			{
				pool[i].set_thread_id(i);
				pool[i].set_thread_pool(this);
			}
		}
	}

	void threadpool::run(const void *args, char *local_memory, void (*kernel)(const void*,char*,size_t,const size_t*))
	{
		this->args = args;
		this->local_memory = local_memory;
		this->kernel = kernel;

#ifdef FREEOCL_USE_OPENMP
		omp_set_num_threads(nb_threads);
		const size_t num = local_size[0] * local_size[1] * local_size[2];
#pragma omp parallel
		{
			size_t group_id[3];
			for(group_id[2] = 0 ; group_id[2] < num_groups[2] ; ++group_id[2])
			{
				for(group_id[1] = 0 ; group_id[1] < num_groups[1] ; ++group_id[1])
				{
					for(group_id[0] = 0 ; group_id[0] < num_groups[0] ; ++group_id[0])
					{
#pragma omp for nowait
						for(size_t i = 0 ; i < num ; ++i)
							kernel(args, local_memory, i, group_id);
					}
				}
			}
		}
#else
		for(size_t i = 1 ; i < nb_threads ; ++i)
		{
			pool[i].set_working();
			pool[i].start();
		}
		pool.front().work();
		wait_for_all();
#endif
	}

	unsigned long threadpool::worker::proc()
	{
		while(!b_stop)
		{
			while(!b_working && !b_stop)
			{
#ifdef __SSE__
				_mm_pause();
#endif
				sched_yield();
			}
			if (b_stop)
			{
				b_working = false;
				return 0;
			}
			work();
			b_working = false;
		}
		return 0;
	}

	void threadpool::worker::work() const
	{
		const size_t num = pool->local_size[0] * pool->local_size[1] * pool->local_size[2];
		const size_t nb_threads = pool->nb_threads;
		const size_t num_groups[] = { pool->num_groups[0], pool->num_groups[1], pool->num_groups[2] };
		char * const local_memory = pool->local_memory;
		const void * const args = pool->args;
		size_t group_id[3];
		for(group_id[2] = 0 ; group_id[2] < num_groups[2] ; ++group_id[2])
		{
			for(group_id[1] = 0 ; group_id[1] < num_groups[1] ; ++group_id[1])
			{
				for(group_id[0] = 0 ; group_id[0] < num_groups[0] ; ++group_id[0])
				{
					for(size_t i = thread_id ; i < num ; i += nb_threads)
						pool->kernel(args, local_memory, i, group_id);
				}
			}
		}
	}
}
