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
#include "freeocl.h"
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include "context.h"
#include "commandqueue.h"
#include "mem.h"
#include "event.h"
#include "kernel.h"
#include "program.h"

namespace FreeOCL
{
	std::set<cl_context> valid_contexts;
	std::set<cl_command_queue> valid_command_queues;
	std::set<cl_mem> valid_mems;
	std::set<cl_event> valid_events;
	std::set<cl_kernel> valid_kernels;
	std::set<cl_program> valid_programs;
	mutex global_mutex;

	bool isValid(cl_context c)
	{
		global_mutex.lock();
		const bool r = valid_contexts.count(c) != 0;
		if (r)
			c->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_command_queue q)
	{
		global_mutex.lock();
		const bool r = valid_command_queues.count(q) != 0;
		if (r)
			q->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_mem m)
	{
		global_mutex.lock();
		const bool r = valid_mems.count(m) != 0;
		if (r)
			m->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_event e)
	{
		global_mutex.lock();
		const bool r = valid_events.count(e) != 0;
		if (r)
			e->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_kernel k)
	{
		global_mutex.lock();
		const bool r = valid_kernels.count(k) != 0;
		if (r)
			k->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_program p)
	{
		global_mutex.lock();
		const bool r = valid_programs.count(p) != 0;
		if (r)
			p->lock();
		global_mutex.unlock();
		return r;
	}

	bool copyMemoryWithinLimits(const void *src, const size_t size, const size_t maxSize, void *dst, size_t *s)
	{
		if (s != NULL)
			*s = size;
		if (dst != NULL)
			memcpy(dst, src, std::min(size, maxSize));
		return size > maxSize;
	}

	std::string runCommand(const std::string &cmd)
	{
		std::string result;
		FILE *file = popen(cmd.c_str(), "r");
		if (!file)
			return result;

		int c;
		while((c = fgetc(file)) != -1 && !feof(file))
			result += (char)c;
		pclose(file);

		return result;
	}

	long int parseInt(const std::string &s)
	{
		return strtol(s.c_str(), NULL, 10);
	}

	std::string trim(const std::string &s)
	{
		if (s.empty())
			return s;
		const size_t start = s.find_first_not_of(" \t\n\r");
		const size_t end = s.find_last_not_of(" \t\n\r");
		if (start == end)
			return std::string();
		return s.substr(start, end - start + 1);
	}
}
