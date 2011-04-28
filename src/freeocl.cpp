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
#include "prototypes.h"

namespace FreeOCL
{
	_cl_icd_dispatch init_dispatch();

	std::set<cl_context> valid_contexts;
	std::set<cl_command_queue> valid_command_queues;
	std::set<cl_mem> valid_mems;
	std::set<cl_event> valid_events;
	std::set<cl_kernel> valid_kernels;
	std::set<cl_program> valid_programs;
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

#define	STUB(X)	cl_int stub##X(void*)	{	std::cout << "stub" #X << std::endl;	return CL_INVALID_OPERATION;	}
	STUB(0)
	STUB(1)
	STUB(2)
	STUB(3)
	STUB(4)
	STUB(5)
	STUB(6)
	STUB(7)
	STUB(8)
	STUB(9)
	STUB(10)
	STUB(11)
	STUB(12)
	STUB(13)
	STUB(14)
	STUB(15)
	STUB(16)
	STUB(17)
	STUB(18)
	STUB(19)
	STUB(20)
	STUB(21)
	STUB(22)
	STUB(23)
	STUB(24)
	STUB(25)
	STUB(26)
	STUB(27)
	STUB(28)
	STUB(29)
	STUB(30)
	STUB(31)
	STUB(32)
	STUB(33)
	STUB(34)
	STUB(35)
	STUB(36)
	STUB(37)
	STUB(38)
	STUB(39)
	STUB(40)
	STUB(41)
	STUB(42)
	STUB(43)
	STUB(44)
	STUB(45)
	STUB(46)
	STUB(47)
	STUB(48)
	STUB(49)
	STUB(50)
	STUB(51)
	STUB(52)
	STUB(53)
	STUB(54)
	STUB(55)
	STUB(56)
	STUB(57)
	STUB(58)
	STUB(59)
	STUB(60)
	STUB(61)
	STUB(62)
	STUB(63)
	STUB(64)
	STUB(65)
	STUB(66)
	STUB(67)
	STUB(68)
	STUB(69)
	STUB(70)
	STUB(71)
	STUB(72)
	STUB(73)
	STUB(74)
	STUB(75)
	STUB(76)
	STUB(77)
	STUB(78)
	STUB(79)
	STUB(80)
	STUB(81)
	STUB(82)
	STUB(83)
	STUB(84)
	STUB(85)
	STUB(86)
	STUB(87)
	STUB(88)
	STUB(89)
	STUB(90)
	STUB(91)
	STUB(92)
	STUB(93)
	STUB(94)
	STUB(95)
	STUB(96)
	STUB(97)
	STUB(98)
	STUB(99)
	STUB(100)

	_cl_icd_dispatch init_dispatch()
	{
		_cl_icd_dispatch table;
		table.clGetPlatformIDs = clIcdGetPlatformIDsKHR;
		table.clGetPlatformInfo = clGetPlatformInfoFCL;
		table.clGetDeviceIDs = clGetDeviceIDsFCL;
		table.clGetDeviceInfo = clGetDeviceInfoFCL;
		table.clCreateContext = clCreateContextFCL;
		table.clCreateContextFromType = NULL;
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

		table.clGetExtensionFunctionAddress = clGetExtensionFunctionAddress;

		void **t = ((void**)&table);
//		t[0] = (void*)stub0;
//		t[1] = (void*)stub1;
//		t[2] = (void*)stub2;
//		t[3] = (void*)stub3;
//		t[4] = (void*)stub4;
		t[5] = (void*)stub5;
		t[6] = (void*)stub6;
//		t[7] = (void*)stub7;
		t[8] = (void*)stub8;
//		t[9] = (void*)stub9;
		t[10] = (void*)stub10;
//		t[11] = (void*)stub11;
		t[12] = (void*)stub12;
		t[13] = (void*)stub13;
//		t[14] = (void*)stub14;
		t[15] = (void*)stub15;
		t[16] = (void*)stub16;
		t[17] = (void*)stub17;
//		t[18] = (void*)stub18;
		t[19] = (void*)stub19;
		t[20] = (void*)stub20;
		t[21] = (void*)stub21;
		t[22] = (void*)stub22;
		t[23] = (void*)stub23;
		t[24] = (void*)stub24;
		t[25] = (void*)stub25;
		t[26] = (void*)stub26;
		t[27] = (void*)stub27;
		t[28] = (void*)stub28;
		t[29] = (void*)stub29;
		t[30] = (void*)stub30;
		t[31] = (void*)stub31;
		t[32] = (void*)stub32;
		t[33] = (void*)stub33;
		t[34] = (void*)stub34;
		t[35] = (void*)stub35;
		t[36] = (void*)stub36;
		t[37] = (void*)stub37;
		t[38] = (void*)stub38;
		t[39] = (void*)stub39;
		t[40] = (void*)stub40;
		t[41] = (void*)stub41;
		t[42] = (void*)stub42;
		t[43] = (void*)stub43;
		t[44] = (void*)stub44;
		t[45] = (void*)stub45;
		t[46] = (void*)stub46;
		t[47] = (void*)stub47;
		t[48] = (void*)stub48;
//		t[49] = (void*)stub49;
		t[50] = (void*)stub50;
		t[51] = (void*)stub51;
		t[52] = (void*)stub52;
		t[53] = (void*)stub53;
		t[54] = (void*)stub54;
		t[55] = (void*)stub55;
//		t[56] = (void*)stub56;
		t[57] = (void*)stub57;
		t[58] = (void*)stub58;
		t[59] = (void*)stub59;
		t[60] = (void*)stub60;
		t[61] = (void*)stub61;
		t[62] = (void*)stub62;
		t[63] = (void*)stub63;
		t[64] = (void*)stub64;
		t[65] = (void*)stub65;
		t[66] = (void*)stub66;
		t[67] = (void*)stub67;
		t[68] = (void*)stub68;
		t[69] = (void*)stub69;
		t[70] = (void*)stub70;
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
