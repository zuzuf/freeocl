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
#include "node.h"
#include <sstream>
#include <algorithm>
#include "utils/string.h"

namespace FreeOCL
{
	std::string node::wrap_reserved(const std::string &word)
	{
		const char *cxx_reserved[] = {"and","and_eq",
									  "bitand","bitor",
									  "compl","not","not_eq",
									  "or","or_eq",
									  "xor","xor_eq"};
		// Check if name is a C++ reserved word
		const size_t nb_cxx_reserved = sizeof(cxx_reserved) / sizeof(cxx_reserved[0]);
		if (std::find((const char**)cxx_reserved, cxx_reserved + nb_cxx_reserved, word) < cxx_reserved + nb_cxx_reserved)
			return "__reserved_" + word;
		return word;
	}
}
