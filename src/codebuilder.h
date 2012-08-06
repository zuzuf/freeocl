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
#ifndef __FREEOCL_CODEBUILDER_H__
#define __FREEOCL_CODEBUILDER_H__

#include "FreeOCL/config.h"
#include <string>
#include <cstdio>
#include <vector>
#include "utils/set.h"
#include "utils/map.h"

namespace FreeOCL
{
	// Return a string to the .so/.o file built
	// In case an error occurs, it returns an empty string
	// In any case it'll write logs
	std::string build_program(const std::string &options,
							  const std::string &code,
							  std::stringstream &log,
							  FreeOCL::set<std::string> &kernels,
							  bool &b_valid_options,
							  const bool b_compile_only = false,
							  const FreeOCL::map<std::string, std::string> &headers = FreeOCL::map<std::string, std::string>(),
							  std::string *temporary_filename = NULL);

	// Return the output of the C preprocessor on the parameter 'code'
	std::string preprocess_code(const std::string &code,
								const std::vector<std::pair<std::string, std::string> > &options,
								std::stringstream &log,
								const std::vector<std::string> &include_paths,
								const map<std::string, std::string> &headers);

	// Validate the code and does the OpenCL C -> C++ conversion
	std::string validate_code(const std::string &code,
							  std::stringstream &log,
							  FreeOCL::set<std::string> &kernels,
							  const bool b_debug_mode);

	// Return a string to the .so/.a file built
	// In case an error occurs, it returns an empty string
	// In any case it'll write logs
	std::string link_program(const std::string &options,
							 const std::vector<std::string> &files_to_link,
							  std::stringstream &log,
							  bool &b_valid_options);
}

#endif
