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
#include "sampler.h"
#include "context.h"

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
		SET_RET(CL_INVALID_OPERATION);
		return NULL;
	}

	cl_int clRetainSamplerFCL (cl_sampler sampler)
	{
		MSG(clRetainSamplerFCL);
		return CL_INVALID_SAMPLER;
	}

	cl_int clReleaseSamplerFCL (cl_sampler sampler)
	{
		MSG(clReleaseSamplerFCL);
		return CL_INVALID_SAMPLER;
	}

	cl_int clGetSamplerInfoFCL (cl_sampler sampler,
							 cl_sampler_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		MSG(clGetSamplerInfoFCL);
		return CL_INVALID_SAMPLER;
	}
}
