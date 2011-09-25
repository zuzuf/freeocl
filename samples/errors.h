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
#ifndef __ERRORS_H__
#define __ERRORS_H__

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl_platform.h>
#else
#include <CL/cl_platform.h>
#endif

const char *get_error_as_string(cl_int err);

void init_signal_handler();
void signal_handler(int);

#endif
