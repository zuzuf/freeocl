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
#include "kernel.h"
#include "commandqueue.h"
#include "context.h"
#include "event.h"
#include "mem.h"
#include "program.h"
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>

namespace
{
	const size_t kernel_preferred_work_group_size_multiple = 1;
}

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

		cmd.common.event = (event != NULL) ? new _cl_event(command_queue->context) : NULL;
		if (cmd.common.event)
		{
			*event = cmd.common.event;
			cmd.common.event->command_queue = command_queue;
			cmd.common.event->command_type = CL_COMMAND_NATIVE_KERNEL;
			cmd.common.event->status = CL_SUBMITTED;
			cmd.common.event->retain();
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

		if (kernel_name == NULL)
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(program))
		{
			SET_RET(CL_INVALID_PROGRAM);
			return 0;
		}
		unlock.handle(program);

		if (program->build_status != CL_BUILD_SUCCESS)
		{
			SET_RET(CL_INVALID_PROGRAM_EXECUTABLE);
			return 0;
		}

		if (program->kernel_names.count(kernel_name) == 0)
		{
			SET_RET(CL_INVALID_KERNEL_NAME);
			return 0;
		}

		cl_kernel kernel = new _cl_kernel;
		kernel->program = program;
		kernel->function_name = kernel_name;
		kernel->__FCL_info = (size_t (*)(size_t,int*)) dlsym(program->handle, ("__FCL_info_" + kernel->function_name).c_str());
		kernel->__FCL_kernel = (void (*)(const void*,size_t,size_t*,size_t*,size_t*)) dlsym(program->handle, ("__FCL_kernel_" + kernel->function_name).c_str());

		if (kernel->__FCL_info == NULL || kernel->__FCL_kernel == NULL)
		{
			delete kernel;
			SET_RET(CL_OUT_OF_RESOURCES);
			return 0;
		}

		size_t offset = 0;
		int type = 0;
		for(size_t s = kernel->__FCL_info(0, &type), i = 1 ; s != 0 ; s = kernel->__FCL_info(i, &type), ++i)
		{
			kernel->args_size.push_back(s);
			kernel->args_offset.push_back(offset);
			kernel->args_type.push_back(type);
			offset += s;
		}
		kernel->args_buffer.resize(offset);

		program->retain();

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
			clReleaseProgram(kernel->program);
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
		if (kernel->args_size.size() <= arg_index)
			return CL_INVALID_ARG_INDEX;
		switch(kernel->args_type[arg_index])
		{
		case CL_GLOBAL:
			{
				if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
				{
					memset(&(kernel->args_buffer[kernel->args_offset[arg_index]]), 0, sizeof(size_t));
				}
				else
				{
					if (arg_size != sizeof(cl_mem))
						return CL_INVALID_ARG_SIZE;
					cl_mem mem_object = *(cl_mem*)arg_value;
					if (!FreeOCL::isValid(mem_object))
						return CL_INVALID_MEM_OBJECT;
					unlock.handle(mem_object);
					memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &(mem_object->ptr), arg_size);
				}
			}
			break;
		case CL_LOCAL:
			if (arg_value != NULL || arg_size == 0)
				return CL_INVALID_ARG_VALUE;
			memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &arg_size, sizeof(size_t));
			break;
		default:
			if (kernel->args_size[arg_index] != arg_size)
				return CL_INVALID_ARG_SIZE;
			memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), arg_value, arg_size);
		}

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
			{
				const cl_uint num = cl_uint(kernel->args_size.size());
				bTooSmall = SET_VAR(num);
			}
			break;
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
			{
				const size_t val = 1024;
				bTooSmall = SET_VAR(val);
			}
			break;
		case CL_KERNEL_COMPILE_WORK_GROUP_SIZE:
			{
				const size_t val[3] = {0, 0, 0};
				bTooSmall = FreeOCL::copyMemoryWithinLimits(val, sizeof(val), param_value_size, param_value, param_value_size_ret);
			}
			break;
		case CL_KERNEL_LOCAL_MEM_SIZE:
			{
				const cl_ulong val = 0x1000;
				bTooSmall = SET_VAR(val);
			}
			break;
		case CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE:
			bTooSmall = SET_VAR(kernel_preferred_work_group_size_multiple);
			break;
		case CL_KERNEL_PRIVATE_MEM_SIZE:
			{
				const cl_ulong val = 0x10000;
				bTooSmall = SET_VAR(val);
			}
			break;
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

		if (work_dim < 1 || work_dim > 3)
			return CL_INVALID_WORK_DIMENSION;

		if (global_work_size == NULL)
			return CL_INVALID_GLOBAL_WORK_SIZE;

		if (local_work_size == NULL)
			return CL_INVALID_WORK_DIMENSION;

		if ((num_events_in_wait_list > 0 && event_wait_list == NULL)
			|| (num_events_in_wait_list == 0 && event_wait_list != NULL))
			return CL_INVALID_EVENT_WAIT_LIST;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::isValid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);

		FreeOCL::command cmd;
		cmd.type = CL_COMMAND_NDRANGE_KERNEL;
		cmd.common.num_events_in_wait_list = num_events_in_wait_list;
		cmd.common.event_wait_list = event_wait_list;
		cmd.common.event = NULL;
		cmd.ndrange_kernel.dim = work_dim;
		cmd.ndrange_kernel.kernel = kernel;
		for(size_t i = work_dim ; i < 3 ; ++i)
		{
			cmd.ndrange_kernel.global_size[i] = 1;
			cmd.ndrange_kernel.global_offset[i] = 0;
			cmd.ndrange_kernel.local_size[i] = 1;
		}
		for(size_t i = 0 ; i < work_dim ; ++i)
		{
			cmd.ndrange_kernel.global_size[i] = global_work_size[i];
			cmd.ndrange_kernel.global_offset[i] = global_work_offset ? global_work_offset[i] : 0;
			cmd.ndrange_kernel.local_size[i] = local_work_size[i];
		}
		if (kernel->args_buffer.empty())
			cmd.ndrange_kernel.args = NULL;
		else
		{
			cmd.ndrange_kernel.args = malloc(kernel->args_buffer.size());
			memcpy(cmd.ndrange_kernel.args, &(kernel->args_buffer.front()), kernel->args_buffer.size());
		}

		if (event)
		{
			cmd.common.event = *event = new _cl_event(command_queue->context);
			cmd.common.event->command_queue = command_queue;
			cmd.common.event->command_type = CL_COMMAND_NDRANGE_KERNEL;
			cmd.common.event->status = CL_SUBMITTED;
			cmd.common.event->retain();
		}

		kernel->retain();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		return CL_SUCCESS;
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
		return clEnqueueNDRangeKernelFCL(command_queue,
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

_cl_kernel::_cl_kernel()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_kernels.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_kernel::~_cl_kernel()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_kernels.erase(this);
	FreeOCL::global_mutex.unlock();
}
