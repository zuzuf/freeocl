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
#include "device.h"
#include <cstring>
#include "prototypes.h"

#define SET_VAR(X)	FreeOCL::copy_memory_within_limits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_mem clCreateImageCommonFCL (cl_context context,
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
		MSG(clCreateImageCommonFCL);
		if (image_format == NULL)
		{
			SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
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
		if (host_ptr == NULL && (image_row_pitch != 0 || image_slice_pitch != 0))
		{
			SET_RET(CL_INVALID_IMAGE_SIZE);
			return 0;
		}

		size_t channels = 0;
		size_t data_size = 0;
		switch(image_format->image_channel_order)
		{
		case CL_RGBA:
			channels = 4;
			break;
		case CL_BGRA:
		case CL_ARGB:
			channels = 4;
			switch(image_format->image_channel_data_type)
			{
			case CL_UNORM_INT8:
			case CL_SNORM_INT8:
			case CL_SIGNED_INT8:
			case CL_UNSIGNED_INT8:
				break;
			default:
				SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
				return 0;
			}
			break;
		case CL_R:
		case CL_A:
			channels = 1;
			break;
		case CL_Rx:
		case CL_RG:
		case CL_RA:
			channels = 2;
			break;
		case CL_RGx:
			channels = 3;
			break;
		case CL_INTENSITY:
		case CL_LUMINANCE:
			channels = 1;
			switch(image_format->image_channel_data_type)
			{
			case CL_UNORM_INT8:
			case CL_UNORM_INT16:
			case CL_SNORM_INT8:
			case CL_SNORM_INT16:
			case CL_HALF_FLOAT:
			case CL_FLOAT:
				break;
			default:
				SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
				return 0;
			}
			break;
		case CL_RGB:
		case CL_RGBx:
			channels = (image_format->image_channel_order == CL_RGB) ? 3 : 4;
			switch(image_format->image_channel_data_type)
			{
			case CL_UNORM_SHORT_555:
			case CL_UNORM_SHORT_565:
			case CL_UNORM_INT_101010:
				break;
			default:
				SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
				return 0;
			}
			break;
		default:
			SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
			return 0;
		}

		switch(image_format->image_channel_data_type)
		{
		case CL_UNORM_INT8:		data_size = 1;	break;
		case CL_UNORM_INT16:	data_size = 2;	break;
		case CL_SIGNED_INT8:	data_size = 1;	break;
		case CL_SIGNED_INT16:	data_size = 2;	break;
		case CL_SIGNED_INT32:	data_size = 4;	break;
		case CL_UNSIGNED_INT8:	data_size = 1;	break;
		case CL_UNSIGNED_INT16:	data_size = 2;	break;
		case CL_UNSIGNED_INT32:	data_size = 4;	break;
		case CL_HALF_FLOAT:		data_size = 2;	break;
		case CL_FLOAT:			data_size = 4;	break;

		case CL_SNORM_INT8:			data_size = 1;	break;
		case CL_SNORM_INT16:		data_size = 2;	break;
		case CL_UNORM_SHORT_565:	data_size = 2;	break;
		case CL_UNORM_SHORT_555:	data_size = 2;	break;
		case CL_UNORM_INT_101010:	data_size = 4;	break;
		default:
			SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
			return 0;
		}

		const size_t element_size = image_format->image_channel_data_type == CL_UNORM_SHORT_555
									|| image_format->image_channel_data_type == CL_UNORM_SHORT_565
									|| image_format->image_channel_data_type == CL_UNORM_INT_101010
									? data_size
									: channels * data_size;

		if (image_row_pitch == 0)
			image_row_pitch = image_width * element_size;
		else if (image_row_pitch < image_width * element_size || (image_row_pitch % element_size) != 0)
		{
			SET_RET(CL_INVALID_IMAGE_SIZE);
			return 0;
		}

		if (image_slice_pitch == 0)
			image_slice_pitch = image_row_pitch * image_height;
		else if (image_slice_pitch < image_row_pitch * image_height || (image_slice_pitch % image_row_pitch) != 0)
		{
			SET_RET(CL_INVALID_IMAGE_SIZE);
			return 0;
		}

		const size_t size = image_depth * image_slice_pitch;

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
		mem->mem_type = CL_MEM_OBJECT_IMAGE3D;
		mem->host_ptr = host_ptr;
		mem->parent = NULL;
		mem->offset = 0;
		mem->width = image_width;
		mem->height = image_height;
		mem->depth = image_depth;
		mem->row_pitch = image_row_pitch;
		mem->slice_pitch = image_slice_pitch;
		mem->element_size = element_size;
		mem->image_format = *image_format;
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

	cl_mem clCreateImage2DFCL (cl_context context,
							cl_mem_flags flags,
							const cl_image_format *image_format,
							size_t image_width,
							size_t image_height,
							size_t image_row_pitch,
							void *host_ptr,
							cl_int *errcode_ret)
	{
		MSG(clCreateImage2DFCL);
		if (image_width == 0
				|| image_height == 0
				|| image_width > FreeOCL::device->image2d_max_width
				|| image_height > FreeOCL::device->image2d_max_height)
		{
			SET_RET(CL_INVALID_IMAGE_SIZE);
			return 0;
		}
		cl_mem mem = clCreateImageCommonFCL(context,
											flags,
											image_format,
											image_width,
											image_height,
											1,
											image_row_pitch,
											0,
											host_ptr,
											errcode_ret);
		if (mem)
			mem->mem_type = CL_MEM_OBJECT_IMAGE2D;
		return mem;
	}

	cl_mem clCreateImage3DFCL (cl_context context,
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
		MSG(clCreateImage3DFCL);
		if (image_width == 0
				|| image_height == 0
				|| image_depth <= 1
				|| image_width > FreeOCL::device->image3d_max_width
				|| image_height > FreeOCL::device->image3d_max_height
				|| image_depth > FreeOCL::device->image3d_max_depth)
		{
			SET_RET(CL_INVALID_IMAGE_SIZE);
			return 0;
		}
		cl_mem mem = clCreateImageCommonFCL(context,
											flags,
											image_format,
											image_width,
											image_height,
											image_depth,
											image_row_pitch,
											image_slice_pitch,
											host_ptr,
											errcode_ret);
		if (mem)
			mem->mem_type = CL_MEM_OBJECT_IMAGE3D;
		return mem;
	}

	cl_int clGetSupportedImageFormatsFCL (cl_context context,
									   cl_mem_flags flags,
									   cl_mem_object_type image_type,
									   cl_uint num_entries,
									   cl_image_format *image_formats,
									   cl_uint *num_image_formats)
	{
		MSG(clGetSupportedImageFormatsFCL);
		if (image_type != CL_MEM_OBJECT_IMAGE2D
			&& image_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_VALUE;

		if (num_entries == 0 && image_formats != NULL)
			return CL_INVALID_VALUE;

		if ((flags & CL_MEM_USE_HOST_PTR) && (flags & (CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR)))
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(context))
			return CL_INVALID_CONTEXT;
		unlock.handle(context);

		static _cl_image_format supported_formats[] = {
			{CL_RGBA, CL_UNORM_INT8}, {CL_RGBA, CL_UNORM_INT16},
			{CL_RGBA, CL_SNORM_INT8}, {CL_RGBA, CL_SNORM_INT16},
			{CL_RGBA, CL_SIGNED_INT8}, {CL_RGBA, CL_SIGNED_INT16}, {CL_RGBA, CL_SIGNED_INT32},
			{CL_RGBA, CL_UNSIGNED_INT8}, {CL_RGBA, CL_UNSIGNED_INT16}, {CL_RGBA, CL_UNSIGNED_INT32},
			{CL_RGBA, CL_HALF_FLOAT}, {CL_RGBA, CL_FLOAT},

			{CL_BGRA, CL_UNORM_INT8}, {CL_BGRA, CL_SNORM_INT8}, {CL_BGRA, CL_SIGNED_INT8}, {CL_BGRA, CL_UNSIGNED_INT8},
			{CL_ARGB, CL_UNORM_INT8}, {CL_ARGB, CL_SNORM_INT8}, {CL_ARGB, CL_SIGNED_INT8}, {CL_ARGB, CL_UNSIGNED_INT8},

			{CL_R, CL_UNORM_INT8}, {CL_R, CL_UNORM_INT16},
			{CL_R, CL_SNORM_INT8}, {CL_R, CL_SNORM_INT16},
			{CL_R, CL_SIGNED_INT8}, {CL_R, CL_SIGNED_INT16}, {CL_R, CL_SIGNED_INT32},
			{CL_R, CL_UNSIGNED_INT8}, {CL_R, CL_UNSIGNED_INT16}, {CL_R, CL_UNSIGNED_INT32},
			{CL_R, CL_HALF_FLOAT}, {CL_R, CL_FLOAT},

			{CL_A, CL_UNORM_INT8}, {CL_A, CL_UNORM_INT16},
			{CL_A, CL_SNORM_INT8}, {CL_A, CL_SNORM_INT16},
			{CL_A, CL_SIGNED_INT8}, {CL_A, CL_SIGNED_INT16}, {CL_A, CL_SIGNED_INT32},
			{CL_A, CL_UNSIGNED_INT8}, {CL_A, CL_UNSIGNED_INT16}, {CL_A, CL_UNSIGNED_INT32},
			{CL_A, CL_HALF_FLOAT}, {CL_A, CL_FLOAT},

			{CL_Rx, CL_UNORM_INT8}, {CL_Rx, CL_UNORM_INT16},
			{CL_Rx, CL_SNORM_INT8}, {CL_Rx, CL_SNORM_INT16},
			{CL_Rx, CL_SIGNED_INT8}, {CL_Rx, CL_SIGNED_INT16}, {CL_Rx, CL_SIGNED_INT32},
			{CL_Rx, CL_UNSIGNED_INT8}, {CL_Rx, CL_UNSIGNED_INT16}, {CL_Rx, CL_UNSIGNED_INT32},
			{CL_Rx, CL_HALF_FLOAT}, {CL_Rx, CL_FLOAT},

			{CL_RG, CL_UNORM_INT8}, {CL_RG, CL_UNORM_INT16},
			{CL_RG, CL_SNORM_INT8}, {CL_RG, CL_SNORM_INT16},
			{CL_RG, CL_SIGNED_INT8}, {CL_RG, CL_SIGNED_INT16}, {CL_RG, CL_SIGNED_INT32},
			{CL_RG, CL_UNSIGNED_INT8}, {CL_RG, CL_UNSIGNED_INT16}, {CL_RG, CL_UNSIGNED_INT32},
			{CL_RG, CL_HALF_FLOAT}, {CL_RG, CL_FLOAT},

			{CL_RA, CL_UNORM_INT8}, {CL_RA, CL_UNORM_INT16},
			{CL_RA, CL_SNORM_INT8}, {CL_RA, CL_SNORM_INT16},
			{CL_RA, CL_SIGNED_INT8}, {CL_RA, CL_SIGNED_INT16}, {CL_RA, CL_SIGNED_INT32},
			{CL_RA, CL_UNSIGNED_INT8}, {CL_RA, CL_UNSIGNED_INT16}, {CL_RA, CL_UNSIGNED_INT32},
			{CL_RA, CL_HALF_FLOAT}, {CL_RA, CL_FLOAT},

			{CL_RGx, CL_UNORM_INT8}, {CL_RGx, CL_UNORM_INT16},
			{CL_RGx, CL_SNORM_INT8}, {CL_RGx, CL_SNORM_INT16},
			{CL_RGx, CL_SIGNED_INT8}, {CL_RGx, CL_SIGNED_INT16}, {CL_RGx, CL_SIGNED_INT32},
			{CL_RGx, CL_UNSIGNED_INT8}, {CL_RGx, CL_UNSIGNED_INT16}, {CL_RGx, CL_UNSIGNED_INT32},
			{CL_RGx, CL_HALF_FLOAT}, {CL_RGx, CL_FLOAT},

			{CL_LUMINANCE, CL_UNORM_INT8}, {CL_LUMINANCE, CL_UNORM_INT16},
			{CL_LUMINANCE, CL_SNORM_INT8}, {CL_LUMINANCE, CL_SNORM_INT16},
			{CL_LUMINANCE, CL_SIGNED_INT8}, {CL_LUMINANCE, CL_SIGNED_INT16}, {CL_LUMINANCE, CL_SIGNED_INT32},
			{CL_LUMINANCE, CL_UNSIGNED_INT8}, {CL_LUMINANCE, CL_UNSIGNED_INT16}, {CL_LUMINANCE, CL_UNSIGNED_INT32},
			{CL_LUMINANCE, CL_HALF_FLOAT}, {CL_LUMINANCE, CL_FLOAT},

			{CL_INTENSITY, CL_UNORM_INT8}, {CL_INTENSITY, CL_UNORM_INT16},
			{CL_INTENSITY, CL_SNORM_INT8}, {CL_INTENSITY, CL_SNORM_INT16},
			{CL_INTENSITY, CL_SIGNED_INT8}, {CL_INTENSITY, CL_SIGNED_INT16}, {CL_INTENSITY, CL_SIGNED_INT32},
			{CL_INTENSITY, CL_UNSIGNED_INT8}, {CL_INTENSITY, CL_UNSIGNED_INT16}, {CL_INTENSITY, CL_UNSIGNED_INT32},
			{CL_INTENSITY, CL_HALF_FLOAT}, {CL_INTENSITY, CL_FLOAT},
		};
		const size_t num_supported_formats = sizeof(supported_formats) / sizeof(*supported_formats);

		if (num_image_formats != NULL)
			*num_image_formats = num_supported_formats;

		if (image_formats)
			memcpy(image_formats,
				   supported_formats,
				   sizeof(*image_formats) * std::min<size_t>(num_entries, num_supported_formats));

		return CL_SUCCESS;
	}

	cl_int clEnqueueReadImageFCL (cl_command_queue command_queue,
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
		MSG(clEnqueueReadImageFCL);

		if (ptr == NULL
				|| region[0] == 0
				|| region[1] == 0
				|| region[2] == 0)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(image))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(image);

		if (image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& image->mem_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_MEM_OBJECT;

		if (image->mem_type == CL_MEM_OBJECT_IMAGE2D
				&& (origin[2] != 0 || region[2] != 1))
			return CL_INVALID_VALUE;

		if (image->width < origin[0] + region[0]
				|| image->height < origin[1] + region[1]
				|| image->depth < origin[2] + region[2])
			return CL_INVALID_VALUE;

		if (row_pitch == 0)	row_pitch = image->width * image->element_size;
		if (slice_pitch == 0)	slice_pitch = image->height * image->row_pitch;

		if (row_pitch < image->width * image->element_size
				|| slice_pitch < image->row_pitch * image->height)
			return CL_INVALID_VALUE;

		if (blocking_read == CL_TRUE)
		{
			for(size_t i = 0 ; i < num_events_in_wait_list ; ++i)
				if (event_wait_list[i]->status < 0)
					return CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
		}

		FreeOCL::smartptr<FreeOCL::command_read_image> cmd = new FreeOCL::command_read_image;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = (blocking_read == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = image;
		cmd->offset = origin[0] * image->element_size + origin[1] * image->row_pitch + origin[2] * image->slice_pitch;
		cmd->cb[0] = region[0] * image->element_size;
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->ptr = ptr;
		cmd->buffer_pitch[0] = image->row_pitch;
		cmd->buffer_pitch[1] = image->slice_pitch;
		cmd->host_pitch[0] = row_pitch;
		cmd->host_pitch[1] = slice_pitch;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_READ_IMAGE;
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

	cl_int clEnqueueWriteImageFCL (cl_command_queue command_queue,
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
		MSG(clEnqueueWriteImageFCL);

		if (ptr == NULL
				|| region[0] == 0
				|| region[1] == 0
				|| region[2] == 0)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(image))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(image);

		if (image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& image->mem_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_MEM_OBJECT;

		if (image->mem_type == CL_MEM_OBJECT_IMAGE2D
				&& (origin[2] != 0 || region[2] != 1))
			return CL_INVALID_VALUE;

		if (image->width < origin[0] + region[0]
				|| image->height < origin[1] + region[1]
				|| image->depth < origin[2] + region[2])
			return CL_INVALID_VALUE;

		if (input_row_pitch == 0)	input_row_pitch = image->width * image->element_size;
		if (input_slice_pitch == 0)	input_slice_pitch = image->height * image->row_pitch;

		if (input_row_pitch < image->width * image->element_size
				|| input_slice_pitch < image->row_pitch * image->height)
			return CL_INVALID_VALUE;

		if (blocking_write == CL_TRUE)
		{
			for(size_t i = 0 ; i < num_events_in_wait_list ; ++i)
				if (event_wait_list[i]->status < 0)
					return CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
		}

		FreeOCL::smartptr<FreeOCL::command_write_image> cmd = new FreeOCL::command_write_image;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = (blocking_write == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
		cmd->buffer = image;
		cmd->offset = origin[0] * image->element_size + origin[1] * image->row_pitch + origin[2] * image->slice_pitch;
		cmd->cb[0] = region[0] * image->element_size;
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->ptr = ptr;
		cmd->buffer_pitch[0] = image->row_pitch;
		cmd->buffer_pitch[1] = image->slice_pitch;
		cmd->host_pitch[0] = input_row_pitch;
		cmd->host_pitch[1] = input_slice_pitch;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_WRITE_IMAGE;
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

	cl_int clEnqueueCopyImageFCL (cl_command_queue command_queue,
							   cl_mem src_image,
							   cl_mem dst_image,
							   const size_t src_origin[3],
							   const size_t dst_origin[3],
							   const size_t region[3],
							   cl_uint num_events_in_wait_list,
							   const cl_event *event_wait_list,
							   cl_event *event)
	{
		MSG(clEnqueueCopyImageFCL);
		if (region[0] == 0
				|| region[1] == 0
				|| region[2] == 0)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(src_image))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(src_image);

		if (src_image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& src_image->mem_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_MEM_OBJECT;

		if (src_image->mem_type == CL_MEM_OBJECT_IMAGE2D
				&& (src_origin[2] != 0 || region[2] != 1))
			return CL_INVALID_VALUE;

		if (src_image->width < src_origin[0] + region[0]
				|| src_image->height < src_origin[1] + region[1]
				|| src_image->depth < src_origin[2] + region[2])
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(dst_image))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(dst_image);

		if (dst_image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& dst_image->mem_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_MEM_OBJECT;

		if (dst_image->mem_type == CL_MEM_OBJECT_IMAGE2D
				&& (dst_origin[2] != 0 || region[2] != 1))
			return CL_INVALID_VALUE;

		if (dst_image->width < dst_origin[0] + region[0]
				|| dst_image->height < dst_origin[1] + region[1]
				|| dst_image->depth < dst_origin[2] + region[2])
			return CL_INVALID_VALUE;

		if (src_image->image_format.image_channel_order != dst_image->image_format.image_channel_order
			|| src_image->image_format.image_channel_data_type != dst_image->image_format.image_channel_data_type)
			return CL_IMAGE_FORMAT_MISMATCH;

		if (src_image == dst_image
				&& src_origin[0] + region[0] > dst_origin[0] && src_origin[0] < dst_origin[0] + region[0]
				&& src_origin[1] + region[1] > dst_origin[1] && src_origin[1] < dst_origin[1] + region[1]
				&& src_origin[2] + region[2] > dst_origin[2] && src_origin[2] < dst_origin[2] + region[2])
			return CL_MEM_COPY_OVERLAP;

		FreeOCL::smartptr<FreeOCL::command_copy_image> cmd = new FreeOCL::command_copy_image;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = event ? new _cl_event(command_queue->context) : NULL;
		cmd->src_buffer = src_image;
		cmd->src_offset = src_origin[0] * src_image->element_size
						  + src_origin[1] * src_image->row_pitch
						  + src_origin[2] * src_image->slice_pitch;
		cmd->dst_buffer = dst_image;
		cmd->dst_offset = dst_origin[0] * dst_image->element_size
						  + dst_origin[1] * dst_image->row_pitch
						  + dst_origin[2] * dst_image->slice_pitch;
		cmd->cb[0] = region[0] * src_image->element_size;
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->src_pitch[0] = src_image->row_pitch;
		cmd->src_pitch[1] = src_image->slice_pitch;
		cmd->dst_pitch[0] = dst_image->row_pitch;
		cmd->dst_pitch[1] = dst_image->slice_pitch;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_COPY_IMAGE;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		return CL_SUCCESS;
	}

	cl_int clEnqueueCopyImageToBufferFCL (cl_command_queue command_queue,
									   cl_mem src_image,
									   cl_mem dst_buffer,
									   const size_t src_origin[3],
									   const size_t region[3],
									   size_t dst_offset,
									   cl_uint num_events_in_wait_list,
									   const cl_event *event_wait_list,
									   cl_event *event)
	{
		MSG(clEnqueueCopyImageToBufferFCL);
		if (region[0] == 0
				|| region[1] == 0
				|| region[2] == 0)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		unlock.handle(command_queue);

		if (!FreeOCL::is_valid(command_queue->context))
			return CL_INVALID_CONTEXT;
		command_queue->context->unlock();

		if (!FreeOCL::is_valid(src_image))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(src_image);

		if (src_image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& src_image->mem_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_MEM_OBJECT;

		if (src_image->mem_type == CL_MEM_OBJECT_IMAGE2D
				&& (src_origin[2] != 0 || region[2] != 1))
			return CL_INVALID_VALUE;

		if (src_image->width < src_origin[0] + region[0]
				|| src_image->height < src_origin[1] + region[1]
				|| src_image->depth < src_origin[2] + region[2])
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(dst_buffer))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(dst_buffer);

		if (dst_buffer->size < dst_offset + region[0] * region[1] * region[2] * src_image->element_size)
			return CL_INVALID_VALUE;

		FreeOCL::smartptr<FreeOCL::command_copy_image_to_buffer> cmd = new FreeOCL::command_copy_image_to_buffer;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = event ? new _cl_event(command_queue->context) : NULL;
		cmd->src_buffer = src_image;
		cmd->src_offset = src_origin[0] * src_image->element_size
						  + src_origin[1] * src_image->row_pitch
						  + src_origin[2] * src_image->slice_pitch;
		cmd->dst_buffer = dst_buffer;
		cmd->dst_offset = dst_offset;
		cmd->cb[0] = region[0] * src_image->element_size;
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->src_pitch[0] = src_image->row_pitch;
		cmd->src_pitch[1] = src_image->slice_pitch;
		cmd->dst_pitch[0] = region[0] * src_image->element_size;
		cmd->dst_pitch[1] = region[0] * region[1] * src_image->element_size;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_COPY_IMAGE_TO_BUFFER;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		return CL_SUCCESS;
	}

	cl_int clEnqueueCopyBufferToImageFCL (cl_command_queue command_queue,
									   cl_mem src_buffer,
									   cl_mem dst_image,
									   size_t src_offset,
									   const size_t dst_origin[3],
									   const size_t region[3],
									   cl_uint num_events_in_wait_list,
									   const cl_event *event_wait_list,
									   cl_event *event)
	{
		MSG(clEnqueueCopyBufferToImageFCL);
		if (region[0] == 0
				|| region[1] == 0
				|| region[2] == 0)
			return CL_INVALID_VALUE;

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

		if (!FreeOCL::is_valid(dst_image))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(dst_image);

		if (dst_image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& dst_image->mem_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_MEM_OBJECT;

		if (dst_image->mem_type == CL_MEM_OBJECT_IMAGE2D
				&& (dst_origin[2] != 0 || region[2] != 1))
			return CL_INVALID_VALUE;

		if (dst_image->width < dst_origin[0] + region[0]
				|| dst_image->height < dst_origin[1] + region[1]
				|| dst_image->depth < dst_origin[2] + region[2])
			return CL_INVALID_VALUE;

		if (src_buffer->size < src_offset + region[0] * region[1] * region[2] * dst_image->element_size)
			return CL_INVALID_VALUE;

		FreeOCL::smartptr<FreeOCL::command_copy_buffer_to_image> cmd = new FreeOCL::command_copy_buffer_to_image;
		cmd->num_events_in_wait_list = num_events_in_wait_list;
		cmd->event_wait_list = event_wait_list;
		cmd->event = event ? new _cl_event(command_queue->context) : NULL;
		cmd->src_buffer = src_buffer;
		cmd->src_offset = src_offset;
		cmd->dst_buffer = dst_image;
		cmd->dst_offset = dst_origin[0] * dst_image->element_size
						  + dst_origin[1] * dst_image->row_pitch
						  + dst_origin[2] * dst_image->slice_pitch;
		cmd->cb[0] = region[0] * dst_image->element_size;
		cmd->cb[1] = region[1];
		cmd->cb[2] = region[2];
		cmd->src_pitch[0] = region[0] * dst_image->element_size;
		cmd->src_pitch[1] = region[0] * region[1] * dst_image->element_size;
		cmd->dst_pitch[0] = dst_image->row_pitch;
		cmd->dst_pitch[1] = dst_image->slice_pitch;

		if (cmd->event)
		{
			cmd->event->command_queue = command_queue;
			cmd->event->command_type = CL_COMMAND_COPY_BUFFER_TO_IMAGE;
			cmd->event->status = CL_QUEUED;
		}

		if (event)
			*event = cmd->event.weak();

		unlock.forget(command_queue);
		command_queue->enqueue(cmd);

		unlock.unlockall();

		return CL_SUCCESS;
	}

	void *clEnqueueMapImageFCL (cl_command_queue command_queue,
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
		MSG(clEnqueueMapImageFCL);

		if (image_row_pitch == NULL)
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

		if (!FreeOCL::is_valid(image))
		{
			SET_RET(CL_INVALID_MEM_OBJECT);
			return NULL;
		}
		unlock.handle(image);
		if (image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& image->mem_type != CL_MEM_OBJECT_IMAGE3D)
		{
			SET_RET(CL_INVALID_MEM_OBJECT);
			return NULL;
		}

		if (image->mem_type == CL_MEM_OBJECT_IMAGE2D
				&& (origin[2] != 0 || region[2] != 1))
		{
			SET_RET(CL_INVALID_VALUE);
			return NULL;
		}

		if (image->mem_type == CL_MEM_OBJECT_IMAGE3D && image_slice_pitch == NULL)
		{
			SET_RET(CL_INVALID_VALUE);
			return NULL;
		}

		if (image->width < origin[0] + region[0]
				|| image->height < origin[1] + region[1]
				|| image->depth < origin[2] + region[2])
		{
			SET_RET(CL_INVALID_VALUE);
			return NULL;
		}

		*image_row_pitch = image->row_pitch;
		if (image_slice_pitch)
			*image_slice_pitch = (image->mem_type == CL_MEM_OBJECT_IMAGE2D) ? 0 : image->slice_pitch;

		void *p = (char*)image->ptr
				  + image->element_size * origin[0]
				  + image->row_pitch * origin[1]
				  + image->slice_pitch * origin[2];
		if (num_events_in_wait_list == 0 || event_wait_list == NULL)
		{
			image->mapped.insert(p);
			if (event)
			{
				cl_event e = new _cl_event(command_queue->context);
				*event = e;
				e->command_queue = command_queue;
				e->command_type = CL_COMMAND_MAP_IMAGE;
				e->status = CL_QUEUED;
				e->change_status(CL_QUEUED);
				e->change_status(CL_SUBMITTED);
				e->change_status(CL_RUNNING);
				e->change_status(CL_COMPLETE);
			}
		}
		else
		{
			FreeOCL::smartptr<FreeOCL::command_map_image> cmd = new FreeOCL::command_map_image;
			cmd->num_events_in_wait_list = num_events_in_wait_list;
			cmd->event_wait_list = event_wait_list;
			cmd->event = (blocking_map == CL_TRUE || event) ? new _cl_event(command_queue->context) : NULL;
			if (cmd->event)
			{
				cmd->event->command_queue = command_queue;
				cmd->event->command_type = CL_COMMAND_MAP_IMAGE;
				cmd->event->status = CL_QUEUED;
				if (event)
					*event = cmd->event.weak();
			}
			cmd->buffer = image;
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

	cl_int clGetImageInfoFCL (cl_mem image,
						   cl_image_info param_name,
						   size_t param_value_size,
						   void *param_value,
						   size_t *param_value_size_ret)
	{
		MSG(clGetImageInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(image))
			return CL_INVALID_MEM_OBJECT;
		unlock.handle(image);

		if (image->mem_type != CL_MEM_OBJECT_IMAGE2D
				&& image->mem_type != CL_MEM_OBJECT_IMAGE3D)
			return CL_INVALID_MEM_OBJECT;

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_IMAGE_FORMAT:		bTooSmall = SET_VAR(image->image_format);	break;
		case CL_IMAGE_ELEMENT_SIZE:	bTooSmall = SET_VAR(image->element_size);	break;
		case CL_IMAGE_ROW_PITCH:	bTooSmall = SET_VAR(image->row_pitch);		break;
		case CL_IMAGE_SLICE_PITCH:	bTooSmall = SET_VAR(image->slice_pitch);	break;
		case CL_IMAGE_WIDTH:		bTooSmall = SET_VAR(image->width);	break;
		case CL_IMAGE_HEIGHT:		bTooSmall = SET_VAR(image->height);	break;
		case CL_IMAGE_DEPTH:		bTooSmall = SET_VAR(image->depth);	break;
		default:
			return CL_INVALID_VALUE;
		}

		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}
}
