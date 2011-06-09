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
#ifndef __FREEOCL_COMMANDQUEUE_H__
#define __FREEOCL_COMMANDQUEUE_H__

#include "freeocl.h"
#include "condition.h"
#include "thread.h"
#include <vector>
#include <deque>

namespace FreeOCL
{
	struct command_common
	{
		cl_command_type type;
		cl_event		event;
		cl_uint			num_events_in_wait_list;
		const cl_event	*event_wait_list;
	};

	struct command_read_buffer : public command_common
	{
		cl_mem buffer;
		size_t offset;
		size_t cb;
		void *ptr;
	};

	struct command_write_buffer : public command_common
	{
		cl_mem buffer;
		size_t offset;
		size_t cb;
		const void *ptr;
	};

	struct command_copy_buffer : public command_common
	{
		cl_mem src_buffer;
		cl_mem dst_buffer;
		size_t src_offset;
		size_t dst_offset;
		size_t cb;
	};

	struct command_map_buffer : public command_common
	{
		cl_mem buffer;
		void *ptr;
	};

	struct command_unmap_buffer : public command_map_buffer
	{
	};

	struct command_marker : public command_common
	{
	};

	struct command_native_kernel : public command_common
	{
		void (*user_func)(void *);
		void *args;
	};

	struct command_ndrange_kernel : public command_common
	{
		cl_kernel kernel;
		void *args;
		cl_uint dim;
		size_t global_offset[3];
		size_t global_size[3];
		size_t local_size[3];
	};

	union command
	{
		cl_command_type			type;
		command_common			common;
		command_read_buffer		read_buffer;
		command_write_buffer	write_buffer;
		command_copy_buffer		copy_buffer;
		command_map_buffer		map_buffer;
		command_unmap_buffer	unmap_buffer;
		command_marker			marker;
		command_native_kernel	native_kernel;
		command_ndrange_kernel	ndrange_kernel;
	};
}

struct _cl_command_queue : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::condition, public FreeOCL::valid_flag, public FreeOCL::context_resource
{
	_cl_command_queue(cl_context);
	~_cl_command_queue();

	cl_device_id device;
	cl_command_queue_properties properties;

private:
	class thread : public FreeOCL::thread
	{
	public:
		thread(cl_command_queue command_queue) : command_queue(command_queue)	{}
		virtual int proc();

	private:
		cl_command_queue command_queue;
	};

private:
	std::deque<FreeOCL::command>	queue;
	FreeOCL::mutex	q_mutex;
	thread q_thread;
	volatile bool b_stop;

public:
	bool empty();
	void enqueue(const FreeOCL::command &cmd);

private:
	int proc();
};

#endif
