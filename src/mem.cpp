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
#include "mem.h"
#include "context.h"
#include "commandqueue.h"
#include "event.h"
#include <cstring>
#include <iostream>
#include <algorithm>

#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_mem clCreateBuffer (cl_context context,
						   cl_mem_flags flags,
						   size_t size,
						   void *host_ptr,
						   cl_int *errcode_ret)
	{
		if (size == 0)
		{
			SET_RET(CL_INVALID_BUFFER_SIZE);
			return 0;
		}
		if (((flags & (CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR))
			&& host_ptr == NULL)
			|| (host_ptr != NULL && !(flags & (CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR))))
		{
			SET_RET(CL_INVALID_HOST_PTR);
			return 0;
		}

		if ((flags & CL_MEM_USE_HOST_PTR) && (flags & (CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR)))
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}

		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return 0;
		}
		unlock.handle(context);

		cl_mem mem = new _cl_mem;
		mem->context = context;
		mem->flags = flags;
		mem->size = size;
		mem->mem_type = CL_MEM_OBJECT_BUFFER;
		mem->host_ptr = host_ptr;
		mem->parent = NULL;
		if (flags & CL_MEM_USE_HOST_PTR)
			mem->ptr = host_ptr;
		else if (posix_memalign(&(mem->ptr), 256, size) == ENOMEM)
		{
			SET_RET(CL_OUT_OF_RESOURCES);
			delete mem;
			return 0;
		}

		if (flags & CL_MEM_COPY_HOST_PTR)
			memcpy(mem->ptr, host_ptr, size);

		SET_RET(CL_SUCCESS);

		return mem;
	}

	cl_mem clCreateSubBuffer (cl_mem buffer,
							  cl_mem_flags flags,
							  cl_buffer_create_type buffer_create_type,
							  const void *buffer_create_info,
							  cl_int *errcode_ret)
	{
		std::cerr << "Stub : clCreateSubBuffer" << std::endl;
		if (errcode_ret)
			*errcode_ret = CL_INVALID_VALUE;
		return 0;
	}

	cl_int clRetainMemObject (cl_mem memobj)
	{
		if (!FreeOCL::isValid(memobj))
			return CL_INVALID_MEM_OBJECT;

		memobj->retain();
		memobj->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseMemObject (cl_mem memobj)
	{
		if (!FreeOCL::isValid(memobj))
			return CL_INVALID_MEM_OBJECT;

		memobj->release();
		if (memobj->get_ref_count() == 0)
		{
			memobj->invalidate();
			memobj->unlock();
			delete memobj;
		}
		else
			memobj->unlock();
		return CL_SUCCESS;
	}

	cl_int clSetMemObjectDestructorCallback (cl_mem memobj,
											 void (CL_CALLBACK *pfn_notify)(cl_mem memobj,
																			void *user_data),
											 void *user_data)
	{
		if (pfn_notify)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(memobj))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(memobj);

		FreeOCL::mem_call_back call_back = { pfn_notify, user_data };
		memobj->call_backs.push_back(call_back);
		return CL_SUCCESS;
	}

	cl_int clGetMemObjectInfo (cl_mem memobj,
							   cl_mem_info param_name,
							   size_t param_value_size,
							   void *param_value,
							   size_t *param_value_size_ret)
	{
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(memobj))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(memobj);

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_MEM_TYPE:					bTooSmall = SET_VAR(memobj->mem_type);	break;
		case CL_MEM_FLAGS:					bTooSmall = SET_VAR(memobj->flags);	break;
		case CL_MEM_SIZE:					bTooSmall = SET_VAR(memobj->size);	break;
		case CL_MEM_HOST_PTR:				bTooSmall = SET_VAR(memobj->host_ptr);	break;
		case CL_MEM_MAP_COUNT:
			{
				cl_uint n = memobj->mapped.size();
				bTooSmall = SET_VAR(n);
			}
			break;
		case CL_MEM_REFERENCE_COUNT:		bTooSmall = SET_VAR(memobj->get_ref_count());	break;
		case CL_MEM_CONTEXT:				bTooSmall = SET_VAR(memobj->context);	break;
		case CL_MEM_ASSOCIATED_MEMOBJECT:	bTooSmall = SET_VAR(memobj->parent);	break;
		case CL_MEM_OFFSET:
		default:
			return CL_INVALID_VALUE;
		}

		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clEnqueueReadBuffer (cl_command_queue command_queue,
								cl_mem buffer,
								cl_bool blocking_read,
								size_t offset,
								size_t cb,
								void *ptr,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		FreeOCL::unlocker unlock;
		if (ptr == NULL)
			return CL_INVALID_VALUE;

		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::isValid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::isValid(buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(buffer);

		if (buffer->size < offset + cb)
			return CL_INVALID_VALUE;

		if (blocking_read == CL_TRUE)
		{
			for(size_t i = 0 ; i < num_events_in_wait_list ; ++i)
				if (event_wait_list[i]->status < 0)
					return CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
		}

		FreeOCL::command cmd;
		cmd.type = CL_COMMAND_READ_BUFFER;
		cmd.common.num_events_in_wait_list = num_events_in_wait_list;
		cmd.common.event_wait_list = event_wait_list;
		cmd.common.event = (blocking_read == CL_TRUE || event) ? new _cl_event : NULL;
		cmd.read_buffer.buffer = buffer;
		cmd.read_buffer.offset = offset;
		cmd.read_buffer.cb = cb;
		cmd.read_buffer.ptr = ptr;

		if (cmd.common.event)
		{
			cmd.common.event->command_queue = command_queue;
			cmd.common.event->command_type = CL_COMMAND_READ_BUFFER;
			cmd.common.event->context = command_queue->context;
			cmd.common.event->status = CL_SUBMITTED;
		}

		if (event)
			*event = cmd.common.event;

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		if (blocking_read == CL_TRUE)
		{
			clWaitForEvents(1, &cmd.common.event);
			if (event == NULL)
				clReleaseEvent(cmd.common.event);
		}

		return CL_SUCCESS;
	}

	cl_int clEnqueueWriteBuffer (cl_command_queue command_queue,
								 cl_mem buffer,
								 cl_bool blocking_write,
								 size_t offset,
								 size_t cb,
								 const void *ptr,
								 cl_uint num_events_in_wait_list,
								 const cl_event *event_wait_list,
								 cl_event *event)
	{
		FreeOCL::unlocker unlock;
		if (ptr == NULL)
			return CL_INVALID_VALUE;

		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::isValid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::isValid(buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(buffer);

		if (buffer->size < offset + cb)
			return CL_INVALID_VALUE;

		if (blocking_write == CL_TRUE)
		{
			for(size_t i = 0 ; i < num_events_in_wait_list ; ++i)
				if (event_wait_list[i]->status < 0)
					return CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
		}

		FreeOCL::command cmd;
		cmd.type = CL_COMMAND_WRITE_BUFFER;
		cmd.common.num_events_in_wait_list = num_events_in_wait_list;
		cmd.common.event_wait_list = event_wait_list;
		cmd.common.event = (blocking_write == CL_TRUE || event) ? new _cl_event : NULL;
		cmd.write_buffer.buffer = buffer;
		cmd.write_buffer.offset = offset;
		cmd.write_buffer.cb = cb;
		cmd.write_buffer.ptr = ptr;

		if (cmd.common.event)
		{
			cmd.common.event->command_queue = command_queue;
			cmd.common.event->command_type = CL_COMMAND_WRITE_BUFFER;
			cmd.common.event->context = command_queue->context;
			cmd.common.event->status = CL_SUBMITTED;
		}

		if (event)
			*event = cmd.common.event;

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		if (blocking_write == CL_TRUE)
		{
			clWaitForEvents(1, &cmd.common.event);
			if (event == NULL)
				clReleaseEvent(cmd.common.event);
		}

		return CL_SUCCESS;
	}

	cl_int clEnqueueCopyBuffer (cl_command_queue command_queue,
								cl_mem src_buffer,
								cl_mem dst_buffer,
								size_t src_offset,
								size_t dst_offset,
								size_t cb,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::isValid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::isValid(src_buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(src_buffer);
		if (src_buffer->size < src_offset + cb)
			return CL_INVALID_VALUE;

		if (dst_buffer != src_buffer)		// Don't lock it twice if it's the same buffer
		{
			if (!FreeOCL::isValid(dst_buffer))
				return CL_INVALID_MEM_OBJECT;
			unlock.handle(dst_buffer);
			if (dst_buffer->size < dst_offset + cb)
				return CL_INVALID_VALUE;
		}

		if (src_buffer == dst_buffer
			&& std::max(src_offset, dst_offset) - std::min(src_offset, dst_offset) < cb)
			return CL_MEM_COPY_OVERLAP;

		FreeOCL::command cmd;
		cmd.type = CL_COMMAND_COPY_BUFFER;
		cmd.common.num_events_in_wait_list = num_events_in_wait_list;
		cmd.common.event_wait_list = event_wait_list;
		cmd.common.event = event ? new _cl_event : NULL;
		cmd.copy_buffer.src_buffer = src_buffer;
		cmd.copy_buffer.src_offset = src_offset;
		cmd.copy_buffer.dst_buffer = dst_buffer;
		cmd.copy_buffer.dst_offset = dst_offset;
		cmd.copy_buffer.cb = cb;

		if (cmd.common.event)
		{
			cmd.common.event->command_queue = command_queue;
			cmd.common.event->command_type = CL_COMMAND_COPY_BUFFER;
			cmd.common.event->context = command_queue->context;
			cmd.common.event->status = CL_SUBMITTED;
		}

		if (event)
			*event = cmd.common.event;

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		return CL_SUCCESS;
	}

	void * clEnqueueMapBuffer (cl_command_queue command_queue,
							   cl_mem buffer,
							   cl_bool blocking_map,
							   cl_map_flags map_flags,
							   size_t offset,
							   size_t cb,
							   cl_uint num_events_in_wait_list,
							   const cl_event *event_wait_list,
							   cl_event *event,
							   cl_int *errcode_ret)
	{
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(command_queue))
		{
			SET_RET(CL_INVALID_COMMAND_QUEUE);
			return NULL;
		}
		unlock.handle(command_queue);

		if (!FreeOCL::isValid(command_queue->context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return NULL;
		}
		command_queue->context->unlock();

		if (!FreeOCL::isValid(buffer))
		{
			SET_RET(CL_INVALID_MEM_OBJECT);
			return NULL;
		}
		unlock.handle(buffer);

		if (buffer->size < offset + cb)
		{
			SET_RET(CL_INVALID_VALUE);
			return NULL;
		}

		void *p = (char*)buffer->ptr + offset;
		if (num_events_in_wait_list == 0 || event_wait_list == NULL)
		{
			buffer->mapped.insert(p);
			if (event)
			{
				cl_event e = new _cl_event;
				*event = e;
				e->command_queue = command_queue;
				e->context = command_queue->context;
				e->command_type = CL_COMMAND_MAP_BUFFER;
				e->status = CL_COMPLETE;
			}
		}
		else
		{
			FreeOCL::command cmd;
			cmd.type = CL_COMMAND_MAP_BUFFER;
			cmd.common.num_events_in_wait_list = num_events_in_wait_list;
			cmd.common.event_wait_list = event_wait_list;
			cmd.common.event = (blocking_map == CL_TRUE || event) ? new _cl_event : NULL;
			if (cmd.common.event)
			{
				cmd.common.event->command_queue = command_queue;
				cmd.common.event->context = command_queue->context;
				cmd.common.event->command_type = CL_COMMAND_MAP_BUFFER;
				cmd.common.event->status = CL_SUBMITTED;
				if (event)
					*event = cmd.common.event;
			}
			cmd.map_buffer.buffer = buffer;
			cmd.map_buffer.ptr = p;

			unlock.forget(command_queue);
			command_queue->enqueue(cmd);
			unlock.unlockall();

			if (blocking_map == CL_TRUE)
			{
				clWaitForEvents(1, &cmd.common.event);
				if (event == NULL)
					clReleaseEvent(cmd.common.event);
			}
		}
		SET_RET(CL_SUCCESS);
		return p;
	}

	cl_int clEnqueueUnmapMemObject (cl_command_queue command_queue,
									cl_mem memobj,
									void *mapped_ptr,
									cl_uint num_events_in_wait_list,
									const cl_event *event_wait_list,
									cl_event *event)
	{
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::isValid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::isValid(memobj))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(memobj);

		FreeOCL::command cmd;
		cmd.type = CL_COMMAND_UNMAP_MEM_OBJECT;
		cmd.common.num_events_in_wait_list = num_events_in_wait_list;
		cmd.common.event_wait_list = event_wait_list;
		cmd.common.event = event ? new _cl_event : NULL;
		cmd.unmap_buffer.buffer = memobj;
		cmd.unmap_buffer.ptr = mapped_ptr;

		if (cmd.common.event)
		{
			*event = cmd.common.event;
			cmd.common.event->command_queue = command_queue;
			cmd.common.event->context = command_queue->context;
			cmd.common.event->command_type = CL_COMMAND_UNMAP_MEM_OBJECT;
			cmd.common.event->status = CL_SUBMITTED;
		}

		return CL_SUCCESS;
	}
}

_cl_mem::_cl_mem()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_mems.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_mem::~_cl_mem()
{
	std::deque<FreeOCL::mem_call_back> call_backs;
	call_backs.swap(this->call_backs);
	unlock();

	for(std::deque<FreeOCL::mem_call_back>::const_iterator i = call_backs.begin() ; i != call_backs.end() ; ++i)
		i->pfn_notify(this, i->user_data);

	FreeOCL::global_mutex.lock();
	FreeOCL::valid_mems.erase(this);
	FreeOCL::global_mutex.unlock();
}
