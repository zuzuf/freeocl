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
#ifndef __FREEOCL_MEM_H__
#define __FREEOCL_MEM_H__

#include "freeocl.h"
#include <deque>

namespace FreeOCL
{
	struct mem_call_back
	{
		void (CL_CALLBACK *pfn_notify)(cl_mem memobj,
									   void *user_data);
		void *user_data;
	};
}

struct _cl_mem : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::mutex
{
	void *ptr;
	size_t size;
	cl_mem_flags flags;
	cl_context context;
	cl_mem_object_type mem_type;
	cl_mem parent;
	void *host_ptr;

	_cl_mem();
	~_cl_mem();
	std::deque<FreeOCL::mem_call_back> call_backs;
};

#endif
