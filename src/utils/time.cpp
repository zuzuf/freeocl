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
#include "time.h"

namespace FreeOCL
{
	size_t us_timer()
	{
#if defined(__APPLE__) || defined(__MACOSX)
		return (ns_timer() + 500LU) / 1000LU;
#else
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_nsec / 1000LU + ts.tv_sec * 1000000LU;
#endif
	}

	size_t ms_timer()
	{
#if defined(__APPLE__) || defined(__MACOSX)
		return (ns_timer() + 500000LU) / 1000000LU;
#else
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_nsec / 1000000U + ts.tv_sec * 1000LU;
#endif
	}

	size_t s_timer()
	{
#if defined(__APPLE__) || defined(__MACOSX)
		return (ns_timer() + 500000000LU) / 1000000000LU;
#else
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_sec;
#endif
	}
}
