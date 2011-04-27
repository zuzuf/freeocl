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
#include "kernel.h"
#include "commandqueue.h"
#include "context.h"
#include "event.h"
#include "mem.h"
#include <cstdlib>
#include <cstring>

extern "C"
{
	cl_int clEnqueueNativeKernel (cl_command_queue command_queue,
								  void (*user_func)(void *),
								  void *args,
								  size_t cb_args,
								  cl_uint num_mem_objects,
								  const cl_mem *mem_list,
								  const void **args_mem_loc,
								  cl_uint num_events_in_wait_list,
								  const cl_event *event_wait_list,
								  cl_event *event)
	{
		if (user_func == NULL
			|| (args == NULL && cb_args + num_mem_objects > 0)
			|| (cb_args == 0 && args != NULL)
			|| (num_mem_objects > 0 && (mem_list == NULL || args_mem_loc == NULL))
			|| (num_mem_objects == 0 && (mem_list != NULL || args_mem_loc != NULL)))
			return CL_INVALID_VALUE;

		if ((event_wait_list == NULL && num_events_in_wait_list > 0)
			|| (event_wait_list != NULL && num_events_in_wait_list == 0))
			return CL_INVALID_EVENT_WAIT_LIST;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::isValid(command_queue->context))
			return CL_INVALID_CONTEXT;
		unlock.handle(command_queue->context);

		FreeOCL::command cmd;
		cmd.type = CL_COMMAND_NATIVE_KERNEL;
		cmd.common.num_events_in_wait_list = num_events_in_wait_list;
		cmd.common.event_wait_list = event_wait_list;

		cmd.native_kernel.user_func = user_func;
		cmd.native_kernel.args = malloc(cb_args);
		memcpy(cmd.native_kernel.args, args, cb_args);
		for(size_t i = 0 ; i < num_mem_objects ; ++i)
		{
			if (!FreeOCL::isValid(mem_list[i]))
			{
				free(cmd.native_kernel.args);
				return CL_INVALID_MEM_OBJECT;
			}
			const ptrdiff_t offset = (char*)args_mem_loc[i] - (char*)args;
			*((void**)((char*)cmd.native_kernel.args + offset)) = mem_list[i]->ptr;
			mem_list[i]->unlock();
		}

		cmd.common.event = (event != NULL) ? new _cl_event : NULL;
		if (cmd.common.event)
		{
			*event = cmd.common.event;
			cmd.common.event->command_queue = command_queue;
			cmd.common.event->context = command_queue->context;
			cmd.common.event->command_type = CL_COMMAND_NATIVE_KERNEL;
			cmd.common.event->status = CL_SUBMITTED;
		}

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		return CL_SUCCESS;
	}
}
