#include "commandqueue.h"

#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_command_queue clCreateCommandQueue (cl_context context,
										   cl_device_id device,
										   cl_command_queue_properties properties,
										   cl_int *errcode_ret)
	{

	}

	cl_int clRetainCommandQueue (cl_command_queue command_queue)
	{
		command_queue->lock();
		command_queue->retain();
		command_queue->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseCommandQueue (cl_command_queue command_queue)
	{
		command_queue->lock();
		command_queue->release();
		if (command_queue->get_ref_count() == 0)
			delete command_queue;
		else
			command_queue->unlock();
		return CL_SUCCESS;
	}

	cl_int clGetCommandQueueInfo (cl_command_queue command_queue,
								  cl_command_queue_info param_name,
								  size_t param_value_size,
								  void *param_value,
								  size_t *param_value_size_ret)
	{
		command_queue->lock();

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_QUEUE_CONTEXT:			bTooSmall = SET_VAR(command_queue->context);	break;
		case CL_QUEUE_DEVICE:			bTooSmall = SET_VAR(command_queue->device);	break;
		case CL_QUEUE_REFERENCE_COUNT:	bTooSmall = SET_VAR(command_queue->get_ref_count());	break;
		case CL_QUEUE_PROPERTIES:		bTooSmall = SET_VAR(command_queue->properties);	break;
		default:
			command_queue->unlock();
			return CL_INVALID_VALUE;
		}
		command_queue->unlock();
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clFlush (cl_command_queue command_queue)
	{
		return CL_SUCCESS;
	}

	cl_int clFinish (cl_command_queue command_queue)
	{
		command_queue->lock();
		while(!command_queue->queue.empty())
			command_queue->wait_locked();
		command_queue->unlock();
		return CL_SUCCESS;
	}
}
