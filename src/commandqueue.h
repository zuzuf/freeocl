#ifndef __FREEOCL_COMMANDQUEUE_H__
#define __FREEOCL_COMMANDQUEUE_H__

#include "freeocl.h"
#include "condition.h"
#include <vector>
#include <deque>

namespace FreeOCL
{
	struct command_read_buffer
	{
		cl_command_type type;
	};

	struct command_write_buffer
	{
		cl_command_type type;
	};

	struct command_copy_buffer
	{
		cl_command_type type;
	};

	struct command_map_buffer
	{
		cl_command_type type;
	};

	struct command_unmap_buffer
	{
		cl_command_type type;
	};

	union command
	{
		cl_command_type	type;
		command_read_buffer	read_buffer;
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
