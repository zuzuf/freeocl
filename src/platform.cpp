#include "platform.h"
#include <cstring>

namespace
{
	const char *profile = "FULL_PROFILE";
	const char *version = FREEOCL_VERSION_STRING;
	const char *name = "FreeOCL";
	const char *vendor = FREEOCL_VENDOR;
	const char *extensions = "cl_khr_icd";
}

#define SET_STRING(X)	FreeOCL::copyMemoryWithinLimits(X, strlen(X) + 1, param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_int clGetPlatformInfo(cl_platform_id platform,
							 cl_platform_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
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
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;
		return CL_SUCCESS;
	}

	cl_int clGetPlatformIDs (cl_uint num_entries,
							 cl_platform_id *platforms,
							 cl_uint *num_platforms)
	{
		if ((num_entries != 0 && platforms == NULL)
			|| (platforms == NULL && num_platforms == NULL))
			return CL_INVALID_VALUE;

		if (num_platforms != NULL)
			*num_platforms = 1;

		if (platforms != NULL)
			platforms[0] = FreeOCL::platform;

		return CL_SUCCESS;
	}
}

namespace FreeOCL
{
	_cl_platform_id freeocl_platform;
	cl_platform_id platform = &freeocl_platform;
}
