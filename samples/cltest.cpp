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
#define __CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <stdlib.h>

#include "errors.h"

using namespace std;

int main(int argc, const char **argv)
{
	init_signal_handler();
	try
	{
		vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		size_t id = 0;
		for(size_t i = 0 ; i < platforms.size() ; ++i)
		{
			const cl::Platform &p = platforms[i];
			if (p.getInfo<CL_PLATFORM_NAME>() == "FreeOCL")
				id = i;

			std::vector<cl::Device> devices;
			p.getDevices(CL_DEVICE_TYPE_ALL, &devices);
		}

		if (platforms.empty())
			return 0;

		string filename;
		string compiler_args;
		for(int i = 1 ; i < argc ; ++i)
		{
			if (!strcmp(argv[i], "--platform") || !strcmp(argv[i], "-p"))
			{
				if (i + 1 < argc)
				{
					++i;
					id = strtol(argv[i], NULL, 0);
				}
				continue;
			}
			if (!strcmp(argv[i], "-I"))
			{
				if (i + 1 < argc)
				{
					++i;
					compiler_args += " -I";
					compiler_args += argv[i];
				}
				continue;
			}
			if (!strncmp(argv[i], "-I", 2))
			{
				compiler_args += ' ';
				compiler_args += argv[i];
				continue;
			}
			if (!strncmp(argv[i], "-cl", 3))
			{
				compiler_args += ' ';
				compiler_args += argv[i];
				continue;
			}
			filename = argv[i];
		}

		if (filename.empty())
		{
			cerr << "syntax:" << endl
				 << '\t' << argv[0] << " [--platform/-p <id>] filename" << endl << endl;
			return 0;
		}

		cl::Platform &platform = platforms[id];
		cout << endl << "platform selected: " << platform.getInfo<CL_PLATFORM_NAME>() << endl << endl;

		vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

		// Create a context
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0};
		cl::Context context(devices, properties);

		cl_int err;

		fstream src_file(filename.c_str(), fstream::in | fstream::binary);
		if (!src_file.is_open())
			throw runtime_error("could not read source file '" + filename + "'");

		src_file.seekg(0, ios_base::end);
		size_t len = src_file.tellg();
		src_file.seekg(0);
		vector<char> source_code;
		source_code.resize(len);
		src_file.read(source_code.data(), len);
		src_file.close();
		source_code.push_back(0);

		cl::Program::Sources sources;
		sources.push_back(make_pair(source_code.data(), len));
		cl::Program program(context, sources);
		try
		{
			program.build(devices, compiler_args.c_str());
		} catch(...)	{}
		cout << "source code: " << endl << source_code.data() << endl;
		cout << "build status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices.front()) << endl;
		cout << "build log: " << endl << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << endl;
	}
	catch(cl::Error err)
	{
		cerr << "error: " << err.what() << '(' << get_error_as_string(err.err()) << ')' << endl;
		return -1;
	}

	return 0;
}
