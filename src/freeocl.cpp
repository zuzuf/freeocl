#include "freeocl.h"
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include "context.h"
#include "commandqueue.h"
#include "mem.h"
#include "event.h"

namespace FreeOCL
{
	std::set<cl_context> valid_contexts;
	std::set<cl_command_queue> valid_command_queues;
	std::set<cl_mem> valid_mem;
	std::set<cl_event> valid_event;
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
		const bool r = valid_mem.count(m) != 0;
		if (r)
			m->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_event e)
	{
		global_mutex.lock();
		const bool r = valid_event.count(e) != 0;
		if (r)
			e->lock();
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
