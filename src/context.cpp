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
#include "context.h"
#include "device.h"
#include "prototypes.h"

// Required by _cl_context::~_cl_context
#include "event.h"
#include "mem.h"
#include "program.h"
#include "sampler.h"

#define SET_VAR(X)	FreeOCL::copy_memory_within_limits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_context clCreateContextFCL (const cl_context_properties *properties,
								   cl_uint num_devices,
								   const cl_device_id *devices,
								   void (CL_CALLBACK *pfn_notify)(const char *errinfo,
																  const void *private_info,
																  size_t cb,
																  void *user_data),
								   void *user_data,
								   cl_int *errcode_ret)
	{
		MSG(clCreateContextFCL);
		if (devices == NULL
			|| num_devices == 0
			|| (pfn_notify == NULL && user_data != NULL))
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		cl_context c = NULL;
		try
		{
			c = new _cl_context;
		}
		catch(const std::bad_alloc &e)
		{
			SET_RET(CL_OUT_OF_HOST_MEMORY);
			return 0;
		}

		c->devices.reserve(num_devices);
		for(size_t i = 0 ; i < num_devices ; ++i)
		{
			if (devices[i] != FreeOCL::device)
			{
				SET_RET(CL_INVALID_DEVICE);
				delete c;
				return 0;
			}
			c->devices.push_back(devices[i]);
		}

		if (properties)
		{
			int k = 0;
			while(*properties != 0)
			{
				if (properties[0] == CL_CONTEXT_PLATFORM)
				{
					if (((cl_platform_id*)properties)[1] != FreeOCL::platform)
					{
						SET_RET(CL_INVALID_PLATFORM);
						delete c;
						return 0;
					}
					++k;
				}
				if (properties[0] != CL_CONTEXT_PLATFORM || k > 1)
				{
					SET_RET(CL_INVALID_PROPERTY);
					delete c;
					return 0;
				}

				c->properties.push_back(*properties);
				++properties;
				c->properties.push_back(*properties);
				++properties;
			}
		}

		c->pfn_notify = pfn_notify;
		c->user_data = user_data;

		SET_RET(CL_SUCCESS);

		return c;
	}

	cl_context clCreateContextFromTypeFCL (const cl_context_properties *properties,
										 cl_device_type device_type,
										 void (CL_CALLBACK *pfn_notify)(const char *errinfo,
																		const void *private_info,
																		size_t cb,
																		void *user_data),
										 void *user_data,
										 cl_int *errcode_ret)
	{
		MSG(clCreateContextFromType);
		cl_platform_id platform = 0;
		if (!properties)
		{
			const cl_context_properties *prop = properties;
			while(!*prop)
			{
				switch(*prop)
				{
				case CL_CONTEXT_PLATFORM:
					if (platform != NULL)
					{
						SET_RET(CL_INVALID_PROPERTY);
						return 0;
					}
					++prop;
					platform = *((cl_platform_id*)prop);
					break;
				default:
					SET_RET(CL_INVALID_PROPERTY);
					return 0;
				}
				++prop;
			}
		}

		switch(device_type)
		{
		case CL_DEVICE_TYPE_CPU:
		case CL_DEVICE_TYPE_ALL:
		case CL_DEVICE_TYPE_DEFAULT:
			break;
		case CL_DEVICE_TYPE_GPU:
		case CL_DEVICE_TYPE_ACCELERATOR:
			break;
			SET_RET(CL_DEVICE_NOT_AVAILABLE);
			return 0;
		default:
			SET_RET(CL_INVALID_DEVICE_TYPE);
			return 0;
		}
		return clCreateContext(properties, 1, &FreeOCL::device, pfn_notify, user_data, errcode_ret);
	}

	cl_int clRetainContextFCL (cl_context context)
	{
		MSG(clRetainContextFCL);
		if (!FreeOCL::is_valid(context))
			return CL_INVALID_CONTEXT;

		context->retain();
		context->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseContextFCL (cl_context context)
	{
		MSG(clReleaseContextFCL);
		if (!FreeOCL::is_valid(context))
			return CL_INVALID_CONTEXT;

		context->release();
		if (context->get_ref_count() == 0)
		{
			context->invalidate();
			context->unlock();
			delete context;
		}
		else
			context->unlock();
		return CL_SUCCESS;
	}

	cl_int clGetContextInfoFCL (cl_context context,
							 cl_context_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		MSG(clGetContextInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(context))
			return CL_INVALID_CONTEXT;
		unlock.handle(context);

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_CONTEXT_REFERENCE_COUNT:
			bTooSmall = SET_VAR(context->get_ref_count());
			break;
		case CL_CONTEXT_NUM_DEVICES:
			{
				const cl_uint nb = context->devices.size();
				bTooSmall = SET_VAR(nb);
			}
			break;
		case CL_CONTEXT_DEVICES:
			bTooSmall = FreeOCL::copy_memory_within_limits(&(context->devices.front()), sizeof(cl_device_id) * context->devices.size(), param_value_size, param_value, param_value_size_ret);
			break;
		case CL_CONTEXT_PROPERTIES:
			if (!context->properties.empty())
				bTooSmall = FreeOCL::copy_memory_within_limits(&(context->devices.front()), sizeof(cl_device_id) * context->devices.size(), param_value_size, param_value, param_value_size_ret);
			else
			{
				if (param_value != NULL && param_value_size >= sizeof(cl_context_properties))
					*((cl_context_properties*)param_value) = 0;
				if (param_value_size_ret)
					*param_value_size_ret = 0;
			}
			break;
		default:
			return CL_INVALID_VALUE;
		}

		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}
}

_cl_context::_cl_context()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_contexts.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_context::~_cl_context()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_contexts.erase(this);
	FreeOCL::global_mutex.unlock();

	lock();
	FreeOCL::set<FreeOCL::context_resource*> resources = this->resources;
	unlock();
	for(FreeOCL::set<FreeOCL::context_resource*>::iterator it = resources.begin(), end = resources.end() ; it != end ; ++it)
	{
		FreeOCL::context_resource *ptr = *it;
#define IMPLEMENT_FOR_TYPE(type)\
		if (FreeOCL::is_valid(static_cast<type>(ptr)))\
		{\
			delete static_cast<type>(ptr);\
			continue;\
		}
		IMPLEMENT_FOR_TYPE(cl_event);
		IMPLEMENT_FOR_TYPE(cl_mem);
		IMPLEMENT_FOR_TYPE(cl_program);
		IMPLEMENT_FOR_TYPE(cl_sampler);
#undef IMPLEMENT_FOR_TYPE
	}
}
