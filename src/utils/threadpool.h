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
#ifndef __FREEOCL_THREADPOOL_H__
#define __FREEOCL_THREADPOOL_H__

#include "thread.h"
#include <deque>

namespace FreeOCL
{
	class threadpool
	{
	private:
		class worker : public thread
		{
		public:
			worker() : b_working(false), b_stop(false)	{}
			virtual unsigned long proc();
			void work() const;
			void set_thread_id(const size_t thread_id)	{	this->thread_id = thread_id;	}
			void set_thread_pool(threadpool *pool)	{	this->pool = pool;	}
			bool is_working() const	{	return b_working;	}
			void set_working()	{	b_working = true;	}
			void stop()	{	b_stop = true;	}

		private:
			threadpool *pool;
			size_t thread_id;
			volatile bool b_working;
			volatile bool b_stop;
		};

		friend class worker;

	public:
		threadpool();
		~threadpool();

		void wait_for_all();

		void set_num_groups(const size_t *num_groups);
		void set_local_size(const size_t *local_size);
		void set_thread_num(const size_t nb_threads);

		void run(const void *args, char *local_memory, void (*kernel)(const void*,char*,size_t,const size_t*));

	private:
		void (*kernel)(const void*,char*,size_t,const size_t*);
		char *local_memory;
		const void *args;
		size_t num_groups[3];
		size_t local_size[3];
		size_t nb_threads;
		std::deque<worker> pool;
	};
}

#endif
