#include "sampler.h"

#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_sampler clCreateSampler (cl_context context,
								cl_bool normalized_coords,
								cl_addressing_mode addressing_mode,
								cl_filter_mode filter_mode,
								cl_int *errcode_ret)
	{
		SET_RET(CL_INVALID_OPERATION);
		return NULL;
	}

	cl_int clRetainSampler (cl_sampler sampler)
	{
		return CL_INVALID_SAMPLER;
	}

	cl_int clReleaseSampler (cl_sampler sampler)
	{
		return CL_INVALID_SAMPLER;
	}

	cl_int clGetSamplerInfo (cl_sampler sampler,
							 cl_sampler_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		return CL_INVALID_SAMPLER;
	}
}
