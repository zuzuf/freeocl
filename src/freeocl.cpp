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
#include "freeocl.h"
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <iostream>
#include "context.h"
#include "utils/commandqueue.h"
#include "mem.h"
#include "event.h"
#include "kernel.h"
#include "program.h"
#include "sampler.h"
#include "prototypes.h"
#include <stdexcept>
#include <algorithm>
#include <sys/time.h>

#ifdef FREEOCL_RUN_GDB_ON_CRASH
#include <unistd.h>
#include <signal.h>
#include <sstream>

namespace
{
	void signal_handler(int)
	{
		pid_t mypid = getpid();
		std::stringstream cmd;
		cmd << "gdb --pid " << mypid << " -ex \"bt\"";
		const int _ = system(cmd.str().c_str());
		exit(-1);
	}
}
#endif

namespace FreeOCL
{
	_cl_icd_dispatch init_dispatch();

	FreeOCL::set<cl_context> valid_contexts;
	FreeOCL::set<cl_command_queue> valid_command_queues;
	FreeOCL::set<cl_mem> valid_mems;
	FreeOCL::set<cl_event> valid_events;
	FreeOCL::set<cl_kernel> valid_kernels;
	FreeOCL::set<cl_program> valid_programs;
	FreeOCL::set<cl_sampler> valid_samplers;
	mutex global_mutex;
	_cl_icd_dispatch dispatch = init_dispatch();

	bool is_valid(cl_context c)
	{
		global_mutex.lock();
		const bool r = valid_contexts.count(c) != 0 && c->valid();
		if (r)
			c->lock();
		global_mutex.unlock();
		return r;
	}

	bool is_valid(cl_command_queue q)
	{
		global_mutex.lock();
		const bool r = valid_command_queues.count(q) != 0 && q->valid();
		if (r)
			q->lock();
		global_mutex.unlock();
		return r;
	}

	bool is_valid(cl_mem m)
	{
		global_mutex.lock();
		const bool r = valid_mems.count(m) != 0 && m->valid();
		if (r)
			m->lock();
		global_mutex.unlock();
		return r;
	}

	bool is_valid(cl_event e)
	{
		global_mutex.lock();
		const bool r = valid_events.count(e) != 0 && e->valid();
		if (r)
			e->lock();
		global_mutex.unlock();
		return r;
	}

	bool is_valid(cl_kernel k)
	{
		global_mutex.lock();
		const bool r = valid_kernels.count(k) != 0 && k->valid();
		if (r)
			k->lock();
		global_mutex.unlock();
		return r;
	}

	bool is_valid(cl_program p)
	{
		global_mutex.lock();
		const bool r = valid_programs.count(p) != 0 && p->valid();
		if (r)
			p->lock();
		global_mutex.unlock();
		return r;
	}

	bool is_valid(cl_sampler s)
	{
		global_mutex.lock();
		const bool r = valid_samplers.count(s) != 0 && s->valid();
		if (r)
			s->lock();
		global_mutex.unlock();
		return r;
	}

	bool is_valid(cl_device_id d)
	{
		return d == FreeOCL::device;
	}

	bool is_valid(cl_platform_id p)
	{
		return p == FreeOCL::platform;
	}

	bool copy_memory_within_limits(const void *src, const size_t size, const size_t maxSize, void *dst, size_t *s)
	{
		if (s != NULL)
			*s = size;
		if (dst != NULL)
			memcpy(dst, src, std::min(size, maxSize));
		return size > maxSize;
	}

	std::string run_command(const std::string &cmd, int *ret)
	{
		std::string result;
		FILE *file = popen(cmd.c_str(), "r");
		if (!file)
		{
			if (ret)
				*ret = 0x7FFFFFFF;
			return result;
		}

		int c;
		while((c = fgetc(file)) != -1)
			result += (char)c;
		const int r = pclose(file);
		if (ret)
			*ret = r;

		return result;
	}

	long int parse_int(const std::string &s)
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

	std::deque<std::string> split(const std::string &s, const std::string &sep)
	{
		std::deque<std::string> result;

		size_t pos = s.find_first_not_of(sep, 0);
		if (pos != std::string::npos)
			do
			{
				const size_t next = s.find_first_of(sep, pos);
				result.push_back(s.substr(pos, next - pos));
				if (next != std::string::npos)
					pos = s.find_first_not_of(sep, next);
				else
					pos = std::string::npos;
			} while(pos != std::string::npos);

		return result;
	}

	bool contains_word(const std::string &s, const std::string &w)
	{
		std::deque<std::string> words = split(s, " \t\n\r");
		return std::find(words.begin(), words.end(), w) != words.end();
	}

	u_int64_t usec_timer()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec * 1000000UL + tv.tv_usec;
	}

	cl_int empty_slot(void*)
	{
		std::cerr << "FreeOCL: unimplemented function" << std::endl;
		throw std::runtime_error(std::string("FreeOCL: unimplemented function"));
		return CL_INVALID_OPERATION;
	}

	_cl_icd_dispatch init_dispatch()
	{
#ifdef FREEOCL_RUN_GDB_ON_CRASH
		struct sigaction s;
		memset(&s, 0, sizeof(struct sigaction));
		s.sa_handler = signal_handler;
		sigaction(SIGSEGV, &s, NULL);
#endif

		_cl_icd_dispatch table;
		table.clGetPlatformIDs = clIcdGetPlatformIDsKHR;
		table.clGetPlatformInfo = clGetPlatformInfoFCL;
		table.clGetDeviceIDs = clGetDeviceIDsFCL;
		table.clGetDeviceInfo = clGetDeviceInfoFCL;
		table.clCreateContext = clCreateContextFCL;
		table.clCreateContextFromType = clCreateContextFromTypeFCL;
		table.clRetainContext = clRetainContextFCL;
		table.clReleaseContext = clReleaseContextFCL;
		table.clGetContextInfo = clGetContextInfoFCL;

		table.clCreateCommandQueue = clCreateCommandQueueFCL;
		table.clRetainCommandQueue = clRetainCommandQueueFCL;
		table.clReleaseCommandQueue = clReleaseCommandQueueFCL;
		table.clGetCommandQueueInfo = clGetCommandQueueInfoFCL;
		table.clSetCommandQueueProperty = clSetCommandQueuePropertyFCL;

		table.clCreateBuffer = clCreateBufferFCL;
		table.clCreateSubBuffer = clCreateSubBufferFCL;
		table.clCreateImage2D = clCreateImage2DFCL;
		table.clCreateImage3D = clCreateImage3DFCL;
		table.clRetainMemObject = clRetainMemObjectFCL;
		table.clReleaseMemObject = clReleaseMemObjectFCL;
		table.clGetSupportedImageFormats = clGetSupportedImageFormatsFCL;
		table.clGetMemObjectInfo = clGetMemObjectInfoFCL;
		table.clGetImageInfo = clGetImageInfoFCL;
		table.clSetMemObjectDestructorCallback = clSetMemObjectDestructorCallbackFCL;

		table.clCreateSampler = clCreateSamplerFCL;
		table.clRetainSampler = clRetainSamplerFCL;
		table.clReleaseSampler = clReleaseSamplerFCL;
		table.clGetSamplerInfo = clGetSamplerInfoFCL;

		table.clCreateProgramWithSource = clCreateProgramWithSourceFCL;
		table.clCreateProgramWithBinary = clCreateProgramWithBinaryFCL;
		table.clRetainProgram = clRetainProgramFCL;
		table.clReleaseProgram = clReleaseProgramFCL;
		table.clBuildProgram = clBuildProgramFCL;
		table.clUnloadCompiler = NULL;
		table.clGetProgramInfo = clGetProgramInfoFCL;
		table.clGetProgramBuildInfo = clGetProgramBuildInfoFCL;

		table.clCreateKernel = clCreateKernelFCL;
		table.clCreateKernelsInProgram = clCreateKernelsInProgramFCL;
		table.clRetainKernel = clRetainKernelFCL;
		table.clReleaseKernel = clReleaseKernelFCL;
		table.clSetKernelArg = clSetKernelArgFCL;
		table.clGetKernelInfo = clGetKernelInfoFCL;
		table.clGetKernelWorkGroupInfo = clGetKernelWorkGroupInfoFCL;
		table.clWaitForEvents = clWaitForEventsFCL;
		table.clGetEventInfo = clGetEventInfoFCL;
		table.clCreateUserEvent = clCreateUserEventFCL;
		table.clRetainEvent = clRetainEventFCL;
		table.clReleaseEvent = clReleaseEventFCL;
		table.clSetUserEventStatus = clSetUserEventStatusFCL;
		table.clSetEventCallback = clSetEventCallbackFCL;

		table.clGetEventProfilingInfo = clGetEventProfilingInfoFCL;

		table.clFlush = clFlushFCL;
		table.clFinish = clFinishFCL;

		table.clEnqueueReadBuffer = clEnqueueReadBufferFCL;
		table.clEnqueueReadBufferRect = clEnqueueReadBufferRectFCL;
		table.clEnqueueWriteBuffer = clEnqueueWriteBufferFCL;
		table.clEnqueueWriteBufferRect = clEnqueueWriteBufferRectFCL;
		table.clEnqueueCopyBuffer = clEnqueueCopyBufferFCL;
		table.clEnqueueCopyBufferRect = clEnqueueCopyBufferRectFCL;
		table.clEnqueueReadImage = clEnqueueReadImageFCL;
		table.clEnqueueWriteImage = clEnqueueWriteImageFCL;
		table.clEnqueueCopyImage = clEnqueueCopyImageFCL;
		table.clEnqueueCopyImageToBuffer = clEnqueueCopyImageToBufferFCL;
		table.clEnqueueCopyBufferToImage = clEnqueueCopyBufferToImageFCL;
		table.clEnqueueMapBuffer = clEnqueueMapBufferFCL;
		table.clEnqueueMapImage = clEnqueueMapImageFCL;
		table.clEnqueueUnmapMemObject = clEnqueueUnmapMemObjectFCL;

		table.clEnqueueNDRangeKernel = clEnqueueNDRangeKernelFCL;
		table.clEnqueueTask = clEnqueueTaskFCL;
		table.clEnqueueNativeKernel = clEnqueueNativeKernelFCL;
		table.clEnqueueMarker = clEnqueueMarkerFCL;
		table.clEnqueueWaitForEvents = clEnqueueWaitForEventsFCL;
		table.clEnqueueBarrier = clEnqueueBarrierFCL;

//		table.clGetExtensionFunctionAddress = clGetExtensionFunctionAddress;

		table.clCreateSubDevices = clCreateSubDevicesFCL;
		table.clRetainDevice = clRetainDeviceFCL;
		table.clReleaseDevice = clReleaseDeviceFCL;
		table.clCreateImage = clCreateImageFCL;
		table.clCreateProgramWithBuiltInKernels = clCreateProgramWithBuiltInKernelsFCL;
		table.clCompileProgram = clCompileProgramFCL;
		table.clLinkProgram = clLinkProgramFCL;
		table.clUnloadPlatformCompiler = clUnloadPlatformCompilerFCL;
		table.clGetKernelArgInfo = clGetKernelArgInfoFCL;
		table.clEnqueueFillBuffer = clEnqueueFillBufferFCL;
		table.clEnqueueFillImage = clEnqueueFillImageFCL;
		table.clEnqueueMigrateMemObjects = clEnqueueMigrateMemObjectsFCL;
		table.clEnqueueMarkerWithWaitList = clEnqueueMarkerWithWaitListFCL;
		table.clEnqueueBarrierWithWaitList = clEnqueueBarrierWithWaitListFCL;
//		table.clSetPrintfCallback = clSetPrintfCallbackFCL;
		table.clGetExtensionFunctionAddressForPlatform = clGetExtensionFunctionAddressForPlatformFCL;

		for(size_t i = 0 ; i < sizeof(table.empty_after_1_0_API) / sizeof(size_t) ; ++i)
			table.empty_after_1_0_API[i] = (_stub)empty_slot;

		for(size_t i = 0 ; i < sizeof(table.empty_after_1_1_API) / sizeof(size_t) ; ++i)
			table.empty_after_1_1_API[i] = (_stub)empty_slot;

		for(size_t i = 0 ; i < sizeof(table.empty_after_1_2_API) / sizeof(size_t) ; ++i)
			table.empty_after_1_2_API[i] = (_stub)empty_slot;
		return table;
	}

	context_resource::context_resource(cl_context context)
		: context(context)
	{
		if (context)
		{
			context->lock();
			context->resources.insert(this);
			context->unlock();
		}
	}

	context_resource::~context_resource()
	{
		if (context)
		{
			context->lock();
			context->resources.erase(this);
			context->unlock();
		}
	}
}

extern "C"
{
	void* clGetExtensionFunctionAddress (const char *funcname)
	{
		return clGetExtensionFunctionAddressFCL(funcname);
	}

	void* clGetExtensionFunctionAddressFCL (const char *funcname)
	{
	#define ADD(name)	if (strcmp(funcname, #name) == 0)	return (void*)name

		ADD(clIcdGetPlatformIDsKHR);

		return NULL;
	}
}
