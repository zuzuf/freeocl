#include "context.h"

#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_context clCreateContext (const cl_context_properties *properties,
								cl_uint num_devices,
								const cl_device_id *devices,
								void (CL_CALLBACK *pfn_notify)(const char *errinfo,
															   const void *private_info,
															   size_t cb,
															   void *user_data),
								void *user_data,
								cl_int *errcode_ret)
	{
		if (devices == NULL
			|| num_devices == 0
			|| (pfn_notify == NULL && user_data != NULL))
		{
			if (errcode_ret)
				*errcode_ret = CL_INVALID_VALUE;
			return 0;
		}
		cl_context c = NULL;
		try
		{
			c = new _cl_context;
		}
		catch(const std::bad_alloc &e)
		{
			if (errcode_ret)
				*errcode_ret = CL_OUT_OF_HOST_MEMORY;
			return 0;
		}

		for(size_t i = 0 ; i < num_devices ; ++i)
		{
			if (devices[i] != FreeOCL::device)
			{
				if (errcode_ret)
					*errcode_ret = CL_INVALID_DEVICE;
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
						if (errcode_ret)
							*errcode_ret = CL_INVALID_PLATFORM;
						delete c;
						return 0;
					}
					++k;
				}
				if (properties[0] != CL_CONTEXT_PLATFORM || k > 1)
				{
					if (errcode_ret)
						*errcode_ret = CL_INVALID_PROPERTY;
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

		if (errcode_ret)
			*errcode_ret = CL_SUCCESS;

		return c;
	}

	cl_context clCreateContextFromType (const cl_context_properties *properties,
										 cl_device_type device_type,
										 void (CL_CALLBACK *pfn_notify)(const char *errinfo,
																		const void *private_info,
																		size_t cb,
																		void *user_data),
										 void *user_data,
										 cl_int *errcode_ret)
	{
		switch(device_type)
		{
		case CL_DEVICE_TYPE_CPU:
		case CL_DEVICE_TYPE_ALL:
		case CL_DEVICE_TYPE_DEFAULT:
			break;
		case CL_DEVICE_TYPE_GPU:
		case CL_DEVICE_TYPE_ACCELERATOR:
			if (errcode_ret)
				*errcode_ret = CL_DEVICE_NOT_FOUND;
			return 0;
		default:
			if (errcode_ret)
				*errcode_ret = CL_INVALID_DEVICE_TYPE;
			return 0;
		}
		return clCreateContext(properties, 1, &FreeOCL::device, pfn_notify, user_data, errcode_ret);
	}

	cl_int clRetainContext (cl_context context)
	{
		if (!FreeOCL::isValid(context))
			return CL_INVALID_CONTEXT;

		context->retain();
		context->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseContext (cl_context context)
	{
		if (!FreeOCL::isValid(context))
			return CL_INVALID_CONTEXT;

		context->release();
		if (context->get_ref_count() == 0)
			delete context;
		else
			context->unlock();
		return CL_SUCCESS;
	}

	cl_int clGetContextInfo (cl_context context,
							 cl_context_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		if (!FreeOCL::isValid(context))
			return CL_INVALID_CONTEXT;

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
			bTooSmall = FreeOCL::copyMemoryWithinLimits(&(context->devices.front()), sizeof(cl_device_id) * context->devices.size(), param_value_size, param_value, param_value_size_ret);
			break;
		case CL_CONTEXT_PROPERTIES:
			if (!context->properties.empty())
				bTooSmall = FreeOCL::copyMemoryWithinLimits(&(context->devices.front()), sizeof(cl_device_id) * context->devices.size(), param_value_size, param_value, param_value_size_ret);
			else
			{
				if (param_value != NULL && param_value_size >= sizeof(cl_context_properties))
					*((cl_context_properties*)param_value) = 0;
				if (param_value_size_ret)
					*param_value_size_ret = 0;
			}
			break;
		default:
			context->unlock();
			return CL_INVALID_VALUE;
		}

		context->unlock();

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
}
