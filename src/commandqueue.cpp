#include "commandqueue.h"
#include "event.h"
#include "mem.h"
#include <cstring>

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
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;

		command_queue->retain();
		command_queue->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseCommandQueue (cl_command_queue command_queue)
	{
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;

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
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;

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
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;
		command_queue->unlock();

		return CL_SUCCESS;
	}

	cl_int clFinish (cl_command_queue command_queue)
	{
		if (!FreeOCL::isValid(command_queue))
			return CL_INVALID_COMMAND_QUEUE;

		while(!command_queue->empty())
			command_queue->wait_locked();
		command_queue->unlock();
		return CL_SUCCESS;
	}
}

_cl_command_queue::_cl_command_queue() : q_thread(this), b_stop(false)
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_command_queues.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_command_queue::~_cl_command_queue()
{
	b_stop = true;
	wakeup();
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_command_queues.erase(this);
	FreeOCL::global_mutex.unlock();
}

void _cl_command_queue::enqueue(const FreeOCL::command &cmd)
{
	q_mutex.lock();
	queue.push_back(cmd);
	q_mutex.unlock();

	q_thread.start();		// Make sure the scheduler is running
	wakeup();
}

bool _cl_command_queue::empty()
{
	q_mutex.lock();
	const bool b = queue.empty();
	q_mutex.unlock();
	return b;
}

int _cl_command_queue::proc()
{
	while(!b_stop)
	{
		q_mutex.lock();
		if (queue.empty())
		{
			q_mutex.unlock();
			wait();
			q_mutex.lock();
		}

		FreeOCL::command cmd = queue.front();
		queue.pop_front();
		q_mutex.unlock();

		if (b_stop)
			break;

		if (cmd.common.event)
		{
			cmd.common.event->lock();
			cmd.common.event->change_status(CL_RUNNING);
			cmd.common.event->unlock();
		}

		switch(cmd.type)
		{
		case CL_COMMAND_READ_BUFFER:
			memcpy(cmd.read_buffer.ptr, (char*)cmd.read_buffer.buffer->ptr + cmd.read_buffer.offset, cmd.read_buffer.cb);
			break;
		case CL_COMMAND_WRITE_BUFFER:
			memcpy((char*)cmd.write_buffer.buffer->ptr + cmd.write_buffer.offset, cmd.write_buffer.ptr, cmd.write_buffer.cb);
			break;
		case CL_COMMAND_COPY_BUFFER:
			memcpy((char*)cmd.copy_buffer.dst_buffer->ptr + cmd.copy_buffer.dst_offset, (char*)cmd.copy_buffer.src_buffer + cmd.copy_buffer.src_offset, cmd.copy_buffer.cb);
			break;
		}

		if (cmd.common.event)
		{
			cmd.common.event->lock();
			cmd.common.event->change_status(CL_COMPLETE);
			cmd.common.event->unlock();
			clReleaseEvent(cmd.common.event);
		}
	}
	return 0;
}

int _cl_command_queue::thread::proc()
{
	return command_queue->proc();
}
