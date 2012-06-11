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
#ifndef __FREEOCL_DEVICE_H__
#define __FREEOCL_DEVICE_H__

#include "freeocl.h"

struct _cl_device_id : public FreeOCL::icd_table
{
	_cl_device_id();

	const cl_device_type device_type;
	const cl_uint vendor_id;
	const cl_uint addressbits;
	cl_ulong memsize;
	cl_ulong freememsize;
	std::string name;
	std::string vendor;
	const std::string version;
	cl_uint cpu_cores;
	const char *driver_version;
	const char *device_profile;
	const char *opencl_c_version;
	const char *extensions;
	const cl_device_exec_capabilities exec_capabilities;

	const cl_uint preferred_vector_width_char;
	const cl_uint preferred_vector_width_short;
	const cl_uint preferred_vector_width_int;
	const cl_uint preferred_vector_width_long;
	const cl_uint preferred_vector_width_float;
	const cl_uint preferred_vector_width_double;
	const cl_uint preferred_vector_width_half;
	const cl_uint native_vector_width_char;
	const cl_uint native_vector_width_short;
	const cl_uint native_vector_width_int;
	const cl_uint native_vector_width_long;
	const cl_uint native_vector_width_float;
	const cl_uint native_vector_width_double;
	const cl_uint native_vector_width_half;
	size_t max_work_item_sizes[3];
	const cl_uint max_work_item_dimensions;
	const size_t max_work_group_size;
	cl_uint max_clock_frequency;
	const cl_device_mem_cache_type mem_cache_type;
	cl_uint mem_cacheline_size;
	cl_ulong mem_cache_size;
	const cl_device_local_mem_type local_mem_type;
	const cl_ulong local_mem_size;
	const size_t max_parameter_size;
	const cl_uint mem_base_addr_align;
	const cl_device_fp_config fp_config;
	const cl_ulong max_constant_buffer_size;
	const cl_uint max_constant_args;
	const size_t timer_resolution;
	const cl_command_queue_properties command_queue_properties;

	const cl_uint max_read_image_args;
	const cl_uint max_write_image_args;
	const size_t image2d_max_width;
	const size_t image2d_max_height;
	const size_t image3d_max_width;
	const size_t image3d_max_height;
	const size_t image3d_max_depth;
	const size_t image_max_buffer_size;
	const size_t image_max_array_size;
	const cl_uint max_samplers;

	cl_bool endian_little;

	size_t printf_buffer_size;
	cl_uint max_sub_devices;
};

#endif
