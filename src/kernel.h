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
#ifndef __FREEOCL_KERNEL_H__
#define __FREEOCL_KERNEL_H__

#include "freeocl.h"
#include <string>

struct _cl_kernel : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::mutex, public FreeOCL::valid_flag
{
	cl_program program;
	std::string function_name;

	size_t (*__FCL_info)(size_t, int*);
	void (*__FCL_kernel)(const void*,size_t,size_t*,size_t*,size_t*);
	std::deque<size_t> args_size;
	std::deque<size_t> args_offset;
	std::deque<int> args_type;
	std::vector<char> args_buffer;

	_cl_kernel();
	~_cl_kernel();
};

#endif
