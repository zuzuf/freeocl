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
#include "device.h"
#include "platform.h"
#include <cstring>
#include <iostream>
#include <time.h>
#include <algorithm>
#include <CL/cl_ext.h>

#define SEP " "

namespace
{
	const cl_bool cl_bool_true = CL_TRUE;
	const cl_bool cl_bool_false = CL_FALSE;
}

#define SET_STRING(X)	FreeOCL::copy_memory_within_limits((X), strlen(X) + 1, param_value_size, param_value, param_value_size_ret)
#define SET_VAR(X)	FreeOCL::copy_memory_within_limits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_int clGetDeviceInfoFCL (cl_device_id device,
							cl_device_info param_name,
							size_t param_value_size,
							void *param_value,
							size_t *param_value_size_ret)
	{
		MSG(clGetDeviceInfoFCL);
		if (device != FreeOCL::device)
			return CL_INVALID_DEVICE;
		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_DEVICE_TYPE:							bTooSmall = SET_VAR(device->device_type);	break;
		case CL_DEVICE_VENDOR_ID:						bTooSmall = SET_VAR(device->vendor_id);		break;
		case CL_DEVICE_MAX_COMPUTE_UNITS:				bTooSmall = SET_VAR(device->cpu_cores);		break;
		case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:		bTooSmall = SET_VAR(device->max_work_item_dimensions);	break;
		case CL_DEVICE_MAX_WORK_ITEM_SIZES:				bTooSmall = SET_VAR(device->max_work_item_sizes);	break;
		case CL_DEVICE_MAX_WORK_GROUP_SIZE:				bTooSmall = SET_VAR(device->max_work_group_size);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:		bTooSmall = SET_VAR(device->preferred_vector_width_char);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:	bTooSmall = SET_VAR(device->preferred_vector_width_short);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:		bTooSmall = SET_VAR(device->preferred_vector_width_int);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:		bTooSmall = SET_VAR(device->preferred_vector_width_long);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:	bTooSmall = SET_VAR(device->preferred_vector_width_float);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:	bTooSmall = SET_VAR(device->preferred_vector_width_double);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF:		bTooSmall = SET_VAR(device->preferred_vector_width_half);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR:		bTooSmall = SET_VAR(device->native_vector_width_char);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT:		bTooSmall = SET_VAR(device->native_vector_width_short);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_INT:			bTooSmall = SET_VAR(device->native_vector_width_int);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG:		bTooSmall = SET_VAR(device->native_vector_width_long);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:		bTooSmall = SET_VAR(device->native_vector_width_float);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE:		bTooSmall = SET_VAR(device->native_vector_width_double);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF:		bTooSmall = SET_VAR(device->native_vector_width_half);	break;
		case CL_DEVICE_MAX_CLOCK_FREQUENCY:				bTooSmall = SET_VAR(device->max_clock_frequency);	break;
		case CL_DEVICE_ADDRESS_BITS:					bTooSmall = SET_VAR(device->addressbits);	break;
		case CL_DEVICE_MAX_MEM_ALLOC_SIZE:				bTooSmall = SET_VAR(device->freememsize);	break;
		case CL_DEVICE_IMAGE_SUPPORT:					bTooSmall = SET_VAR(cl_bool_true);	break;

		case CL_DEVICE_MAX_READ_IMAGE_ARGS:				bTooSmall = SET_VAR(device->max_read_image_args);	break;
		case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:			bTooSmall = SET_VAR(device->max_write_image_args);	break;
		case CL_DEVICE_IMAGE2D_MAX_WIDTH:				bTooSmall = SET_VAR(device->image2d_max_width);	break;
		case CL_DEVICE_IMAGE2D_MAX_HEIGHT:				bTooSmall = SET_VAR(device->image2d_max_height);	break;
		case CL_DEVICE_IMAGE3D_MAX_WIDTH:				bTooSmall = SET_VAR(device->image3d_max_width);	break;
		case CL_DEVICE_IMAGE3D_MAX_HEIGHT:				bTooSmall = SET_VAR(device->image3d_max_height);	break;
		case CL_DEVICE_IMAGE3D_MAX_DEPTH:				bTooSmall = SET_VAR(device->image3d_max_depth);	break;
		case CL_DEVICE_MAX_SAMPLERS:					bTooSmall = SET_VAR(device->max_samplers);	break;

		case CL_DEVICE_MAX_PARAMETER_SIZE:				bTooSmall = SET_VAR(device->max_parameter_size);	break;
		case CL_DEVICE_MEM_BASE_ADDR_ALIGN:				bTooSmall = SET_VAR(device->mem_base_addr_align);	break;
		case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:		bTooSmall = SET_VAR(device->mem_base_addr_align);	break;
		case CL_DEVICE_SINGLE_FP_CONFIG:				bTooSmall = SET_VAR(device->fp_config);	break;
		case CL_DEVICE_DOUBLE_FP_CONFIG:				bTooSmall = SET_VAR(device->fp_config);	break;
		case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:			bTooSmall = SET_VAR(device->mem_cache_type);	break;
		case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:		bTooSmall = SET_VAR(device->mem_cacheline_size);	break;
		case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:			bTooSmall = SET_VAR(device->mem_cache_size);	break;
		case CL_DEVICE_GLOBAL_MEM_SIZE:					bTooSmall = SET_VAR(device->memsize);	break;
		case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:		bTooSmall = SET_VAR(device->max_constant_buffer_size);	break;
		case CL_DEVICE_MAX_CONSTANT_ARGS:				bTooSmall = SET_VAR(device->max_constant_args);	break;
		case CL_DEVICE_LOCAL_MEM_TYPE:					bTooSmall = SET_VAR(device->local_mem_type);	break;
		case CL_DEVICE_LOCAL_MEM_SIZE:					bTooSmall = SET_VAR(device->local_mem_size);	break;
		case CL_DEVICE_ERROR_CORRECTION_SUPPORT:		bTooSmall = SET_VAR(cl_bool_false);	break;
		case CL_DEVICE_HOST_UNIFIED_MEMORY:				bTooSmall = SET_VAR(cl_bool_true);	break;
		case CL_DEVICE_PROFILING_TIMER_RESOLUTION:		bTooSmall = SET_VAR(device->timer_resolution);	break;
		case CL_DEVICE_ENDIAN_LITTLE:					bTooSmall = SET_VAR(device->endian_little);	break;
		case CL_DEVICE_AVAILABLE:
		case CL_DEVICE_COMPILER_AVAILABLE:				bTooSmall = SET_VAR(cl_bool_true);	break;
		case CL_DEVICE_EXECUTION_CAPABILITIES:			bTooSmall = SET_VAR(device->exec_capabilities);	break;
		case CL_DEVICE_QUEUE_PROPERTIES:				bTooSmall = SET_VAR(device->command_queue_properties);	break;
		case CL_DEVICE_PLATFORM:						bTooSmall = SET_VAR(FreeOCL::platform);	break;
		case CL_DEVICE_NAME:							bTooSmall = SET_STRING(device->name.c_str());	break;
		case CL_DEVICE_VENDOR:							bTooSmall = SET_STRING(device->vendor.c_str());	break;
		case CL_DRIVER_VERSION:							bTooSmall = SET_STRING(device->driver_version);	break;
		case CL_DEVICE_PROFILE:							bTooSmall = SET_STRING(device->device_profile);	break;
		case CL_DEVICE_VERSION:							bTooSmall = SET_STRING(device->version.c_str());	break;
		case CL_DEVICE_OPENCL_C_VERSION:				bTooSmall = SET_STRING(device->opencl_c_version);	break;
		case CL_DEVICE_EXTENSIONS:						bTooSmall = SET_STRING(device->extensions);	break;
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;
		return CL_SUCCESS;
	}

	cl_int clGetDeviceIDsFCL (cl_platform_id platform,
							  cl_device_type device_type,
							  cl_uint num_entries,
							  cl_device_id *devices,
							  cl_uint *num_devices)
	{
		MSG(clGetDeviceIDsFCL);
		if (platform != FreeOCL::platform)
			return CL_INVALID_PLATFORM;

		if ((num_entries == 0 && devices != NULL)
			|| (devices == NULL && num_devices == NULL))
			return CL_INVALID_VALUE;

		switch(device_type)
		{
		case CL_DEVICE_TYPE_DEFAULT:
		case CL_DEVICE_TYPE_CPU:
		case CL_DEVICE_TYPE_ALL:
			if (num_devices != NULL)
				*num_devices = 1;
			if (devices != NULL)
				devices[0] = FreeOCL::device;
			break;
		case CL_DEVICE_TYPE_GPU:
		case CL_DEVICE_TYPE_ACCELERATOR:
			if (num_devices != NULL)
				*num_devices = 0;
			return CL_DEVICE_NOT_FOUND;
		default:
			return CL_INVALID_DEVICE_TYPE;
		}

		return CL_SUCCESS;
	}
}

_cl_device_id::_cl_device_id() :
	device_type(CL_DEVICE_TYPE_CPU),
	vendor_id(0),
	addressbits(sizeof(void*) * 8),
	version("OpenCL 1.1 FreeOCL-" FREEOCL_VERSION_STRING),
	driver_version(FREEOCL_VERSION_STRING),
	device_profile("FULL_PROFILE"),
	opencl_c_version("OpenCL C 1.1 FreeOCL-" FREEOCL_VERSION_STRING),
	extensions("cl_khr_icd" SEP
			   "cl_khr_global_int32_base_atomics" SEP
			   "cl_khr_global_int32_extended_atomics" SEP
			   "cl_khr_local_int32_base_atomics" SEP
			   "cl_khr_local_int32_extended_atomics" SEP
			   "cl_khr_byte_addressable_store" SEP
			   "cl_khr_fp64" SEP
			   "cl_khr_int64_base_atomics" SEP
			   "cl_khr_int64_extended_atomics"),
	exec_capabilities(CL_EXEC_KERNEL | CL_EXEC_NATIVE_KERNEL),
	preferred_vector_width_char(16),
	preferred_vector_width_short(8),
	preferred_vector_width_int(4),
	preferred_vector_width_long(2),
	preferred_vector_width_float(4),
	preferred_vector_width_double(2),
	preferred_vector_width_half(8),
	native_vector_width_char(16),
	native_vector_width_short(8),
	native_vector_width_int(4),
	native_vector_width_long(2),
	native_vector_width_float(4),
	native_vector_width_double(2),
	native_vector_width_half(8),
	max_work_item_dimensions(3),
	max_work_group_size(1024),
	mem_cache_type(CL_READ_WRITE_CACHE),
	local_mem_type(CL_GLOBAL),
	local_mem_size(0x100000),
	max_parameter_size(8192),
	mem_base_addr_align(128),
	fp_config(CL_FP_DENORM | CL_FP_INF_NAN | CL_FP_FMA | CL_FP_ROUND_TO_NEAREST | CL_FP_ROUND_TO_ZERO | CL_FP_ROUND_TO_INF),
	max_constant_buffer_size(0x100000),
	max_constant_args(1024),
	timer_resolution(std::max<size_t>(1LU, 1000000000LU / CLOCKS_PER_SEC)),
	command_queue_properties(CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE),

	max_read_image_args(128),
	max_write_image_args(8),
	image2d_max_width(8192),
	image2d_max_height(8192),
	image3d_max_width(2048),
	image3d_max_height(2048),
	image3d_max_depth(2048),
	max_samplers(16)
{
	using namespace FreeOCL;

	const std::string ostype = trim(run_command("sysctl -a | grep ostype | awk '{ print $NF }'"));
	if (ostype == "Linux")
	{
		cpu_cores = parse_int(run_command("cat /proc/cpuinfo | grep \"cpu cores\" | head -1 | sed -e \"s/cpu cores\t: //\""));
		memsize = parse_int(run_command("cat /proc/meminfo | grep MemTotal | awk '{ print $2 }'")) * 1024U;
		freememsize = parse_int(run_command("cat /proc/meminfo | grep MemFree | awk '{ print $2 }'")) * 1024U;
		name = trim(run_command("cat /proc/cpuinfo | grep \"model name\" | head -1 | sed -e \"s/model name\t: //\""));
		vendor = trim(run_command("cat /proc/cpuinfo | grep vendor_id | head -1 | sed -e \"s/vendor_id\t: //\""));
		max_clock_frequency = parse_int(run_command("cat /proc/cpuinfo | grep \"cpu MHz\" | head -1 | sed -e \"s/cpu MHz\t\t: //\""));
		mem_cacheline_size = parse_int(run_command("cat /proc/cpuinfo | grep cache_alignment | head -1 | sed -e \"s/cache_alignment\t: //\""));
		mem_cache_size = parse_int(run_command("cat /proc/cpuinfo | grep \"cache size\" | head -1 | awk '{ print $4 }'")) * 1024U;
	}
	else if (ostype == "FreeBSD" || ostype == "NetBSD" || ostype == "OpenBSD")	// Regarding NetBSD and OpenBSD
	{																			// this is just a guess
		cpu_cores = parse_int(run_command("sysctl hw.ncpu | awk '{ print $NF }'"));
		name = trim(run_command("sysctl hw.model | awk '{ for(i=2;i<NF;++i) print $i }' | tr \"\\n\" \" \""));
		vendor = trim(run_command("sysctl hw.model | awk '{ print $2 }'"));
		memsize = parse_int(run_command("sysctl hw.realmem | awk '{ print $NF }'"));
		freememsize = parse_int(run_command("sysctl -a | grep v_free_count | awk '{ print $NF }'"))
					  * parse_int(run_command("sysctl -a | grep v_page_size | awk '{ print $NF }'"));
		max_clock_frequency = parse_int(run_command("sysctl hw.clockrate | awk '{ print $NF }'"));
		// I don't know how to get this information ... so let's use default values
		mem_cacheline_size = 64;
		mem_cache_size = 4 * 1024 * 1024;
	}

	max_work_item_sizes[0] = 1024;
	max_work_item_sizes[1] = 1024;
	max_work_item_sizes[2] = 1024;
	union
	{
		const int a;
		const char b[sizeof(int)];
	} v = { 1 };
	endian_little = v.b[0] ? CL_TRUE : CL_FALSE;
}

namespace FreeOCL
{
	_cl_device_id freeocl_device;
	cl_device_id device = &freeocl_device;
}
