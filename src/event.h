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
#ifndef __FREEOCL_EVENT_H__
#define __FREEOCL_EVENT_H__

#include "freeocl.h"
#include "utils/condition.h"
#include <deque>
#include <utils/map.h>

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

struct _cl_event : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::condition, public FreeOCL::valid_flag, public FreeOCL::context_resource
{
	_cl_event(cl_context);
	~_cl_event();

	cl_command_queue command_queue;
	cl_command_type command_type;
	volatile cl_int status;

	FreeOCL::map<cl_int, std::deque<FreeOCL::event_call_back> > call_backs;

	void change_status(cl_int new_status);
};

#endif
