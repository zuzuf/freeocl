#include "event.h"
#include "context.h"
#include "commandqueue.h"

#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_event clCreateUserEvent (cl_context context, cl_int *errcode_ret)
	{
		if (!FreeOCL::isValid(context))
		{
			if (errcode_ret)
				*errcode_ret = CL_INVALID_CONTEXT;
			return 0;
		}

		if (errcode_ret)
			*errcode_ret = CL_SUCCESS;

		cl_event e = new _cl_event;
		e->context = context;
		e->command_queue = 0;
		e->command_type = CL_COMMAND_USER;
		e->status = CL_SUBMITTED;

		context->unlock();
		return e;
	}

	cl_int clSetUserEventStatus (cl_event event, cl_int execution_status)
	{
		if (execution_status != CL_COMPLETE && execution_status >= 0)
			return CL_INVALID_VALUE;
		if (!FreeOCL::isValid(event))
			return CL_INVALID_EVENT;
		if (event->status == CL_COMPLETE || event->status < 0)
		{
			event->unlock();
			return CL_INVALID_OPERATION;
		}
		event->change_status(execution_status);
		event->unlock();
		return CL_SUCCESS;
	}

	cl_int clWaitForEvents (cl_uint num_events, const cl_event *event_list)
	{
		if (num_events == 0 || event_list == NULL)
			return CL_INVALID_VALUE;

		for(size_t i = 0 ; i < num_events ; )
		{
			if (!FreeOCL::isValid(event_list[i]))
				return CL_INVALID_EVENT;
			if (event_list[i]->status < 0)
			{
				event_list[i]->unlock();
				return CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST;
			}
			if (event_list[i]->status == CL_COMPLETE)
			{
				event_list[i]->unlock();
				++i;
				continue;
			}
			event_list[i]->wait_locked();
			event_list[i]->unlock();
		}

		return CL_SUCCESS;
	}

	cl_int clGetEventInfo (cl_event event,
						   cl_event_info param_name,
						   size_t param_value_size,
						   void *param_value,
						   size_t *param_value_size_ret)
	{
		bool bTooSmall = false;
		if (!FreeOCL::isValid(event))
			return CL_INVALID_EVENT;
		switch(param_name)
		{
		case CL_EVENT_COMMAND_QUEUE:				bTooSmall = SET_VAR(event->command_queue);	break;
		case CL_EVENT_CONTEXT:						bTooSmall = SET_VAR(event->context);	break;
		case CL_EVENT_COMMAND_TYPE:					bTooSmall = SET_VAR(event->command_type);	break;
		case CL_EVENT_COMMAND_EXECUTION_STATUS:
			{
				const cl_uint status = event->status;
				bTooSmall = SET_VAR(status);
			}
			break;
		case CL_EVENT_REFERENCE_COUNT:				bTooSmall = SET_VAR(event->get_ref_count());	break;
		default:
			event->unlock();
			return CL_INVALID_VALUE;
		}
		event->unlock();

		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clSetEventCallback (cl_event event,
							   cl_int command_exec_callback_type,
							   void (CL_CALLBACK *pfn_event_notify)(cl_event event,
																	cl_int event_command_exec_status,
																	void *user_data),
							   void *user_data)
	{
		if (command_exec_callback_type != CL_COMPLETE
			|| pfn_event_notify == NULL)
			return CL_INVALID_VALUE;

		if (!FreeOCL::isValid(event))
			return CL_INVALID_EVENT;

		FreeOCL::event_call_back call_back = { pfn_event_notify, user_data };
		event->call_backs[command_exec_callback_type].push_back(call_back);
		event->unlock();

		return CL_SUCCESS;
	}

	cl_int clRetainEvent (cl_event event)
	{
		if (!FreeOCL::isValid(event))
			return CL_INVALID_EVENT;

		event->retain();
		event->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseEvent (cl_event event)
	{
		if (!FreeOCL::isValid(event))
			return CL_INVALID_EVENT;

		event->release();
		if (event->get_ref_count() == 0)
			delete event;
		else
			event->unlock();
		return CL_SUCCESS;
	}
}

void _cl_event::change_status(cl_int new_status)
{
	status = new_status;
	std::deque<FreeOCL::event_call_back> call_backs;
	call_backs.swap(this->call_backs[new_status]);

	unlock();

	if (!call_backs.empty())
		for(std::deque<FreeOCL::event_call_back>::const_iterator i = call_backs.begin() ; i != call_backs.end() ; ++i)
			if (i->pfn_notify)
				i->pfn_notify(this, new_status, i->user_data);

	wakeup();
	command_queue->wakeup();		// If the command queue is waiting for a status change, notify it

	lock();
}

_cl_event::_cl_event()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_event.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_event::~_cl_event()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_event.erase(this);
	FreeOCL::global_mutex.unlock();
}
