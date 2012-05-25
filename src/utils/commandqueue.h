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
#include "utils/smartptr.h"
#include "event.h"
#include <vector>
#include <deque>

namespace FreeOCL
{
	struct command_common : public ref_counter, public valid_flag, public mutex
	{
		smartptr<_cl_event>	event;
		cl_uint				num_events_in_wait_list;
		const cl_event		*event_wait_list;

		command_common()
		{
			ref_counter::release();
		}
		virtual ~command_common() {}

		virtual cl_command_type get_type() const = 0;
	};

	struct command_read_buffer : public command_common
	{
		smartptr<_cl_mem> buffer;
		size_t offset;
		size_t cb;
		void *ptr;

		virtual cl_command_type get_type() const;
	};

	struct command_write_buffer : public command_common
	{
		smartptr<_cl_mem> buffer;
		size_t offset;
		size_t cb;
		const void *ptr;

		virtual cl_command_type get_type() const;
	};

	struct command_copy_buffer : public command_common
	{
		smartptr<_cl_mem> src_buffer;
		smartptr<_cl_mem> dst_buffer;
		size_t src_offset;
		size_t dst_offset;
		size_t cb;

		virtual cl_command_type get_type() const;
	};

	struct command_fill_buffer : public command_common
	{
		smartptr<_cl_mem> buffer;
		size_t offset;
		size_t size;
		void *pattern;
		size_t pattern_size;

		virtual cl_command_type get_type() const;
	};

	struct command_read_buffer_rect : public command_common
	{
		smartptr<_cl_mem> buffer;
		size_t offset;
		size_t cb[3];
		size_t buffer_pitch[2];
		size_t host_pitch[2];
		void *ptr;

		virtual cl_command_type get_type() const;
	};

	struct command_write_buffer_rect : public command_common
	{
		smartptr<_cl_mem> buffer;
		size_t offset;
		size_t cb[3];
		size_t buffer_pitch[2];
		size_t host_pitch[2];
		const void *ptr;

		virtual cl_command_type get_type() const;
	};

	struct command_fill_image : public command_common
	{
		smartptr<_cl_mem> buffer;
		size_t offset;
		size_t cb[3];
		size_t buffer_pitch[2];
		void *fill_color;

		virtual cl_command_type get_type() const;

		void process() const;
	};

	struct command_copy_buffer_rect : public command_common
	{
		smartptr<_cl_mem> src_buffer;
		smartptr<_cl_mem> dst_buffer;
		size_t src_offset;
		size_t dst_offset;
		size_t cb[3];
		size_t src_pitch[2];
		size_t dst_pitch[2];

		virtual cl_command_type get_type() const;
	};

	struct command_read_image : public command_read_buffer_rect
	{
		virtual cl_command_type get_type() const;
	};

	struct command_write_image : public command_write_buffer_rect
	{
		virtual cl_command_type get_type() const;
	};

	struct command_copy_image : public command_copy_buffer_rect
	{
		virtual cl_command_type get_type() const;
	};

	struct command_copy_image_to_buffer : public command_copy_buffer_rect
	{
		virtual cl_command_type get_type() const;
	};

	struct command_copy_buffer_to_image : public command_copy_buffer_rect
	{
		virtual cl_command_type get_type() const;
	};

	struct command_map_buffer : public command_common
	{
		smartptr<_cl_mem> buffer;
		void *ptr;

		virtual cl_command_type get_type() const;
	};

	struct command_map_image : public command_map_buffer
	{
		virtual cl_command_type get_type() const;
	};

	struct command_unmap_buffer : public command_map_buffer
	{
		virtual cl_command_type get_type() const;
	};

	struct command_marker : public command_common
	{
		virtual cl_command_type get_type() const;
	};

	struct command_native_kernel : public command_common
	{
		void (*user_func)(void *);
		void *args;

		virtual cl_command_type get_type() const;
	};

	struct command_ndrange_kernel : public command_common
	{
		smartptr<_cl_kernel> kernel;
		void *args;
		cl_uint dim;
		size_t global_offset[3];
		size_t global_size[3];
		size_t local_size[3];

		virtual cl_command_type get_type() const;
	};

	typedef command_common command;
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
		virtual unsigned long proc();

	private:
		cl_command_queue command_queue;
	};

private:
	std::deque<FreeOCL::smartptr<FreeOCL::command> >	queue;
	thread q_thread;
	volatile bool b_stop;

public:
	bool empty();
	void enqueue(const FreeOCL::smartptr<FreeOCL::command> &cmd);

private:
	unsigned long proc();
};

#endif
