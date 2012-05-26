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
#include "mem.h"
#include "context.h"
#include "utils/commandqueue.h"
#include "event.h"
#include "device.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include "prototypes.h"

#define SET_VAR(X)	FreeOCL::copy_memory_within_limits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_mem clCreateBufferFCL (cl_context context,
						   cl_mem_flags flags,
						   size_t size,
						   void *host_ptr,
						   cl_int *errcode_ret)
	{
		MSG(clCreateBufferFCL);
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
		if (!FreeOCL::is_valid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return 0;
		}
		unlock.handle(context);

		cl_mem mem = new _cl_mem(context);
		mem->flags = flags;
		mem->size = size;
		mem->mem_type = CL_MEM_OBJECT_BUFFER;
		mem->host_ptr = host_ptr;
		mem->parent = NULL;
		mem->offset = 0;
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

	cl_mem clCreateSubBufferFCL (cl_mem buffer,
							  cl_mem_flags flags,
							  cl_buffer_create_type buffer_create_type,
							  const void *buffer_create_info,
							  cl_int *errcode_ret)
	{
		MSG(clCreateSubBufferFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(buffer))
		{
			SET_RET(CL_INVALID_MEM_OBJECT);
			return 0;
		}
		unlock.handle(buffer);

		if (buffer->parent)
		{
			SET_RET(CL_INVALID_MEM_OBJECT);
			return 0;
		}

		if (((buffer->flags & CL_MEM_WRITE_ONLY) && ((flags & CL_MEM_READ_WRITE) || (flags & CL_MEM_READ_ONLY)))
				|| ((buffer->flags & CL_MEM_READ_ONLY) && ((flags & CL_MEM_READ_WRITE) || (flags & CL_MEM_WRITE_ONLY)))
				|| (flags & CL_MEM_USE_HOST_PTR)
				|| (flags & CL_MEM_ALLOC_HOST_PTR)
				|| (flags & CL_MEM_COPY_HOST_PTR))
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}

		if (buffer_create_info == NULL)
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}

		switch(buffer_create_type)
		{
		case CL_BUFFER_CREATE_TYPE_REGION:
			{
				const _cl_buffer_region *p_info = (const _cl_buffer_region *)buffer_create_info;
				if (p_info->size == 0)
				{
					SET_RET(CL_INVALID_BUFFER_SIZE);
					return 0;
				}
				if (p_info->size + p_info->origin > buffer->size)
				{
					SET_RET(CL_INVALID_VALUE);
					return 0;
				}
				if ((p_info->origin % FreeOCL::device->mem_base_addr_align) != 0)
				{
					SET_RET(CL_MISALIGNED_SUB_BUFFER_OFFSET);
					return 0;
				}

				cl_mem mem = new _cl_mem(buffer->context);
				mem->flags = flags;
				mem->size = p_info->size;
				mem->mem_type = CL_MEM_OBJECT_BUFFER;
				mem->host_ptr = NULL;
				mem->parent = buffer;
				mem->offset = p_info->origin;
				mem->ptr = (char*)buffer->ptr + p_info->origin;

				SET_RET(CL_SUCCESS);
				return mem;
			}
			break;
		default:
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}

		SET_RET(CL_INVALID_VALUE);
		return 0;
	}

	cl_int clRetainMemObjectFCL (cl_mem memobj)
	{
		MSG(clRetainMemObjectFCL);
		if (!FreeOCL::is_valid(memobj))
			return CL_INVALID_MEM_OBJECT;

		memobj->retain();
		memobj->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseMemObjectFCL (cl_mem memobj)
	{
		MSG(clReleaseMemObjectFCL);
		if (!FreeOCL::is_valid(memobj))
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

	cl_int clSetMemObjectDestructorCallbackFCL (cl_mem memobj,
											 void (CL_CALLBACK *pfn_notify)(cl_mem memobj,
																			void *user_data),
											 void *user_data)
	{
		MSG(clSetMemObjectDestructorCallbackFCL);
		if (pfn_notify)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(memobj))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(memobj);

		FreeOCL::mem_call_back call_back = { pfn_notify, user_data };
		memobj->call_backs.push_front(call_back);		// They are called in reverse order
		return CL_SUCCESS;
	}

	cl_int clGetMemObjectInfoFCL (cl_mem memobj,
							   cl_mem_info param_name,
							   size_t param_value_size,
							   void *param_value,
							   size_t *param_value_size_ret)
	{
		MSG(clGetMemObjectInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(memobj))
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
		case CL_MEM_OFFSET:					bTooSmall = SET_VAR(memobj->offset);	break;
		default:
			return CL_INVALID_VALUE;
		}

		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clEnqueueReadBufferFCL (cl_command_queue command_queue,
								cl_mem buffer,
								cl_bool blocking_read,
								size_t offset,
								size_t cb,
								void *ptr,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		MSG(clEnqueueReadBufferFCL);
		FreeOCL::unlocker unlock;
		if (ptr == NULL)
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(buffer))
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

		FreeOCL::smartptr<FreeOCL::command_read_buffer> cmd = new FreeOCL::command_read_buffer;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = (blocking_read == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = buffer;
		cmd->offset = offset;
		cmd->cb = cb;
		cmd->ptr = ptr;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_READ_BUFFER;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		if (blocking_read == CL_TRUE)
		{
			clWaitForEventsFCL(1, &(cmd->event.weak()));
			if (event == NULL)
				clReleaseEventFCL(cmd->event.weak());
		}

		return CL_SUCCESS;
	}

	cl_int clEnqueueWriteBufferFCL (cl_command_queue command_queue,
								 cl_mem buffer,
								 cl_bool blocking_write,
								 size_t offset,
								 size_t cb,
								 const void *ptr,
								 cl_uint num_events_in_wait_list,
								 const cl_event *event_wait_list,
								 cl_event *event)
	{
		MSG(clEnqueueWriteBufferFCL);
		FreeOCL::unlocker unlock;
		if (ptr == NULL)
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(buffer))
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

		FreeOCL::smartptr<FreeOCL::command_write_buffer> cmd = new FreeOCL::command_write_buffer;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = (blocking_write == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = buffer;
		cmd->offset = offset;
		cmd->cb = cb;
		cmd->ptr = ptr;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_WRITE_BUFFER;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		if (blocking_write == CL_TRUE)
		{
			clWaitForEventsFCL(1, &(cmd->event.weak()));
			if (event == NULL)
				clReleaseEventFCL(cmd->event.weak());
		}

		return CL_SUCCESS;
	}

	cl_int clEnqueueCopyBufferFCL (cl_command_queue command_queue,
								cl_mem src_buffer,
								cl_mem dst_buffer,
								size_t src_offset,
								size_t dst_offset,
								size_t cb,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		MSG(clEnqueueCopyBufferFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(src_buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(src_buffer);
		if (src_buffer->size < src_offset + cb)
			return CL_INVALID_VALUE;

		if (dst_buffer != src_buffer)		// Don't lock it twice if it's the same buffer
		{
			if (!FreeOCL::is_valid(dst_buffer))
				return CL_INVALID_MEM_OBJECT;
			unlock.handle(dst_buffer);
			if (dst_buffer->size < dst_offset + cb)
				return CL_INVALID_VALUE;
		}

		if (src_buffer == dst_buffer
			&& std::max(src_offset, dst_offset) - std::min(src_offset, dst_offset) < cb)
			return CL_MEM_COPY_OVERLAP;

		FreeOCL::smartptr<FreeOCL::command_copy_buffer> cmd = new FreeOCL::command_copy_buffer;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = event ? new _cl_event(command_queue->context) : NULL;
		cmd->src_buffer = src_buffer;
		cmd->src_offset = src_offset;
		cmd->dst_buffer = dst_buffer;
		cmd->dst_offset = dst_offset;
		cmd->cb = cb;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_COPY_BUFFER;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		return CL_SUCCESS;
	}

	void * clEnqueueMapBufferFCL (cl_command_queue command_queue,
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
		MSG(clEnqueueMapBufferFCL);
		if (map_flags & ~(CL_MAP_READ | CL_MAP_WRITE | CL_MAP_WRITE_INVALIDATE_REGION))
		{
			SET_RET(CL_INVALID_VALUE);
			return NULL;
		}
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
		{
			SET_RET(CL_INVALID_COMMAND_QUEUE);
			return NULL;
		}
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return NULL;
		}
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(buffer))
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
				cl_event e = new _cl_event(command_queue->context);
				*event = e;
				e->command_queue = command_queue;
				e->command_type = CL_COMMAND_MAP_BUFFER;
				e->status = CL_QUEUED;
				e->change_status(CL_QUEUED);
				e->change_status(CL_SUBMITTED);
				e->change_status(CL_RUNNING);
				e->change_status(CL_COMPLETE);
			}
		}
		else
		{
			FreeOCL::smartptr<FreeOCL::command_map_buffer> cmd = new FreeOCL::command_map_buffer;
			cmd->num_events_in_wait_list = num_events_in_wait_list;
			cmd->event_wait_list = event_wait_list;
			cmd->event = (blocking_map == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
			if (cmd->event)
			{
				cmd->event->command_queue = command_queue;
				cmd->event->command_type = CL_COMMAND_MAP_BUFFER;
				cmd->event->status = CL_QUEUED;
				if (event)
					*event = cmd->event.weak();
			}
			cmd->buffer = buffer;
			cmd->ptr = p;

			unlock.forget(command_queue);
			command_queue->enqueue(cmd);
			unlock.unlockall();

			if (blocking_map == CL_TRUE)
			{
				clWaitForEventsFCL(1, &cmd->event.weak());
				if (event == NULL)
					clReleaseEventFCL(cmd->event.weak());
			}
		}
		SET_RET(CL_SUCCESS);
		return p;
	}

	cl_int clEnqueueUnmapMemObjectFCL (cl_command_queue command_queue,
									cl_mem memobj,
									void *mapped_ptr,
									cl_uint num_events_in_wait_list,
									const cl_event *event_wait_list,
									cl_event *event)
	{
		MSG(clEnqueueUnmapMemObjectFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(memobj))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(memobj);

		FreeOCL::smartptr<FreeOCL::command_unmap_buffer> cmd = new FreeOCL::command_unmap_buffer;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = event ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = memobj;
		cmd->ptr = mapped_ptr;

		if (cmd->event)
		{
			*event = cmd->event.weak();
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_UNMAP_MEM_OBJECT;
			cmd->event->status = CL_QUEUED;
		}
		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		return CL_SUCCESS;
	}

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueCopyBufferRectFCL(cl_command_queue command_queue,
							cl_mem              src_buffer,
							cl_mem              dst_buffer,
							const size_t *      src_origin,
							const size_t *      dst_origin,
							const size_t *      region,
							size_t              src_row_pitch,
							size_t              src_slice_pitch,
							size_t              dst_row_pitch,
							size_t              dst_slice_pitch,
							cl_uint             num_events_in_wait_list,
							const cl_event *    event_wait_list,
							cl_event *          event) CL_API_SUFFIX__VERSION_1_1
	{
		MSG(clEnqueueCopyBufferRectFCL);
		FreeOCL::unlocker unlock;
		if (src_row_pitch == 0)	src_row_pitch = region[0];
		if (dst_row_pitch == 0)	dst_row_pitch = region[0];
		if (src_slice_pitch == 0)	src_slice_pitch = region[1] * src_row_pitch;
		if (dst_slice_pitch == 0)	dst_slice_pitch = region[1] * dst_row_pitch;

		if (region[0] == 0
				|| region[1] == 0
				|| region[2] == 0
				|| src_row_pitch < region[0]
				|| dst_row_pitch < region[0]
				|| src_slice_pitch < region[1] * src_row_pitch
				|| dst_slice_pitch < region[1] * dst_row_pitch)
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(src_buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(src_buffer);

		if (!FreeOCL::is_valid(dst_buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(dst_buffer);

		if (src_buffer->size < src_origin[0] + region[0]
							+ (src_origin[1] + region[1]) * src_row_pitch
							+ (src_origin[2] + region[2]) * src_slice_pitch)
			return CL_INVALID_VALUE;

		if (dst_buffer->size < dst_origin[0] + region[0]
							+ (dst_origin[1] + region[1]) * dst_row_pitch
							+ (dst_origin[2] + region[2]) * dst_slice_pitch)
			return CL_INVALID_VALUE;

		if (dst_buffer == src_buffer
				&& (dst_origin[0] + region[0] > src_origin[0]
					|| dst_origin[0] < src_origin[0] + region[0])
				&& (dst_origin[1] + region[1] > src_origin[1]
					|| dst_origin[1] < src_origin[1] + region[1])
				&& (dst_origin[2] + region[2] > src_origin[2]
					|| dst_origin[2] < src_origin[2] + region[2]))
			return CL_MEM_COPY_OVERLAP;

		FreeOCL::smartptr<FreeOCL::command_copy_buffer_rect> cmd = new FreeOCL::command_copy_buffer_rect;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = event ? new _cl_event(command_queue->context) : NULL;
		cmd->src_buffer = src_buffer;
		cmd->src_offset = src_origin[0] + src_origin[1] * src_row_pitch + src_origin[2] * src_slice_pitch;
		cmd->dst_buffer = dst_buffer;
		cmd->dst_offset = dst_origin[0] + dst_origin[1] * dst_row_pitch + dst_origin[2] * dst_slice_pitch;
		cmd->cb[0] = region[0];
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->src_pitch[0] = src_row_pitch;
		cmd->src_pitch[1] = src_slice_pitch;
		cmd->dst_pitch[0] = dst_row_pitch;
		cmd->dst_pitch[1] = dst_slice_pitch;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_WRITE_BUFFER_RECT;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		return CL_SUCCESS;
	}

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueWriteBufferRectFCL(cl_command_queue command_queue,
							 cl_mem              buffer,
							 cl_bool             blocking_write,
							 const size_t *      buffer_origin,
							 const size_t *      host_origin,
							 const size_t *      region,
							 size_t              buffer_row_pitch,
							 size_t              buffer_slice_pitch,
							 size_t              host_row_pitch,
							 size_t              host_slice_pitch,
							 const void *        ptr,
							 cl_uint             num_events_in_wait_list,
							 const cl_event *    event_wait_list,
							 cl_event *          event) CL_API_SUFFIX__VERSION_1_1
	{
		MSG(clEnqueueWriteBufferRectFCL);
		FreeOCL::unlocker unlock;
		if (buffer_row_pitch == 0)	buffer_row_pitch = region[0];
		if (host_row_pitch == 0)	host_row_pitch = region[0];
		if (buffer_slice_pitch == 0)	buffer_slice_pitch = region[1] * buffer_row_pitch;
		if (host_slice_pitch == 0)		host_slice_pitch = region[1] * host_row_pitch;

		if (ptr == NULL
				|| region[0] == 0
				|| region[1] == 0
				|| region[2] == 0
				|| buffer_row_pitch < region[0]
				|| host_row_pitch < region[0]
				|| buffer_slice_pitch < region[1] * buffer_row_pitch
				|| host_slice_pitch < region[1] * host_row_pitch)
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(buffer);

		if (buffer->size < buffer_origin[0] + region[0]
							+ (buffer_origin[1] + region[1]) * buffer_row_pitch
							+ (buffer_origin[2] + region[2]) * buffer_slice_pitch)
			return CL_INVALID_VALUE;

		if (blocking_write == CL_TRUE)
		{
			for(size_t i = 0 ; i < num_events_in_wait_list ; ++i)
				if (event_wait_list[i]->status < 0)
					return CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
		}

		FreeOCL::smartptr<FreeOCL::command_write_buffer_rect> cmd = new FreeOCL::command_write_buffer_rect;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = (blocking_write == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = buffer;
		cmd->offset = buffer_origin[0] + buffer_origin[1] * buffer_row_pitch + buffer_origin[2] * buffer_slice_pitch;
		cmd->cb[0] = region[0];
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->ptr = (const char*)ptr + host_origin[0] + host_origin[1] * host_row_pitch + host_origin[2] * host_slice_pitch;
		cmd->buffer_pitch[0] = buffer_row_pitch;
		cmd->buffer_pitch[1] = buffer_slice_pitch;
		cmd->host_pitch[0] = host_row_pitch;
		cmd->host_pitch[1] = host_slice_pitch;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_WRITE_BUFFER_RECT;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		if (blocking_write == CL_TRUE)
		{
			clWaitForEventsFCL(1, &(cmd->event.weak()));
			if (event == NULL)
				clReleaseEventFCL(cmd->event.weak());
		}

		return CL_SUCCESS;
	}

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueReadBufferRectFCL(cl_command_queue command_queue,
							cl_mem              buffer,
							cl_bool             blocking_read,
							const size_t *      buffer_origin,
							const size_t *      host_origin,
							const size_t *      region,
							size_t              buffer_row_pitch,
							size_t              buffer_slice_pitch,
							size_t              host_row_pitch,
							size_t              host_slice_pitch,
							void *              ptr,
							cl_uint             num_events_in_wait_list,
							const cl_event *    event_wait_list,
							cl_event *          event) CL_API_SUFFIX__VERSION_1_1
	{
		MSG(clEnqueueReadBufferRectFCL);
		FreeOCL::unlocker unlock;
		if (buffer_row_pitch == 0)	buffer_row_pitch = region[0];
		if (host_row_pitch == 0)	host_row_pitch = region[0];
		if (buffer_slice_pitch == 0)	buffer_slice_pitch = region[1] * buffer_row_pitch;
		if (host_slice_pitch == 0)		host_slice_pitch = region[1] * host_row_pitch;

		if (ptr == NULL
				|| region[0] == 0
				|| region[1] == 0
				|| region[2] == 0
				|| buffer_row_pitch < region[0]
				|| host_row_pitch < region[0]
				|| buffer_slice_pitch < region[1] * buffer_row_pitch
				|| host_slice_pitch < region[1] * host_row_pitch)
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(buffer);

		if (buffer->size < buffer_origin[0] + region[0]
							+ (buffer_origin[1] + region[1]) * buffer_row_pitch
							+ (buffer_origin[2] + region[2]) * buffer_slice_pitch)
			return CL_INVALID_VALUE;

		if (blocking_read == CL_TRUE)
		{
			for(size_t i = 0 ; i < num_events_in_wait_list ; ++i)
				if (event_wait_list[i]->status < 0)
					return CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
		}

		FreeOCL::smartptr<FreeOCL::command_read_buffer_rect> cmd = new FreeOCL::command_read_buffer_rect;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = (blocking_read == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = buffer;
		cmd->offset = buffer_origin[0] + buffer_origin[1] * buffer_row_pitch + buffer_origin[2] * buffer_slice_pitch;
		cmd->cb[0] = region[0];
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->ptr = (char*)ptr + host_origin[0] + host_origin[1] * host_row_pitch + host_origin[2] * host_slice_pitch;
		cmd->buffer_pitch[0] = buffer_row_pitch;
		cmd->buffer_pitch[1] = buffer_slice_pitch;
		cmd->host_pitch[0] = host_row_pitch;
		cmd->host_pitch[1] = host_slice_pitch;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_READ_BUFFER_RECT;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		if (blocking_read == CL_TRUE)
		{
			clWaitForEventsFCL(1, &(cmd->event.weak()));
			if (event == NULL)
				clReleaseEventFCL(cmd->event.weak());
		}

		return CL_SUCCESS;
	}

	CL_API_ENTRY cl_int CL_API_CALL	clEnqueueFillBufferFCL(cl_command_queue   command_queue,
														   cl_mem             buffer,
														   const void *       pattern,
														   size_t             pattern_size,
														   size_t             offset,
														   size_t             size,
														   cl_uint            num_events_in_wait_list,
														   const cl_event *   event_wait_list,
														   cl_event *         event) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clEnqueueFillBufferFCL);

		if ((pattern_size & (pattern_size - 1))
				|| pattern_size > 128
				|| pattern_size == 0
				|| pattern == NULL)
			return CL_INVALID_VALUE;

		if (offset % pattern_size || size % pattern_size)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(buffer);

		if (buffer->context != command_queue->context)
			return CL_INVALID_CONTEXT;

		if (buffer->size < offset + size)
			return CL_INVALID_VALUE;

		FreeOCL::smartptr<FreeOCL::command_fill_buffer> cmd = new FreeOCL::command_fill_buffer;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = event ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = buffer;
		cmd->offset = offset;
		cmd->size = size;
		cmd->pattern_size = pattern_size;
		cmd->pattern = malloc(pattern_size);
		memcpy(cmd->pattern, pattern, pattern_size);

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_FILL_BUFFER;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		return CL_SUCCESS;
	}

	CL_API_ENTRY cl_int CL_API_CALL	clEnqueueMigrateMemObjectsFCL(cl_command_queue       command_queue,
																  cl_uint                num_mem_objects,
																  const cl_mem *         mem_objects,
																  cl_mem_migration_flags flags,
																  cl_uint                num_events_in_wait_list,
																  const cl_event *       event_wait_list,
																  cl_event *             event) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clEnqueueMigrateMemObjectsFCL);

		if (num_mem_objects == 0 || mem_objects == NULL)
			return CL_INVALID_VALUE;

		if ((num_events_in_wait_list > 0 && mem_objects == NULL)
				|| (num_events_in_wait_list == 0 && mem_objects != NULL))
			return CL_INVALID_EVENT_WAIT_LIST;

		if (flags & ~ (CL_MIGRATE_MEM_OBJECT_HOST | CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED))
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;

		// We have nothing to do except when an event is requested
		if (event)
		{
			FreeOCL::smartptr<FreeOCL::command_marker> cmd = new FreeOCL::command_marker;
			cmd->num_events_in_wait_list = num_events_in_wait_list;
			cmd->event_wait_list = event_wait_list;
			if (event)
			{
				cmd->event = *event = new _cl_event(command_queue->context);
				cmd->event->command_queue = command_queue;
				cmd->event->command_type = CL_COMMAND_MIGRATE_MEM_OBJECTS;
				cmd->event->status = CL_SUBMITTED;
			}
			else
				cmd->event = NULL;

			command_queue->enqueue(cmd);
		}
		else
			command_queue->unlock();

		return CL_SUCCESS;
	}
}

_cl_mem::_cl_mem(cl_context context) : context_resource(context)
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

	if (ptr && !parent && !(flags & CL_MEM_USE_HOST_PTR) && mem_type != CL_MEM_OBJECT_IMAGE1D_BUFFER)
	{
		free(ptr);
		ptr = NULL;
	}
}
