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

#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_mem clCreateImage2D (cl_context context,
							cl_mem_flags flags,
							const cl_image_format *image_format,
							size_t image_width,
							size_t image_height,
							size_t image_row_pitch,
							void *host_ptr,
							cl_int *errcode_ret)
	{
		SET_RET(CL_INVALID_OPERATION);
		return 0;
	}

	cl_mem clCreateImage3D (cl_context context,
							cl_mem_flags flags,
							const cl_image_format *image_format,
							size_t image_width,
							size_t image_height,
							size_t image_depth,
							size_t image_row_pitch,
							size_t image_slice_pitch,
							void *host_ptr,
							cl_int *errcode_ret)
	{
		SET_RET(CL_INVALID_OPERATION);
		return 0;
	}

	cl_int clGetSupportedImageFormats (cl_context context,
									   cl_mem_flags flags,
									   cl_mem_object_type image_type,
									   cl_uint num_entries,
									   cl_image_format *image_formats,
									   cl_uint *num_image_formats)
	{
		if (image_type != CL_MEM_OBJECT_IMAGE2D
			&& image_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_VALUE;

		if (num_entries == 0 && image_formats != NULL)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(context))
			return CL_INVALID_CONTEXT;
		unlock.handle(context);

		if (num_image_formats != NULL)
			*num_image_formats = 0;

		return CL_SUCCESS;
	}

	cl_int clEnqueueReadImage (cl_command_queue command_queue,
							   cl_mem image,
							   cl_bool blocking_read,
							   const size_t origin[3],
							   const size_t region[3],
							   size_t row_pitch,
							   size_t slice_pitch,
							   void *ptr,
							   cl_uint num_events_in_wait_list,
							   const cl_event *event_wait_list,
							   cl_event *event)
	{
		return CL_INVALID_MEM_OBJECT;
	}

	cl_int clEnqueueWriteImage (cl_command_queue command_queue,
								cl_mem image,
								cl_bool blocking_write,
								const size_t origin[3],
								const size_t region[3],
								size_t input_row_pitch,
								size_t input_slice_pitch,
								const void * ptr,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		return CL_INVALID_MEM_OBJECT;
	}

	cl_int clEnqueueCopyImage (cl_command_queue command_queue,
							   cl_mem src_image,
							   cl_mem dst_image,
							   const size_t src_origin[3],
							   const size_t dst_origin[3],
							   const size_t region[3],
							   cl_uint num_events_in_wait_list,
							   const cl_event *event_wait_list,
							   cl_event *event)
	{
		return CL_INVALID_MEM_OBJECT;
	}

	cl_int clEnqueueCopyImageToBuffer (cl_command_queue command_queue,
									   cl_mem src_image,
									   cl_mem dst_buffer,
									   const size_t src_origin[3],
									   const size_t region[3],
									   size_t dst_offset,
									   cl_uint num_events_in_wait_list,
									   const cl_event *event_wait_list,
									   cl_event *event)
	{
		return CL_INVALID_MEM_OBJECT;
	}

	cl_int clEnqueueCopyBufferToImage (cl_command_queue command_queue,
									   cl_mem src_buffer,
									   cl_mem dst_image,
									   size_t src_offset,
									   const size_t dst_origin[3],
									   const size_t region[3],
									   cl_uint num_events_in_wait_list,
									   const cl_event *event_wait_list,
									   cl_event *event)
	{
		return CL_INVALID_MEM_OBJECT;
	}

	void *clEnqueueMapImage (cl_command_queue command_queue,
							  cl_mem image,
							  cl_bool blocking_map,
							  cl_map_flags map_flags,
							  const size_t origin[3],
							  const size_t region[3],
							  size_t *image_row_pitch,
							  size_t *image_slice_pitch,
							  cl_uint num_events_in_wait_list,
							  const cl_event *event_wait_list,
							  cl_event *event,
							  cl_int *errcode_ret)
	{
		SET_RET(CL_INVALID_MEM_OBJECT);
		return NULL;
	}

	cl_int clGetImageInfo (cl_mem image,
						   cl_image_info param_name,
						   size_t param_value_size,
						   void *param_value,
						   size_t *param_value_size_ret)
	{
		return CL_INVALID_MEM_OBJECT;
	}
}
