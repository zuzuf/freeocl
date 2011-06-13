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

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <iostream>
#include <sstream>
#include <iomanip>
#include <signal.h>

#include "errors.h"

inline std::string memSuffix(const size_t s)
{
	std::stringstream buf;
	buf.precision(3);
	if (s >= 0x10000000000LLU)
		buf << (double)(s >> 30) / 1024.0 << "TB";
	else if (s >= 0x40000000LU)
		buf << (double)(s >> 20) / 1024.0 << "GB";
	else if (s >= 0x100000LU)
		buf << (double)(s >> 10) / 1024.0 << "MB";
	else if (s >= 0x400LU)
		buf << (double)s / 1024.0 << "KB";
	else
		buf << s << "B";
	return buf.str();
}

#define STRINGIFY(X)	#X

const char *source_code =
"__kernel void hello(__global char *out, __local char *test)\n"
"{\n"
"	int;\n"
"	const size_t i = get_global_id(0);\n"
"	__constant char *msg = \"hello world\";\n"
"	test[i] = msg[11 - i];\n"
"	barrier(CLK_LOCAL_MEM_FENCE);\n"
"	out[i] = test[11 - i];\n"
""
"}\n";

int main(int argc, const char **argv)
{
	struct sigaction s;
	s.sa_handler = signal_handler;
	sigaction(SIGSEGV, &s, NULL);
	try
	{
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		std::cout << platforms.size() << " platform" << (platforms.size() > 1 ? "s" : "") << " found:" << std::endl;
		size_t id = 0;
		for(size_t i = 0 ; i < platforms.size() ; ++i)
		{
			const cl::Platform &p = platforms[i];
			std::cout
					<< "\tplatform " << i << ':' << std::endl
					<< "\t\tname: " << p.getInfo<CL_PLATFORM_NAME>() << std::endl
					<< "\t\tprofile: " << p.getInfo<CL_PLATFORM_PROFILE>() << std::endl
					<< "\t\tversion: " << p.getInfo<CL_PLATFORM_VERSION>() << std::endl
					<< "\t\tvendor: " << p.getInfo<CL_PLATFORM_VENDOR>() << std::endl
					<< "\t\textensions: " << p.getInfo<CL_PLATFORM_EXTENSIONS>() << std::endl;
			if (p.getInfo<CL_PLATFORM_NAME>() == "FreeOCL")
				id = i;

			std::vector<cl::Device> devices;
			p.getDevices(CL_DEVICE_TYPE_ALL, &devices);

			std::cout << "\t\t" << devices.size() << " device" << (devices.size() > 1 ? "s" : "") << " found:" << std::endl;
			for(size_t j = 0 ; j < devices.size() ; ++j)
			{
				cl::Device &dev = devices[j];
				std::cout
						<< "\t\t\tdevice " << j << ':' << std::endl
						<< "\t\t\t\tname: " << dev.getInfo<CL_DEVICE_NAME>() << std::endl
						<< "\t\t\t\tversion: " << dev.getInfo<CL_DEVICE_VERSION>() << std::endl
						<< "\t\t\t\tvendor: " << dev.getInfo<CL_DEVICE_VENDOR>() << std::endl
						<< "\t\t\t\tmemory: " << memSuffix(dev.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()) << std::endl
						<< "\t\t\t\tendianess: " << (dev.getInfo<CL_DEVICE_ENDIAN_LITTLE>() ? "little endian" : "big endian") << std::endl
						<< "\t\t\t\textensions: " << dev.getInfo<CL_DEVICE_EXTENSIONS>() << std::endl;
			}
		}

		if (platforms.empty())
			return 0;

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
		}

		cl::Platform &platform = platforms[id];
		std::cout << std::endl << "platform selected: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl << std::endl;

		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

		// Create a context
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platform)(), 0};
		cl::Context context(devices, properties);

		cl_int err;

		// Create a command queue
		cl::CommandQueue queue(context, devices[0], 0, &err);

		cl::Program::Sources sources;
		sources.push_back(std::make_pair(source_code, strlen(source_code)));
		cl::Program program(context, sources);
		try {
			program.build(devices);
		} catch(...)	{}
		std::cout << "source code: " << std::endl << source_code << std::endl;
		std::cout << "build status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices.front()) << std::endl;
		std::cout << "build log: " << std::endl << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << std::endl;

		cl::Buffer buffer(context, CL_MEM_READ_WRITE, 32);
		cl::Kernel k(program, "hello");
		k.setArg(0, buffer());
		k.setArg(1, 256, NULL);
		queue.enqueueNDRangeKernel(k, cl::NDRange(0), cl::NDRange(12), cl::NDRange(12));

		queue.finish();
		char *p = (char*)queue.enqueueMapBuffer(buffer, true, CL_MEM_READ_ONLY, 0, 16);
		std::cout << "p = " << p << std::endl;
		queue.enqueueUnmapMemObject(buffer, p);
	}
	catch(cl::Error err)
	{
		std::cerr << "error: " << err.what() << '(' << getErrorAsString(err.err()) << ')' << std::endl;
		return -1;
	}

	return 0;
}
