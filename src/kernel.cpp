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
#include "utils/commandqueue.h"
#include "context.h"
#include "event.h"
#include "mem.h"
#include "program.h"
#include "device.h"
#include "sampler.h"
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include "prototypes.h"

namespace
{
	const size_t kernel_preferred_work_group_size_multiple = 1;

	struct image1d_t
	{
		cl_uint channel_order;
		cl_uint channel_data_type;
		size_t width;
		size_t element_size;
		void *data;
	};

	typedef image1d_t image1d_buffer_t;

	struct image1d_array_t
	{
		cl_uint channel_order;
		cl_uint channel_data_type;
		size_t width, array_size;
		size_t row_pitch;
		size_t element_size;
		void *data;
	};

	struct image2d_t
	{
		cl_uint channel_order;
		cl_uint channel_data_type;
		size_t width, height;
		size_t row_pitch;
		size_t element_size;
		void *data;
	};

	struct image2d_array_t
	{
		cl_uint channel_order;
		cl_uint channel_data_type;
		size_t width, height, array_size;
		size_t row_pitch, slice_pitch;
		size_t element_size;
		void *data;
	};
	struct image3d_t
	{
		cl_uint channel_order;
		cl_uint channel_data_type;
		size_t width, height, depth;
		size_t row_pitch, slice_pitch;
		size_t element_size;
		void *data;
	};
}

#define CLK_NORMALIZED_COORDS_FALSE		0x00000000U
#define CLK_NORMALIZED_COORDS_TRUE		0x01000000U

#define CLK_ADDRESS_NONE				0x00000000U
#define CLK_ADDRESS_CLAMP_TO_EDGE		0x00000001U
#define CLK_ADDRESS_REPEAT				0x00000002U
#define CLK_ADDRESS_CLAMP				0x00000003U
#define CLK_ADDRESS_MIRRORED_REPEAT		0x00000004U

#define CLK_FILTER_NEAREST				0x00000000U
#define CLK_FILTER_LINEAR				0x00010000U

#define SET_STRING(X)	FreeOCL::copy_memory_within_limits(X, strlen(X) + 1, param_value_size, param_value, param_value_size_ret)
#define SET_VAR(X)	FreeOCL::copy_memory_within_limits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
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
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		unlock.handle(command_queue->context);

		FreeOCL::smartptr<FreeOCL::command_native_kernel> cmd = new FreeOCL::command_native_kernel;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;

		cmd->user_func = user_func;
		cmd->args = malloc(cb_args);
		memcpy(cmd->args, args, cb_args);
		for(size_t i = 0 ; i < num_mem_objects ; ++i)
		{
			if (!FreeOCL::is_valid(mem_list[i]))
			{
				free(cmd->args);
				return CL_INVALID_MEM_OBJECT;
			}
			const ptrdiff_t offset = (char*)args_mem_loc[i] - (char*)args;
			*((void**)((char*)cmd->args + offset)) = mem_list[i]->ptr;
			mem_list[i]->unlock();
		}

		cmd->event = (event != NULL) ? new _cl_event(command_queue->context) : NULL;
		if (cmd->event)
		{
			*event = cmd->event.weak();
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_NATIVE_KERNEL;
			cmd->event->status = CL_SUBMITTED;
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
		if (!FreeOCL::is_valid(program))
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
		kernel->__FCL_info = (size_t (*)(size_t, int*, const char **, const char **, int *, int *)) dlsym(program->handle, ("__FCL_info_" + kernel->function_name).c_str());
		kernel->__FCL_init = (bool (*)(const void*,size_t,const size_t*,const size_t *,const size_t*)) dlsym(program->handle, ("__FCL_init_" + kernel->function_name).c_str());
		kernel->__FCL_setwg = (void (*)(char * const,const size_t *, ucontext_t *, ucontext_t *)) dlsym(program->handle, ("__FCL_setwg_" + kernel->function_name).c_str());
		kernel->__FCL_kernel = (void (*)(DUMMYARGS const int)) dlsym(program->handle, ("__FCL_kernel_" + kernel->function_name).c_str());
		program->kernels_attached++;

		if (kernel->__FCL_info == NULL || kernel->__FCL_kernel == NULL)
		{
			delete kernel;
			SET_RET(CL_OUT_OF_RESOURCES);
			return 0;
		}

		size_t offset = 0;
		int type = 0, type_qualifier, type_access_qualifier;
		const char *name, *type_name;
		for(size_t s = kernel->__FCL_info(0, &type, &name, &type_name, &type_qualifier, &type_access_qualifier), i = 1
			; s != 0
			; s = kernel->__FCL_info(i, &type, &name, &type_name, &type_qualifier, &type_access_qualifier), ++i)
		{
			kernel->args_size.push_back(s);
			kernel->args_offset.push_back(offset);
			kernel->args_type.push_back(type);
			kernel->args_access_qualifier.push_back(type_access_qualifier);
			kernel->args_qualifier.push_back(type_qualifier);
			kernel->args_name.push_back(name);
			kernel->args_type_name.push_back(type_name);
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

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(program))
			return CL_INVALID_PROGRAM;
		unlock.handle(program);

		if (program->binary_type != CL_PROGRAM_BINARY_TYPE_EXECUTABLE)
			return CL_INVALID_PROGRAM_EXECUTABLE;

		if (num_kernels_ret)
			*num_kernels_ret = program->kernel_names.size();

		if (kernels)
		{
			if (num_kernels < program->kernel_names.size())
				return CL_INVALID_VALUE;
			if (program->kernel_names.size() == 0)
				return CL_SUCCESS;
			program->retain();
			unlock.forget(program);
			program->unlock();

			size_t idx = 0;
			for(FreeOCL::set<std::string>::const_iterator it = program->kernel_names.begin(), end = program->kernel_names.end()
				; it != end
				; ++it, ++idx)
				kernels[idx] = clCreateKernelFCL(program, it->c_str(), NULL);

			program->release();
		}

		return CL_SUCCESS;
	}

	cl_int clRetainKernelFCL (cl_kernel kernel)
	{
		MSG(clRetainKernelFCL);
		if (!FreeOCL::is_valid(kernel))
			return CL_INVALID_KERNEL;

		kernel->retain();
		kernel->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseKernelFCL (cl_kernel kernel)
	{
		MSG(clReleaseKernelFCL);
		if (!FreeOCL::is_valid(kernel))
			return CL_INVALID_KERNEL;
		kernel->release();
		if (kernel->get_ref_count() == 0)
		{
			kernel->invalidate();
			kernel->unlock();
			clReleaseProgramFCL(kernel->program);
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
		if (!FreeOCL::is_valid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);
		if (kernel->args_size.size() <= arg_index)
			return CL_INVALID_ARG_INDEX;
		switch(kernel->args_type[arg_index])
		{
		case CL_KERNEL_ARG_ADDRESS_GLOBAL:
		case CL_KERNEL_ARG_ADDRESS_CONSTANT:
		case CL_KERNEL_ARG_ADDRESS_PRIVATE:
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
					if (!FreeOCL::is_valid(mem_object))
						return CL_INVALID_MEM_OBJECT;
					unlock.handle(mem_object);
					memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &(mem_object->ptr), arg_size);
				}
			}
			break;
		case CL_KERNEL_ARG_ADDRESS_LOCAL:
			if (arg_value != NULL)
				return CL_INVALID_ARG_VALUE;
			if (arg_size == 0)
				return CL_INVALID_ARG_SIZE;
			memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &arg_size, sizeof(size_t));
			break;
		case CL_UNORM_INT_101010:
			{
				if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
					return CL_INVALID_ARG_VALUE;
				if (arg_size != sizeof(cl_sampler))
					return CL_INVALID_ARG_SIZE;
				cl_sampler sampler = *(cl_sampler*)arg_value;
				if (!FreeOCL::is_valid(sampler))
					return CL_INVALID_SAMPLER;
				unlock.handle(sampler);
				cl_uint sampler_value = 0;
				if (sampler->normalized_coords)
					sampler_value |= CLK_NORMALIZED_COORDS_TRUE;
				else
					sampler_value |= CLK_NORMALIZED_COORDS_FALSE;

				switch(sampler->addressing_mode)
				{
				case CL_ADDRESS_MIRRORED_REPEAT:	sampler_value |= CLK_ADDRESS_MIRRORED_REPEAT;	break;
				case CL_ADDRESS_NONE:				sampler_value |= CLK_ADDRESS_NONE;	break;
				case CL_ADDRESS_CLAMP_TO_EDGE:		sampler_value |= CLK_ADDRESS_CLAMP_TO_EDGE;	break;
				case CL_ADDRESS_CLAMP:				sampler_value |= CLK_ADDRESS_CLAMP;	break;
				case CL_ADDRESS_REPEAT:				sampler_value |= CLK_ADDRESS_REPEAT;	break;
				}

				switch(sampler->filter_mode)
				{
				case CL_FILTER_LINEAR:	sampler_value |= CLK_FILTER_LINEAR;	break;
				case CL_FILTER_NEAREST:	sampler_value |= CLK_FILTER_NEAREST;	break;
				}

				*(cl_uint*)&(kernel->args_buffer[kernel->args_offset[arg_index]]) = sampler_value;
			}
			break;
		case CL_MEM_OBJECT_IMAGE1D:
			if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
				return CL_INVALID_ARG_VALUE;
			else
			{
				if (arg_size != sizeof(cl_mem))
					return CL_INVALID_ARG_SIZE;
				cl_mem image = *(cl_mem*)arg_value;
				if (!FreeOCL::is_valid(image))
					return CL_INVALID_MEM_OBJECT;
				unlock.handle(image);
				if (image->mem_type != CL_MEM_OBJECT_IMAGE1D)
					return CL_INVALID_MEM_OBJECT;

				image1d_t img;
				img.channel_data_type = image->image_format.image_channel_data_type;
				img.channel_order = image->image_format.image_channel_order;
				img.width = image->width;
				img.element_size = image->element_size;
				img.data = image->ptr;

				memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &img, sizeof(img));
			}
			break;
		case CL_MEM_OBJECT_IMAGE1D_BUFFER:
			if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
				return CL_INVALID_ARG_VALUE;
			else
			{
				if (arg_size != sizeof(cl_mem))
					return CL_INVALID_ARG_SIZE;
				cl_mem image = *(cl_mem*)arg_value;
				if (!FreeOCL::is_valid(image))
					return CL_INVALID_MEM_OBJECT;
				unlock.handle(image);
				if (image->mem_type != CL_MEM_OBJECT_IMAGE1D_BUFFER)
					return CL_INVALID_MEM_OBJECT;

				image1d_buffer_t img;
				img.channel_data_type = image->image_format.image_channel_data_type;
				img.channel_order = image->image_format.image_channel_order;
				img.width = image->width;
				img.element_size = image->element_size;
				img.data = image->ptr;

				memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &img, sizeof(img));
			}
			break;
		case CL_MEM_OBJECT_IMAGE1D_ARRAY:
			if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
				return CL_INVALID_ARG_VALUE;
			else
			{
				if (arg_size != sizeof(cl_mem))
					return CL_INVALID_ARG_SIZE;
				cl_mem image = *(cl_mem*)arg_value;
				if (!FreeOCL::is_valid(image))
					return CL_INVALID_MEM_OBJECT;
				unlock.handle(image);
				if (image->mem_type != CL_MEM_OBJECT_IMAGE1D_ARRAY)
					return CL_INVALID_MEM_OBJECT;

				image1d_array_t img;
				img.channel_data_type = image->image_format.image_channel_data_type;
				img.channel_order = image->image_format.image_channel_order;
				img.width = image->width;
				img.array_size = image->height;
				img.row_pitch = image->row_pitch;
				img.element_size = image->element_size;
				img.data = image->ptr;

				memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &img, sizeof(img));
			}
			break;
		case CL_MEM_OBJECT_IMAGE2D:
			if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
				return CL_INVALID_ARG_VALUE;
			else
			{
				if (arg_size != sizeof(cl_mem))
					return CL_INVALID_ARG_SIZE;
				cl_mem image = *(cl_mem*)arg_value;
				if (!FreeOCL::is_valid(image))
					return CL_INVALID_MEM_OBJECT;
				unlock.handle(image);
				if (image->mem_type != CL_MEM_OBJECT_IMAGE2D)
					return CL_INVALID_MEM_OBJECT;

				image2d_t img;
				img.channel_data_type = image->image_format.image_channel_data_type;
				img.channel_order = image->image_format.image_channel_order;
				img.width = image->width;
				img.height = image->height;
				img.row_pitch = image->row_pitch;
				img.element_size = image->element_size;
				img.data = image->ptr;

				memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &img, sizeof(img));
			}
			break;
		case CL_MEM_OBJECT_IMAGE2D_ARRAY:
			if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
				return CL_INVALID_ARG_VALUE;
			else
			{
				if (arg_size != sizeof(cl_mem))
					return CL_INVALID_ARG_SIZE;
				cl_mem image = *(cl_mem*)arg_value;
				if (!FreeOCL::is_valid(image))
					return CL_INVALID_MEM_OBJECT;
				unlock.handle(image);
				if (image->mem_type != CL_MEM_OBJECT_IMAGE2D_ARRAY)
					return CL_INVALID_MEM_OBJECT;

				image2d_array_t img;
				img.channel_data_type = image->image_format.image_channel_data_type;
				img.channel_order = image->image_format.image_channel_order;
				img.width = image->width;
				img.height = image->height;
				img.array_size = image->depth;
				img.row_pitch = image->row_pitch;
				img.slice_pitch = image->slice_pitch;
				img.element_size = image->element_size;
				img.data = image->ptr;

				memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &img, sizeof(img));
			}
			break;
		case CL_MEM_OBJECT_IMAGE3D:
			if (arg_value == NULL || *(cl_mem*)arg_value == NULL)
				return CL_INVALID_ARG_VALUE;
			else
			{
				if (arg_size != sizeof(cl_mem))
					return CL_INVALID_ARG_SIZE;
				cl_mem image = *(cl_mem*)arg_value;
				if (!FreeOCL::is_valid(image))
					return CL_INVALID_MEM_OBJECT;
				unlock.handle(image);
				if (image->mem_type != CL_MEM_OBJECT_IMAGE3D)
					return CL_INVALID_MEM_OBJECT;

				image3d_t img;
				img.channel_data_type = image->image_format.image_channel_data_type;
				img.channel_order = image->image_format.image_channel_order;
				img.width = image->width;
				img.height = image->height;
				img.depth = image->depth;
				img.row_pitch = image->row_pitch;
				img.slice_pitch = image->slice_pitch;
				img.element_size = image->element_size;
				img.data = image->ptr;

				memcpy(&(kernel->args_buffer[kernel->args_offset[arg_index]]), &img, sizeof(img));
			}
			break;
		default:
			if (kernel->args_size[arg_index] != arg_size)
				return CL_INVALID_ARG_SIZE;
			if (arg_value == NULL)
				return CL_INVALID_ARG_VALUE;
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
		if (!FreeOCL::is_valid(kernel))
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
		case CL_KERNEL_ATTRIBUTES:
			//! \todo implement kernel attributes
			bTooSmall = SET_VAR("");
			break;
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
		if (!FreeOCL::is_valid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);

		if (!FreeOCL::is_valid(device))
			return CL_INVALID_DEVICE;

		bool bTooSmall = false;

		switch(param_name)
		{
		case CL_KERNEL_GLOBAL_WORK_SIZE:
			{
				const size_t val[3] = {0x1000, 0x1000, 0x1000};
				bTooSmall = FreeOCL::copy_memory_within_limits(val, sizeof(val), param_value_size, param_value, param_value_size_ret);
			}
			break;
		case CL_KERNEL_WORK_GROUP_SIZE:
			{
				const size_t val = 1024;
				bTooSmall = SET_VAR(val);
			}
			break;
		case CL_KERNEL_COMPILE_WORK_GROUP_SIZE:
			{
				const size_t val[3] = {0, 0, 0};
				bTooSmall = FreeOCL::copy_memory_within_limits(val, sizeof(val), param_value_size, param_value, param_value_size_ret);
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

		if ((num_events_in_wait_list > 0 && event_wait_list == NULL)
			|| (num_events_in_wait_list == 0 && event_wait_list != NULL))
			return CL_INVALID_EVENT_WAIT_LIST;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);

		size_t p_local_work_size[3];
		if (local_work_size == NULL)
		{
			p_local_work_size[0] = 1;
			p_local_work_size[1] = 1;
			p_local_work_size[2] = 1;
			local_work_size = p_local_work_size;
		}

		FreeOCL::smartptr<FreeOCL::command_ndrange_kernel> cmd = new FreeOCL::command_ndrange_kernel;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = NULL;
		cmd->dim = work_dim;
		cmd->kernel = kernel;
		for(size_t i = work_dim ; i < 3 ; ++i)
		{
			cmd->global_size[i] = 1;
			cmd->global_offset[i] = 0;
			cmd->local_size[i] = 1;
		}
		for(size_t i = 0 ; i < work_dim ; ++i)
		{
			cmd->global_size[i] = global_work_size[i];
			cmd->global_offset[i] = global_work_offset ? global_work_offset[i] : 0;
			cmd->local_size[i] = local_work_size[i];
		}
		if (kernel->args_buffer.empty())
			cmd->args = NULL;
		else
		{
			cmd->args = malloc(kernel->args_buffer.size());
			memcpy(cmd->args, &(kernel->args_buffer.front()), kernel->args_buffer.size());
		}

		if (event)
		{
			cmd->event = *event = new _cl_event(command_queue->context);
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_NDRANGE_KERNEL;
			cmd->event->status = CL_SUBMITTED;
		}

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

	CL_API_ENTRY cl_int CL_API_CALL	clGetKernelArgInfoFCL(cl_kernel       kernel,
														  cl_uint         arg_indx,
														  cl_kernel_arg_info  param_name,
														  size_t          param_value_size,
														  void *          param_value,
														  size_t *        param_value_size_ret) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clGetKernelArgInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(kernel))
			return CL_INVALID_KERNEL;
		unlock.handle(kernel);

		if (kernel->args_type.size() <= arg_indx)
			return CL_INVALID_ARG_INDEX;

		bool bTooSmall = false;

		switch(param_name)
		{
		case CL_KERNEL_ARG_ADDRESS_QUALIFIER:
			switch(kernel->args_type[arg_indx])
			{
			case CL_KERNEL_ARG_ADDRESS_CONSTANT:
			case CL_KERNEL_ARG_ADDRESS_LOCAL:
			case CL_KERNEL_ARG_ADDRESS_GLOBAL:
			case CL_KERNEL_ARG_ADDRESS_PRIVATE:
				bTooSmall = SET_VAR(kernel->args_type[arg_indx]);
				break;
			default:
				{
					cl_kernel_arg_address_qualifier tmp = CL_KERNEL_ARG_ADDRESS_PRIVATE;
					bTooSmall = SET_VAR(tmp);
				}
				break;
			}
			break;
		case CL_KERNEL_ARG_ACCESS_QUALIFIER:	bTooSmall = SET_VAR(kernel->args_access_qualifier[arg_indx]);	break;
		case CL_KERNEL_ARG_TYPE_NAME:			bTooSmall = SET_STRING(kernel->args_type_name[arg_indx].c_str());	break;
		case CL_KERNEL_ARG_NAME:				bTooSmall = SET_STRING(kernel->args_name[arg_indx].c_str());	break;
		case CL_KERNEL_ARG_TYPE_QUALIFIER:		bTooSmall = SET_VAR(kernel->args_qualifier[arg_indx]);	break;
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
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
	if (FreeOCL::is_valid(program))
	{
		program->kernels_attached--;
		program->unlock();
	}
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_kernels.erase(this);
	FreeOCL::global_mutex.unlock();
}
