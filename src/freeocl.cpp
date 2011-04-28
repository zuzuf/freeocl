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
#include "sampler.h"
#include "prototypes.h"
#include <stdexcept>

namespace FreeOCL
{
	_cl_icd_dispatch init_dispatch();

	std::set<cl_context> valid_contexts;
	std::set<cl_command_queue> valid_command_queues;
	std::set<cl_mem> valid_mems;
	std::set<cl_event> valid_events;
	std::set<cl_kernel> valid_kernels;
	std::set<cl_program> valid_programs;
	std::set<cl_sampler> valid_samplers;
	mutex global_mutex;
	_cl_icd_dispatch dispatch = init_dispatch();

	bool isValid(cl_context c)
	{
		global_mutex.lock();
		const bool r = valid_contexts.count(c) != 0 && c->valid();
		if (r)
			c->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_command_queue q)
	{
		global_mutex.lock();
		const bool r = valid_command_queues.count(q) != 0 && q->valid();
		if (r)
			q->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_mem m)
	{
		global_mutex.lock();
		const bool r = valid_mems.count(m) != 0 && m->valid();
		if (r)
			m->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_event e)
	{
		global_mutex.lock();
		const bool r = valid_events.count(e) != 0 && e->valid();
		if (r)
			e->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_kernel k)
	{
		global_mutex.lock();
		const bool r = valid_kernels.count(k) != 0 && k->valid();
		if (r)
			k->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_program p)
	{
		global_mutex.lock();
		const bool r = valid_programs.count(p) != 0 && p->valid();
		if (r)
			p->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_sampler s)
	{
		global_mutex.lock();
		const bool r = valid_samplers.count(s) != 0 && s->valid();
		if (r)
			s->lock();
		global_mutex.unlock();
		return r;
	}

	bool isValid(cl_device_id d)
	{
		return d == FreeOCL::device;
	}

	bool isValid(cl_platform_id p)
	{
		return p == FreeOCL::platform;
	}

	bool copyMemoryWithinLimits(const void *src, const size_t size, const size_t maxSize, void *dst, size_t *s)
	{
		if (s != NULL)
			*s = size;
		if (dst != NULL)
			memcpy(dst, src, std::min(size, maxSize));
		return size > maxSize;
	}

	std::string runCommand(const std::string &cmd, int *ret)
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
		while((c = fgetc(file)) != -1 && !feof(file))
			result += (char)c;
		const int r = pclose(file);
		if (ret)
			*ret = r;

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

	cl_int empty_slot(void*)
	{
		std::cerr << "FreeOCL: unimplemented function" << std::endl;
		throw std::runtime_error(std::string("FreeOCL: unimplemented function"));
		return CL_INVALID_OPERATION;
	}

	_cl_icd_dispatch init_dispatch()
	{
		_cl_icd_dispatch table;
		table.clGetPlatformIDs = clIcdGetPlatformIDsKHR;
		table.clGetPlatformInfo = clGetPlatformInfoFCL;
		table.clGetDeviceIDs = clGetDeviceIDsFCL;
		table.clGetDeviceInfo = clGetDeviceInfoFCL;
		table.clCreateContext = clCreateContextFCL;
//		table.clCreateContextFromType = NULL;
		table.clRetainContext = clRetainContextFCL;
		table.clReleaseContext = clReleaseContextFCL;
		table.clGetContextInfo = clGetContextInfoFCL;

		table.clCreateCommandQueue = clCreateCommandQueueFCL;
		table.clRetainCommandQueue = clRetainCommandQueueFCL;
		table.clReleaseCommandQueue = clReleaseCommandQueueFCL;
		table.clGetCommandQueueInfo = clGetCommandQueueInfoFCL;
		table.clSetCommandQueueProperty = NULL;

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

		std::cout << clCreateProgramWithSourceFCL << std::endl;
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

		for(size_t i = 0 ; i < sizeof(table.empty_after_1_0_API) / sizeof(size_t) ; ++i)
			table.empty_after_1_0_API[i] = (_stub)empty_slot;

		for(size_t i = 0 ; i < sizeof(table.empty_after_1_1_API) / sizeof(size_t) ; ++i)
			table.empty_after_1_1_API[i] = (_stub)empty_slot;
		return table;
	}
}

extern "C"
{
	void* clGetExtensionFunctionAddress (const char *funcname)
	{
	#define ADD(name)	if (strcmp(funcname, #name) == 0)	return (void*)name

		ADD(clIcdGetPlatformIDsKHR);

		return NULL;
	}
}
