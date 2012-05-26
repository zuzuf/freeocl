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
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl_ext.h>
#else
#include <CL/cl_ext.h>
#endif
#include "platform.h"
#include <cstring>
#include <iostream>
#include "prototypes.h"

namespace
{
	const char *profile = "FULL_PROFILE";
	const char *version = "OpenCL 1.2 FreeOCL-" FREEOCL_VERSION_STRING;
	const char *name = "FreeOCL";
	const char *vendor = FREEOCL_VENDOR;
	const char *extensions = "cl_khr_icd";
	const char *vendor_suffix = "FCL";
}

#define SET_STRING(X)	FreeOCL::copy_memory_within_limits(X, strlen(X) + 1, param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_int clGetPlatformInfoFCL(cl_platform_id platform,
							 cl_platform_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		MSG(clGetPlatformInfoFCL);
		if (platform != FreeOCL::platform)
			return CL_INVALID_PLATFORM;
		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_PLATFORM_PROFILE:
			bTooSmall = SET_STRING(profile);
			break;
		case CL_PLATFORM_VERSION:
			bTooSmall = SET_STRING(version);
			break;
		case CL_PLATFORM_NAME:
			bTooSmall = SET_STRING(name);
			break;
		case CL_PLATFORM_VENDOR:
			bTooSmall = SET_STRING(vendor);
			break;
		case CL_PLATFORM_EXTENSIONS:
			bTooSmall = SET_STRING(extensions);
			break;
		case CL_PLATFORM_ICD_SUFFIX_KHR:
			bTooSmall = SET_STRING(vendor_suffix);
			break;
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;
		return CL_SUCCESS;
	}

	// The ICD loader needs this version too
	cl_int clGetPlatformInfo(cl_platform_id platform,
							 cl_platform_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		return clGetPlatformInfoFCL(platform,
									param_name,
									param_value_size,
									param_value,
									param_value_size_ret);
	}

	cl_int clIcdGetPlatformIDsKHR (cl_uint num_entries,
							 cl_platform_id *platforms,
							 cl_uint *num_platforms)
	{
		MSG(clIcdGetPlatformIDsKHR);
		if ((num_entries != 0 && platforms == NULL)
			|| (platforms == NULL && num_platforms == NULL))
			return CL_INVALID_VALUE;

		if (num_platforms != NULL)
			*num_platforms = 1;

		if (platforms != NULL)
			platforms[0] = FreeOCL::platform;

		return CL_SUCCESS;
	}

	CL_API_ENTRY void * CL_API_CALL	clGetExtensionFunctionAddressForPlatformFCL(cl_platform_id platform,
																				const char *   func_name) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clGetExtensionFunctionAddressForPlatformFCL);
		if (platform != FreeOCL::platform)
			return NULL;
		return clGetExtensionFunctionAddressFCL(func_name);
	}
}

namespace FreeOCL
{
	_cl_platform_id freeocl_platform;
	cl_platform_id platform = &freeocl_platform;
}
