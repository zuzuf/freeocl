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
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl_ext.h>
#else
#include <CL/cl_ext.h>
#endif

#include "errors.h"
#include <cstdlib>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <unistd.h>

#define IMPLEMENT(X)	case X:	return #X

const char *get_error_as_string(cl_int err)
{
	switch(err)
	{
	IMPLEMENT(CL_PLATFORM_NOT_FOUND_KHR);
	IMPLEMENT(CL_SUCCESS);
	IMPLEMENT(CL_DEVICE_NOT_FOUND);
	IMPLEMENT(CL_DEVICE_NOT_AVAILABLE);
	IMPLEMENT(CL_COMPILER_NOT_AVAILABLE);
	IMPLEMENT(CL_MEM_OBJECT_ALLOCATION_FAILURE);
	IMPLEMENT(CL_OUT_OF_RESOURCES);
	IMPLEMENT(CL_OUT_OF_HOST_MEMORY);
	IMPLEMENT(CL_PROFILING_INFO_NOT_AVAILABLE);
	IMPLEMENT(CL_MEM_COPY_OVERLAP);
	IMPLEMENT(CL_IMAGE_FORMAT_MISMATCH);
	IMPLEMENT(CL_IMAGE_FORMAT_NOT_SUPPORTED);
	IMPLEMENT(CL_BUILD_PROGRAM_FAILURE);
	IMPLEMENT(CL_MAP_FAILURE);
	IMPLEMENT(CL_MISALIGNED_SUB_BUFFER_OFFSET);
	IMPLEMENT(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);

	IMPLEMENT(CL_INVALID_VALUE);
	IMPLEMENT(CL_INVALID_DEVICE_TYPE);
	IMPLEMENT(CL_INVALID_PLATFORM);
	IMPLEMENT(CL_INVALID_DEVICE);
	IMPLEMENT(CL_INVALID_CONTEXT);
	IMPLEMENT(CL_INVALID_QUEUE_PROPERTIES);
	IMPLEMENT(CL_INVALID_COMMAND_QUEUE);
	IMPLEMENT(CL_INVALID_HOST_PTR);
	IMPLEMENT(CL_INVALID_MEM_OBJECT);
	IMPLEMENT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
	IMPLEMENT(CL_INVALID_IMAGE_SIZE);
	IMPLEMENT(CL_INVALID_SAMPLER);
	IMPLEMENT(CL_INVALID_BINARY);
	IMPLEMENT(CL_INVALID_BUILD_OPTIONS);
	IMPLEMENT(CL_INVALID_PROGRAM);
	IMPLEMENT(CL_INVALID_PROGRAM_EXECUTABLE);
	IMPLEMENT(CL_INVALID_KERNEL_NAME);
	IMPLEMENT(CL_INVALID_KERNEL_DEFINITION);
	IMPLEMENT(CL_INVALID_KERNEL);
	IMPLEMENT(CL_INVALID_ARG_INDEX);
	IMPLEMENT(CL_INVALID_ARG_VALUE);
	IMPLEMENT(CL_INVALID_ARG_SIZE);
	IMPLEMENT(CL_INVALID_KERNEL_ARGS);
	IMPLEMENT(CL_INVALID_WORK_DIMENSION);
	IMPLEMENT(CL_INVALID_WORK_GROUP_SIZE);
	IMPLEMENT(CL_INVALID_WORK_ITEM_SIZE);
	IMPLEMENT(CL_INVALID_GLOBAL_OFFSET);
	IMPLEMENT(CL_INVALID_EVENT_WAIT_LIST);
	IMPLEMENT(CL_INVALID_EVENT);
	IMPLEMENT(CL_INVALID_OPERATION);
	IMPLEMENT(CL_INVALID_GL_OBJECT);
	IMPLEMENT(CL_INVALID_BUFFER_SIZE);
	IMPLEMENT(CL_INVALID_MIP_LEVEL);
	IMPLEMENT(CL_INVALID_GLOBAL_WORK_SIZE);
	IMPLEMENT(CL_INVALID_PROPERTY);
	default:
		return "unknown error";
	}
	return NULL;
}

void init_signal_handler()
{
	struct sigaction s;
	memset(&s, 0, sizeof(struct sigaction));
	s.sa_handler = signal_handler;
	sigaction(SIGSEGV, &s, NULL);
}

void signal_handler(int)
{
	pid_t mypid = getpid();
	std::stringstream cmd;
	cmd << "gdb --pid " << mypid << " -ex \"bt\"";
	const int _ = system(cmd.str().c_str());
	exit(-1);
}
