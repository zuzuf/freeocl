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
#ifndef __FREEOCL_CONTEXT_H__
#define __FREEOCL_CONTEXT_H__

#include "freeocl.h"
#include <vector>

struct _cl_context : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::mutex, public FreeOCL::valid_flag
{
	_cl_context();
	~_cl_context();

	std::vector<cl_device_id>	devices;
	std::vector<cl_context_properties>	properties;
	void (CL_CALLBACK *pfn_notify)(const char *errinfo,
								   const void *private_info,
								   size_t cb,
								   void *user_data);
	void *user_data;
};

#endif
