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
#include "device.h"
#include "platform.h"
#include <cstring>
#include <iostream>
#include <time.h>
#include <algorithm>

#define SEP " "

namespace
{
	const cl_device_type device_type = CL_DEVICE_TYPE_CPU;
	const cl_uint vendor_id = 0;
	const cl_uint addressbits = sizeof(void*) * 8;
	const cl_ulong memsize = FreeOCL::parseInt(FreeOCL::runCommand("cat /proc/meminfo | grep MemTotal | awk '{ print $2 }'")) * 1024U;
	const cl_ulong freememsize = FreeOCL::parseInt(FreeOCL::runCommand("cat /proc/meminfo | grep MemFree | awk '{ print $2 }'")) * 1024U;
	const std::string name = FreeOCL::trim(FreeOCL::runCommand("cat /proc/cpuinfo | grep \"model name\" | head -1 | sed -e \"s/model name\t: //\""));
	const std::string vendor = FreeOCL::trim(FreeOCL::runCommand("cat /proc/cpuinfo | grep vendor_id | head -1 | sed -e \"s/vendor_id\t: //\""));
	const std::string version = "unknown";
	const cl_uint cpu_cores = FreeOCL::parseInt(FreeOCL::runCommand("cat /proc/cpuinfo | grep \"cpu cores\" | head -1 | sed -e \"s/cpu cores\t: //\""));
	const char *driver_version = FREEOCL_VERSION_STRING;
	const char *device_profile = "FULL_PROFILE";
	const char *opencl_c_version = "OpenCL C 1.1 FreeOCL-" FREEOCL_VERSION_STRING;
	const char *extensions = "cl_khr_icd" SEP
							 "cl_khr_global_int32_base_atomics" SEP
							 "cl_khr_global_int32_extended_atomics" SEP
							 "cl_khr_local_int32_base_atomics" SEP
							 "cl_khr_local_int32_extended_atomics" SEP
							 "cl_khr_byte_addressable_store" SEP
							 "cl_khr_fp64" SEP
							 "cl_khr_int64_base_atomics" SEP
							 "cl_khr_int64_extended_atomics"
							 ;
	const cl_bool cl_bool_true = CL_TRUE;
	const cl_bool cl_bool_false = CL_FALSE;
	const cl_device_exec_capabilities exec_capabilities = CL_EXEC_KERNEL | CL_EXEC_NATIVE_KERNEL;

	const cl_uint preferred_vector_width_char = 1;
	const cl_uint preferred_vector_width_short = 1;
	const cl_uint preferred_vector_width_int = 1;
	const cl_uint preferred_vector_width_long = 1;
	const cl_uint preferred_vector_width_float = 1;
	const cl_uint preferred_vector_width_double = 1;
	const cl_uint preferred_vector_width_half = 1;
	const cl_uint native_vector_width_char = 1;
	const cl_uint native_vector_width_short = 1;
	const cl_uint native_vector_width_int = 1;
	const cl_uint native_vector_width_long = 1;
	const cl_uint native_vector_width_float = 1;
	const cl_uint native_vector_width_double = 1;
	const cl_uint native_vector_width_half = 1;
	const size_t max_work_item_sizes[] = {	1024, 1024, 1024	};
	const cl_uint max_work_item_dimensions = sizeof(max_work_item_sizes) / sizeof(size_t);
	const size_t max_work_group_size = 1024;
	const cl_uint max_clock_frequency = FreeOCL::parseInt(FreeOCL::runCommand("cat /proc/cpuinfo | grep \"cpu MHz\" | head -1 | sed -e \"s/cpu MHz\t\t: //\""));
	const cl_device_mem_cache_type mem_cache_type = CL_READ_WRITE_CACHE;
	const cl_uint mem_cacheline_size = FreeOCL::parseInt(FreeOCL::runCommand("cat /proc/cpuinfo | grep cache_alignment | head -1 | sed -e \"s/cache_alignment\t: //\""));
	const cl_ulong mem_cache_size = FreeOCL::parseInt(FreeOCL::runCommand("cat /proc/cpuinfo | grep \"cache size\" | head -1 | awk '{ print $4 }'")) * 1024U;
	const cl_device_local_mem_type local_mem_type = CL_GLOBAL;
	const cl_ulong local_mem_size = mem_cache_size;
	const size_t max_parameter_size	= 8192;
	const cl_uint mem_base_addr_align = 128;
	const cl_device_fp_config fp_config = CL_FP_DENORM | CL_FP_INF_NAN | CL_FP_FMA | CL_FP_ROUND_TO_NEAREST;
	const cl_ulong max_constant_buffer_size = 0x1000000;
	const cl_uint max_constant_args = 1024;
	const size_t timer_resolution = std::max<size_t>(1LU, 1000000000LU / CLOCKS_PER_SEC);
	const cl_command_queue_properties command_queue_properties = CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
}

#define SET_STRING(X)	FreeOCL::copyMemoryWithinLimits((X), strlen(X) + 1, param_value_size, param_value, param_value_size_ret)
#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_int clGetDeviceInfo (cl_device_id device,
							cl_device_info param_name,
							size_t param_value_size,
							void *param_value,
							size_t *param_value_size_ret)
	{
		if (!FreeOCL::isValid(device))
			return CL_INVALID_DEVICE;
		return device->dispatch->clGetDeviceInfo(device,
												 param_name,
												 param_value_size,
												 param_value,
												 param_value_size_ret);
	}

	cl_int clGetDeviceInfoFCL (cl_device_id device,
							cl_device_info param_name,
							size_t param_value_size,
							void *param_value,
							size_t *param_value_size_ret)
	{
		if (device != FreeOCL::device)
			return CL_INVALID_DEVICE;
		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_DEVICE_TYPE:								bTooSmall = SET_VAR(device_type);	break;
		case CL_DEVICE_VENDOR_ID:							bTooSmall = SET_VAR(vendor_id);		break;
		case CL_DEVICE_MAX_COMPUTE_UNITS:					bTooSmall = SET_VAR(cpu_cores);		break;
		case CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:			bTooSmall = SET_VAR(max_work_item_dimensions);	break;
		case CL_DEVICE_MAX_WORK_ITEM_SIZES:					bTooSmall = SET_VAR(max_work_item_sizes);	break;
		case CL_DEVICE_MAX_WORK_GROUP_SIZE:					bTooSmall = SET_VAR(max_work_group_size);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:			bTooSmall = SET_VAR(preferred_vector_width_char);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:		bTooSmall = SET_VAR(preferred_vector_width_short);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:			bTooSmall = SET_VAR(preferred_vector_width_int);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:			bTooSmall = SET_VAR(preferred_vector_width_long);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:		bTooSmall = SET_VAR(preferred_vector_width_float);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:		bTooSmall = SET_VAR(preferred_vector_width_double);	break;
		case CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF:			bTooSmall = SET_VAR(preferred_vector_width_half);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR:			bTooSmall = SET_VAR(native_vector_width_char);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT:			bTooSmall = SET_VAR(native_vector_width_short);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_INT:				bTooSmall = SET_VAR(native_vector_width_int);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG:			bTooSmall = SET_VAR(native_vector_width_long);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT:			bTooSmall = SET_VAR(native_vector_width_float);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE:			bTooSmall = SET_VAR(native_vector_width_double);	break;
		case CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF:			bTooSmall = SET_VAR(native_vector_width_half);	break;
		case CL_DEVICE_MAX_CLOCK_FREQUENCY:					bTooSmall = SET_VAR(max_clock_frequency);	break;
		case CL_DEVICE_ADDRESS_BITS:						bTooSmall = SET_VAR(addressbits);	break;
		case CL_DEVICE_MAX_MEM_ALLOC_SIZE:					bTooSmall = SET_VAR(freememsize);	break;
		case CL_DEVICE_IMAGE_SUPPORT:						bTooSmall = SET_VAR(cl_bool_false);	break;

		case CL_DEVICE_MAX_READ_IMAGE_ARGS:
		case CL_DEVICE_MAX_WRITE_IMAGE_ARGS:
		case CL_DEVICE_IMAGE2D_MAX_WIDTH:
		case CL_DEVICE_IMAGE2D_MAX_HEIGHT:
		case CL_DEVICE_IMAGE3D_MAX_WIDTH:
		case CL_DEVICE_IMAGE3D_MAX_HEIGHT:
		case CL_DEVICE_IMAGE3D_MAX_DEPTH:
		case CL_DEVICE_MAX_SAMPLERS:
			return CL_INVALID_VALUE;

		case CL_DEVICE_MAX_PARAMETER_SIZE:					bTooSmall = SET_VAR(max_parameter_size);	break;
		case CL_DEVICE_MEM_BASE_ADDR_ALIGN:					bTooSmall = SET_VAR(mem_base_addr_align);	break;
		case CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:			bTooSmall = SET_VAR(mem_base_addr_align);	break;
		case CL_DEVICE_SINGLE_FP_CONFIG:					bTooSmall = SET_VAR(fp_config);	break;
		case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:				bTooSmall = SET_VAR(mem_cache_type);	break;
		case CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE:			bTooSmall = SET_VAR(mem_cacheline_size);	break;
		case CL_DEVICE_GLOBAL_MEM_CACHE_SIZE:				bTooSmall = SET_VAR(mem_cache_size);	break;
		case CL_DEVICE_GLOBAL_MEM_SIZE:						bTooSmall = SET_VAR(memsize);	break;
		case CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:			bTooSmall = SET_VAR(max_constant_buffer_size);	break;
		case CL_DEVICE_MAX_CONSTANT_ARGS:					bTooSmall = SET_VAR(max_constant_args);	break;
		case CL_DEVICE_LOCAL_MEM_TYPE:						bTooSmall = SET_VAR(local_mem_type);	break;
		case CL_DEVICE_LOCAL_MEM_SIZE:						bTooSmall = SET_VAR(local_mem_size);	break;
		case CL_DEVICE_ERROR_CORRECTION_SUPPORT:			bTooSmall = SET_VAR(cl_bool_false);	break;
		case CL_DEVICE_HOST_UNIFIED_MEMORY:					bTooSmall = SET_VAR(cl_bool_true);	break;
		case CL_DEVICE_PROFILING_TIMER_RESOLUTION:			bTooSmall = SET_VAR(timer_resolution);	break;
		case CL_DEVICE_ENDIAN_LITTLE:
			{
				union
				{
					const int a;
					const char b[sizeof(int)];
				} v = { 1 };
				if (v.b[0])
					bTooSmall = SET_VAR(cl_bool_true);
				else
					bTooSmall = SET_VAR(cl_bool_false);
			}
			break;
		case CL_DEVICE_AVAILABLE:
		case CL_DEVICE_COMPILER_AVAILABLE:
			bTooSmall = SET_VAR(cl_bool_true);
			break;
		case CL_DEVICE_EXECUTION_CAPABILITIES:				bTooSmall = SET_VAR(exec_capabilities);	break;
		case CL_DEVICE_QUEUE_PROPERTIES:					bTooSmall = SET_VAR(command_queue_properties);	break;
		case CL_DEVICE_PLATFORM:							bTooSmall = SET_VAR(FreeOCL::platform);	break;
		case CL_DEVICE_NAME:								bTooSmall = SET_STRING(name.c_str());	break;
		case CL_DEVICE_VENDOR:								bTooSmall = SET_STRING(vendor.c_str());	break;
		case CL_DRIVER_VERSION:								bTooSmall = SET_STRING(driver_version);	break;
		case CL_DEVICE_PROFILE:								bTooSmall = SET_STRING(device_profile);	break;
		case CL_DEVICE_VERSION:								bTooSmall = SET_STRING(version.c_str());	break;
		case CL_DEVICE_OPENCL_C_VERSION:					bTooSmall = SET_STRING(opencl_c_version);	break;
		case CL_DEVICE_EXTENSIONS:							bTooSmall = SET_STRING(extensions);	break;
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;
		return CL_SUCCESS;
	}

	cl_int clGetDeviceIDs (cl_platform_id platform,
						   cl_device_type device_type,
						   cl_uint num_entries,
						   cl_device_id *devices,
						   cl_uint *num_devices)
	{
		if (!FreeOCL::isValid(platform))
			return CL_INVALID_PLATFORM;
		return platform->dispatch->clGetDeviceIDs(platform,
												  device_type,
												  num_entries,
												  devices,
												  num_devices);
	}

	cl_int clGetDeviceIDsFCL (cl_platform_id platform,
							  cl_device_type device_type,
							  cl_uint num_entries,
							  cl_device_id *devices,
							  cl_uint *num_devices)
	{
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


namespace FreeOCL
{
	_cl_device_id freeocl_device;
	cl_device_id device = &freeocl_device;
}
