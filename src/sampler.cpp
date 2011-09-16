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
#include "sampler.h"
#include "context.h"

#define SET_VAR(X)	FreeOCL::copy_memory_within_limits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_sampler clCreateSamplerFCL (cl_context context,
								cl_bool normalized_coords,
								cl_addressing_mode addressing_mode,
								cl_filter_mode filter_mode,
								cl_int *errcode_ret)
	{
		MSG(clCreateSamplerFCL);

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return 0;
		}
		unlock.handle(context);

		cl_sampler sampler = new _cl_sampler(context);
		sampler->normalized_coords = normalized_coords;
		sampler->filter_mode = filter_mode;
		sampler->addressing_mode = addressing_mode;

		SET_RET(CL_SUCCESS);

		return sampler;
	}

	cl_int clRetainSamplerFCL (cl_sampler sampler)
	{
		MSG(clRetainSamplerFCL);
		if (!FreeOCL::is_valid(sampler))
			return CL_INVALID_SAMPLER;

		sampler->retain();
		sampler->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseSamplerFCL (cl_sampler sampler)
	{
		MSG(clReleaseSamplerFCL);
		if (!FreeOCL::is_valid(sampler))
			return CL_INVALID_SAMPLER;

		sampler->release();
		if (sampler->get_ref_count() == 0)
		{
			sampler->invalidate();
			sampler->unlock();
			delete sampler;
		}
		else
			sampler->unlock();
		return CL_SUCCESS;
	}

	cl_int clGetSamplerInfoFCL (cl_sampler sampler,
							 cl_sampler_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		MSG(clGetSamplerInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(sampler))
			return CL_INVALID_SAMPLER;
		unlock.handle(sampler);

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_SAMPLER_REFERENCE_COUNT:	bTooSmall = SET_VAR(sampler->get_ref_count());	break;
		case CL_SAMPLER_CONTEXT:			bTooSmall = SET_VAR(sampler->context);	break;
		case CL_SAMPLER_NORMALIZED_COORDS:	bTooSmall = SET_VAR(sampler->normalized_coords);	break;
		case CL_SAMPLER_ADDRESSING_MODE:	bTooSmall = SET_VAR(sampler->addressing_mode);	break;
		case CL_SAMPLER_FILTER_MODE:		bTooSmall = SET_VAR(sampler->filter_mode);	break;
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}
}

_cl_sampler::_cl_sampler(cl_context context) : context_resource(context)
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_samplers.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_sampler::~_cl_sampler()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_samplers.erase(this);
	FreeOCL::global_mutex.unlock();
}
