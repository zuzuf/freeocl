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
#include "parser.h"
#include "freeocl.h"
#include <CL/cl_ext.h>

namespace FreeOCL
{
	void parser::parse_pragma(const std::string &line)
	{
		std::deque<std::string> words = FreeOCL::split(line, " \t");
		// Check if it's a pragma
		if (words.empty() || words.front() != "pragma" || words.size() < 2)
			return;
		if (words[1] != "OPENCL")
		{
			warning("Unknown pragma");
			return;
		}
		if (words.size() < 3)
		{
			warning("Incorrect pragma");
			return;
		}
		if (words[2] == "EXTENSION")
		{
			if (words.size() < 4)
			{
				warning("Missing extension name");
				return;
			}
			if (*words[3].rbegin() == ':')
			{
				words[3].resize(words[3].size() - 1);
				words.insert(words.begin() + 4, std::string(":"));
			}
			if (words.size() < 5)
			{
				warning("Expected ':' after extension name");
				return;
			}
			if (words.size() < 6)
			{
				warning("Missing behavior (enable/disable)");
				return;
			}
			if (words[5] != "enable" && words[5] != "disable")
			{
				error("Expected enable or disable");
				return;
			}
			const bool b_enable = words[5] == "enable";
			if (words[3] == "cl_khr_icd")
			{
				active_extensions[_cl_khr_icd] = b_enable;
			}
			else if (words[3] == "cl_khr_global_int32_base_atomics")
			{
				active_extensions[_cl_khr_global_int32_base_atomics] = b_enable;
			}
			else if (words[3] == "cl_khr_global_int32_extended_atomics")
			{
				active_extensions[_cl_khr_global_int32_extended_atomics] = b_enable;
			}
			else if (words[3] == "cl_khr_local_int32_base_atomics")
			{
				active_extensions[_cl_khr_local_int32_base_atomics] = b_enable;
			}
			else if (words[3] == "cl_khr_local_int32_extended_atomics")
			{
				active_extensions[_cl_khr_local_int32_extended_atomics] = b_enable;
			}
			else if (words[3] == "cl_khr_byte_addressable_store")
			{
				active_extensions[_cl_khr_byte_addressable_store] = b_enable;
			}
			else if (words[3] == "cl_khr_fp64")
			{
				active_extensions[_cl_khr_fp64] = b_enable;
			}
			else if (words[3] == "cl_khr_int64_base_atomics")
			{
				active_extensions[_cl_khr_int64_base_atomics] = b_enable;
			}
			else if (words[3] == "cl_khr_int64_extended_atomics")
			{
				active_extensions[_cl_khr_int64_extended_atomics] = b_enable;
			}
			else if (words[3] == "all")
			{
				active_extensions.set();
			}
		}
	}
}
