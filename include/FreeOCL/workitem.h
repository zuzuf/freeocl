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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_WORKITEM_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_WORKITEM_H__
// Built-in work-item functions
inline uint get_work_dim()
{
	return 1;
}

inline size_t get_global_size(uint dimindx)
{

}

inline size_t get_global_id(uint dimindx)
{

}

inline size_t get_local_size(uint dimindx)
{

}

inline size_t get_local_id(uint dimindx)
{

}

inline size_t get_num_groups(uint dimindx)
{

}

inline size_t get_group_id(uint dimindx)
{

}

inline size_t get_global_offset(uint dimindx)
{

}
#endif
