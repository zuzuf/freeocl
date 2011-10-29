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
			channels = 4;
			switch(image_format->image_channel_data_type)
			{
			case CL_UNORM_INT8:
				break;
			case CL_SNORM_INT8:
			case CL_SIGNED_INT8:
			case CL_UNSIGNED_INT8:
				SET_RET(CL_IMAGE_FORMAT_NOT_SUPPORTED);
				return 0;
			default:
				SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
				return 0;
			}
			break;
		case CL_R:
		case CL_Rx:
		case CL_A:
		case CL_RG:
		case CL_RGx:
		case CL_RA:
			SET_RET(CL_IMAGE_FORMAT_NOT_SUPPORTED);
			return 0;
		case CL_ARGB:
			switch(image_format->image_channel_data_type)
			{
			case CL_UNORM_INT8:
			case CL_SNORM_INT8:
			case CL_SIGNED_INT8:
			case CL_UNSIGNED_INT8:
				SET_RET(CL_IMAGE_FORMAT_NOT_SUPPORTED);
				break;
			default:
				SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
			}
			return 0;
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
				SET_RET(CL_IMAGE_FORMAT_NOT_SUPPORTED);
				break;
			default:
				SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
			}
			return 0;
		case CL_RGB:
		case CL_RGBx:
			switch(image_format->image_channel_data_type)
			{
			case CL_UNORM_SHORT_555:
			case CL_UNORM_SHORT_565:
			case CL_UNORM_INT_101010:
				SET_RET(CL_IMAGE_FORMAT_NOT_SUPPORTED);
				break;
			default:
				SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
			}
			return 0;
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

		case CL_SNORM_INT8:
		case CL_SNORM_INT16:
		case CL_UNORM_SHORT_565:
		case CL_UNORM_SHORT_555:
		case CL_UNORM_INT_101010:
			SET_RET(CL_IMAGE_FORMAT_NOT_SUPPORTED);
			return 0;
		default:
			SET_RET(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
			return 0;
		}

		const size_t element_size = channels * data_size;

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
			{CL_RGBA, CL_UNORM_INT8},
			{CL_RGBA, CL_UNORM_INT16},

			{CL_RGBA, CL_SIGNED_INT8},
			{CL_RGBA, CL_SIGNED_INT16},
			{CL_RGBA, CL_SIGNED_INT32},

			{CL_RGBA, CL_UNSIGNED_INT8},
			{CL_RGBA, CL_UNSIGNED_INT16},
			{CL_RGBA, CL_UNSIGNED_INT32},

			{CL_RGBA, CL_HALF_FLOAT},
			{CL_RGBA, CL_FLOAT},

			{CL_BGRA, CL_UNORM_INT8},
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
		return CL_INVALID_MEM_OBJECT;
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
		return CL_INVALID_MEM_OBJECT;
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
		return CL_INVALID_MEM_OBJECT;
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
		return CL_INVALID_MEM_OBJECT;
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
		return CL_INVALID_MEM_OBJECT;
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
		SET_RET(CL_INVALID_MEM_OBJECT);
		return NULL;
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
