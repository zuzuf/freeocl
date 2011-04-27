#ifndef __FREEOCL_COMMANDQUEUE_H__
#define __FREEOCL_COMMANDQUEUE_H__

#include "freeocl.h"
#include "condition.h"
#include <vector>
#include <deque>

namespace FreeOCL
{
	struct command_common
	{
		cl_command_type type;
		cl_event		event;
		cl_uint			num_events_in_wait_list;
		const cl_event	*event_wait_list;
	};

	struct command_read_buffer : public command_common
	{
	};

	struct command_write_buffer : public command_common
	{
	};

	struct command_copy_buffer : public command_common
	{
	};

	struct command_map_buffer : public command_common
	{
	};

	struct command_unmap_buffer : public command_common
	{
	};

	union command
	{
		cl_command_type			type;
		command_common			common;
		command_read_buffer		read_buffer;
		command_write_buffer	write_buffer;
	};
}

struct _cl_command_queue : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::condition
{
	cl_context context;
	cl_device_id device;
	cl_command_queue_properties properties;

	std::deque<FreeOCL::command>	queue;

	bool empty() const;
};

#endif
