#ifndef __FREEOCL_EVENT_H__
#define __FREEOCL_EVENT_H__

#include "freeocl.h"
#include "condition.h"
#include <vector>
#include <deque>
#include <map>

namespace FreeOCL
{
	struct event_call_back
	{
		void (CL_CALLBACK *pfn_notify)(cl_event event,
									   cl_int event_command_exec_status,
									   void *user_data);
		void *user_data;
	};
}

struct _cl_event : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::condition
{
	_cl_event();
	~_cl_event();

	cl_context context;
	cl_command_queue command_queue;
	cl_command_type command_type;
	volatile cl_int status;

	std::map<cl_int, std::deque<FreeOCL::event_call_back> > call_backs;

	void change_status(cl_int new_status);
};

#endif
