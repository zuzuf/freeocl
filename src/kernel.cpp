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
#include "program.h"
#include <cstdlib>
#include <cstring>

#define SET_STRING(X)	FreeOCL::copyMemoryWithinLimits(X, strlen(X) + 1, param_value_size, param_value, param_value_size_ret)
#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_int clEnqueueNativeKernelFCL (cl_command_queue command_queue,
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
		MSG(clEnqueueNativeKernelFCL);
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

	cl_kernel clCreateKernelFCL (cl_program program,
							  const char *kernel_name,
							  cl_int *errcode_ret)
	{
		MSG(clCreateKernelFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(program))
		{
			SET_RET(CL_INVALID_PROGRAM);
			return 0;
		}
		unlock.handle(program);

		cl_kernel kernel = new _cl_kernel;
		kernel->program = program;

		SET_RET(CL_SUCCESS);

		return kernel;
	}

	cl_int clCreateKernelsInProgramFCL (cl_program program,
									 cl_uint num_kernels,
									 cl_kernel *kernels,
									 cl_uint *num_kernels_ret)
	{
		MSG(clCreateKernelsInProgramFCL);
		return CL_INVALID_OPERATION;
	}

	cl_int clRetainKernelFCL (cl_kernel kernel)
	{
		MSG(clRetainKernelFCL);
		if (!FreeOCL::isValid(kernel))
			return CL_INVALID_KERNEL;

		kernel->retain();
		kernel->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseKernelFCL (cl_kernel kernel)
	{
		MSG(clReleaseKernelFCL);
		if (!FreeOCL::isValid(kernel))
			return CL_INVALID_KERNEL;
		kernel->release();
		if (kernel->get_ref_count() == 0)
		{
			kernel->invalidate();
			kernel->unlock();
			delete kernel;
		}
		else
			kernel->unlock();
		return CL_SUCCESS;
	}

	cl_int clSetKernelArgFCL (cl_kernel kernel,
						   cl_uint arg_index,
						   size_t arg_size,
						   const void *arg_value)
	{
		MSG(clSetKernelArgFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);

		return CL_SUCCESS;
	}

	cl_int clGetKernelInfoFCL (cl_kernel kernel,
							cl_kernel_info param_name,
							size_t param_value_size,
							void *param_value,
							size_t *param_value_size_ret)
	{
		MSG(clGetKernelInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);

		bool bTooSmall = false;

		switch(param_name)
		{
		case CL_KERNEL_FUNCTION_NAME:	bTooSmall = SET_STRING(kernel->function_name.c_str());	break;
		case CL_KERNEL_NUM_ARGS:
			return CL_INVALID_VALUE;
		case CL_KERNEL_REFERENCE_COUNT:	bTooSmall = SET_VAR(kernel->get_ref_count());	break;
		case CL_KERNEL_CONTEXT:			bTooSmall = SET_VAR(kernel->program->context);	break;
		case CL_KERNEL_PROGRAM:			bTooSmall = SET_VAR(kernel->program);	break;
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clGetKernelWorkGroupInfoFCL (cl_kernel kernel,
									 cl_device_id device,
									 cl_kernel_work_group_info param_name,
									 size_t param_value_size,
									 void *param_value,
									 size_t *param_value_size_ret)
	{
		MSG(clGetKernelWorkGroupInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);

		if (!FreeOCL::isValid(device))
			return CL_INVALID_DEVICE;

		bool bTooSmall = false;

		switch(param_name)
		{
		case CL_KERNEL_WORK_GROUP_SIZE:
		case CL_KERNEL_COMPILE_WORK_GROUP_SIZE:
		case CL_KERNEL_LOCAL_MEM_SIZE:
		case CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE:
		case CL_KERNEL_PRIVATE_MEM_SIZE:
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clEnqueueNDRangeKernelFCL (cl_command_queue command_queue,
								   cl_kernel kernel,
								   cl_uint work_dim,
								   const size_t *global_work_offset,
								   const size_t *global_work_size,
								   const size_t *local_work_size,
								   cl_uint num_events_in_wait_list,
								   const cl_event *event_wait_list,
								   cl_event *event)
	{
		MSG(clEnqueueNDRangeKernelFCL);
		return CL_INVALID_VALUE;
	}

	cl_int clEnqueueTaskFCL (cl_command_queue command_queue,
						  cl_kernel kernel,
						  cl_uint num_events_in_wait_list,
						  const cl_event *event_wait_list,
						  cl_event *event)
	{
		MSG(clEnqueueTaskFCL);
		const size_t global_work_size = 1;
		const size_t local_work_size = 1;
		return clEnqueueNDRangeKernel(command_queue,
									  kernel,
									  1,
									  NULL,
									  &global_work_size,
									  &local_work_size,
									  num_events_in_wait_list,
									  event_wait_list,
									  event);
	}
}
