/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef __FREEOCL_FREEOCL_H__
#define __FREEOCL_FREEOCL_H__

#include "../config.h"
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif
#include <string>
#include <set>
#include "mutex.h"
#include "dispatch.h"
#include <iostream>

#define MSG(X)	std::cout << #X << std::endl
#define CRASH()	(*((int*)NULL) = 0)

namespace FreeOCL
{
	extern cl_platform_id platform;
	extern cl_device_id device;
	extern std::set<cl_context> valid_contexts;
	extern std::set<cl_command_queue> valid_command_queues;
	extern std::set<cl_mem> valid_mems;
	extern std::set<cl_event> valid_events;
	extern std::set<cl_kernel> valid_kernels;
	extern std::set<cl_program> valid_programs;
	extern mutex global_mutex;
	extern _cl_icd_dispatch dispatch;

	bool isValid(cl_context);
	bool isValid(cl_command_queue);
	bool isValid(cl_mem);
	bool isValid(cl_event);
	bool isValid(cl_program);
	bool isValid(cl_kernel);
	bool isValid(cl_device_id);
	bool isValid(cl_platform_id);

	bool copyMemoryWithinLimits(const void *src, const size_t size, const size_t maxSize, void *dst, size_t *s);

	std::string runCommand(const std::string &cmd);
	long int parseInt(const std::string &s);
	std::string trim(const std::string &s);

	struct icd_table
	{
		struct _cl_icd_dispatch *dispatch;

		inline icd_table() : dispatch(&FreeOCL::dispatch)	{}
	};

	struct ref_counter
	{
		inline ref_counter() : ref_count(1)	{}		// Implicit retain

		inline const cl_uint &get_ref_count() const	{	return ref_count;	}
		inline void retain()	{	++ref_count;	}
		inline void release()	{	--ref_count;	}

	private:
		cl_uint ref_count;
	};

	struct valid_flag
	{
		inline valid_flag() : bValid(true)	{}
		inline bool valid() const	{	return bValid;	}
		inline void invalidate()	{	bValid = false;	}
	private:
		volatile bool bValid;
	};
}

#endif
