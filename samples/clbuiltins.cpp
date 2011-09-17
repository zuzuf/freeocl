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
#include <fstream>
#include <iomanip>
#include <signal.h>
#include <stdexcept>
#include <algorithm>

#include "errors.h"

using namespace std;

namespace
{
	vector<cl::Context> contexts;
	vector<cl::CommandQueue> queues;
	vector<cl::Buffer> buffers;
}

void test_function(const string &function_name)
{
	cout << "testing '" << function_name << '\'';

	const string source_code =
			"__kernel void test1(__global float *out, float in)\n"
			"{\n"
			"*out = " + function_name + "(in);\n"
			"}";
//			"__kernel void test2(__global float2 *out, float in)\n"
//			"{\n"
//			"*out = " + function_name + "((float2)(in, in));\n"
//			"}"
//			"__kernel void test3(__global float3 *out, float in)\n"
//			"{\n"
//			"*out = " + function_name + "((float3)(in, in, in));\n"
//			"}"
//			"__kernel void test4(__global float4 *out, float in)\n"
//			"{\n"
//			"*out = " + function_name + "((float4)(in, in, in, in));\n"
//			"}"
//			"__kernel void test8(__global float8 *out, float in)\n"
//			"{\n"
//			"*out = " + function_name + "((float8)(in, in, in, in, in, in, in, in));\n"
//			"}"
//			"__kernel void test16(__global float16 *out, float in)\n"
//			"{\n"
//			"*out = " + function_name + "((float16)(in, in, in, in, in, in, in, in, in, in, in, in, in, in, in, in));\n"
//			"}";

	const float v = float(double(rand()) / RAND_MAX);
	vector<float> results;
	vector<cl::CommandQueue>::iterator queue = queues.begin();
	vector<cl::Buffer>::iterator buffer = buffers.begin();
	for(vector<cl::Context>::iterator context = contexts.begin()
		; context != contexts.end()
		; ++context, ++queue, ++buffer)
	{
		cl::Program::Sources sources;
		sources.push_back(make_pair(source_code.data(), source_code.size()));
		cl::Program program(*context, sources);
		vector<cl::Device> devices = context->getInfo<CL_CONTEXT_DEVICES>();
		try
		{
			program.build(devices);
		}
		catch(...)
		{
			cerr << "error:" << endl;
			cerr << "source code: " << endl << source_code.data() << endl;
			cerr << "build status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices.front()) << endl;
			cerr << "build log: " << endl << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << endl;
		}

		float data[16];
		const char *fname[] = { "test1", "test2", "test3", "test4", "test8", "test16" };
		size_t dim[] = { 1, 2, 3, 4, 8, 16 };
		for(size_t i = 0 ; i < 1 ; ++i)
		{
			cl::Kernel kernel(program, fname[i]);
			cl::KernelFunctor f = kernel.bind(*queue, cl::NDRange(1), cl::NDRange(1));
			f(*buffer, v);
			queue->finish();
			queue->enqueueReadBuffer(*buffer, true, 0, sizeof(data), &data[0]);
			for(size_t j = 0 ; j < dim[i] ; ++j)
				results.push_back(data[j]);

			cout << '.';
		}
	}
	cout << endl;
	const vector<float> copy = results;
	vector<float>::iterator end = unique(results.begin(), results.end());
	if (end - results.begin() > 1)
	{
		cerr << '[' << function_name << "] outputs don't match : " << endl;
		for(size_t i = 0 ; i < copy.size() ; ++i)
		{
			if (i)
				cerr << ',';
			cerr << copy[i];
		}
		cerr << endl;
	}
}

int main(int argc, const char **argv)
{
	cl_device_type device_type = CL_DEVICE_TYPE_CPU;

	for(size_t i = 0 ; i < argc ; ++i)
	{
		if (!strcmp(argv[i], "--devices"))
		{
			++i;
			if (i < argc)
			{
				if (!strcasecmp(argv[i], "cpu"))
					device_type = CL_DEVICE_TYPE_CPU;
				if (!strcasecmp(argv[i], "gpu"))
					device_type = CL_DEVICE_TYPE_GPU;
				if (!strcasecmp(argv[i], "all"))
					device_type = CL_DEVICE_TYPE_ALL;
				if (!strcasecmp(argv[i], "accelerator"))
					device_type = CL_DEVICE_TYPE_ACCELERATOR;
			}
		}
	}

	struct sigaction s;
	s.sa_handler = signal_handler;
	sigaction(SIGSEGV, &s, NULL);
	try
	{
		vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		for(size_t i = 0 ; i < platforms.size() ; ++i)
		{
			const cl::Platform &p = platforms[i];

			vector<cl::Device> devices;
			try
			{
				p.getDevices(device_type, &devices);
			}
			catch(...)
			{
				continue;
			}

			if (devices.empty())
				continue;

			cout << "platform : " << p.getInfo<CL_PLATFORM_NAME>() << endl;

			cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(p)(), 0};
			contexts.push_back(cl::Context(devices, properties));
			queues.push_back(cl::CommandQueue(contexts.back(), devices.front()));
			buffers.push_back(cl::Buffer(contexts.back(), CL_MEM_READ_WRITE, 64));
		}

		if (contexts.empty())
		{
			cerr << "error : no device found" << endl;
			return 0;
		}
		cerr.precision(16);

		const char *functions[] = {"acos", "acosh", "acospi",
								   "asin", "asinh", "asinpi",
								   "atan", "atanh", "atanpi",
								   "cbrt", "ceil",
								   "cos", "cosh", "cospi",
								   "erf", "erfc",
								   "exp", "exp2", "exp10", "expm1",
								   "fabs", "floor",
								   "lgamma",
								   "log", "log2", "log10", "log1p", "logb",
								   "rint", "round", "rsqrt",
								   "sin", "sinh", "sinpi",
								   "sqrt",
								   "tan", "tanh", "tanpi", "tgamma", "trunc",
								   "half_cos",
								   "half_exp", "half_exp2", "half_exp10",
								   "half_log", "half_log2", "half_log10",
								   "half_recip", "half_rsqrt",
								   "half_sin", "half_sqrt", "half_tan",
								   "native_cos",
								   "native_exp", "native_exp2", "native_exp10",
								   "native_log", "native_log2", "native_log10",
								   "native_recip", "native_rsqrt", "native_sin",
								   "native_sqrt", "native_tan",
								   "degrees", "sign", "radians"};

		for(size_t i = 0 ; i < sizeof(functions) / sizeof(const char*) ; ++i)
			test_function(functions[i]);
	}
	catch(cl::Error err)
	{
		cerr << "error: " << err.what() << '(' << get_error_as_string(err.err()) << ')' << endl;
		return -1;
	}

	return 0;
}
