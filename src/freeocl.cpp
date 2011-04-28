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
	STUB(101)
	STUB(102)
	STUB(103)
	STUB(104)
	STUB(105)
	STUB(106)
	STUB(107)
	STUB(108)
	STUB(109)
	STUB(110)
	STUB(111)
	STUB(112)
	STUB(113)
	STUB(114)
	STUB(115)
	STUB(116)
	STUB(117)
	STUB(118)
	STUB(119)
	STUB(120)
	STUB(121)
	STUB(122)
	STUB(123)
	STUB(124)
	STUB(125)
	STUB(126)
	STUB(127)
	STUB(128)
	STUB(129)
	STUB(130)
	STUB(131)
	STUB(132)
	STUB(133)
	STUB(134)
	STUB(135)
	STUB(136)
	STUB(137)
	STUB(138)
	STUB(139)
	STUB(140)
	STUB(141)
	STUB(142)
	STUB(143)
	STUB(144)
	STUB(145)
	STUB(146)
	STUB(147)
	STUB(148)
	STUB(149)
	STUB(150)
	STUB(151)
	STUB(152)
	STUB(153)
	STUB(154)
	STUB(155)
	STUB(156)
	STUB(157)
	STUB(158)
	STUB(159)
	STUB(160)
	STUB(161)
	STUB(162)
	STUB(163)
	STUB(164)
	STUB(165)
	STUB(166)
	STUB(167)
	STUB(168)
	STUB(169)
	STUB(170)
	STUB(171)
	STUB(172)
	STUB(173)
	STUB(174)
	STUB(175)
	STUB(176)
	STUB(177)
	STUB(178)
	STUB(179)
	STUB(180)
	STUB(181)
	STUB(182)
	STUB(183)
	STUB(184)
	STUB(185)
	STUB(186)
	STUB(187)
	STUB(188)
	STUB(189)
	STUB(190)
	STUB(191)
	STUB(192)
	STUB(193)
	STUB(194)
	STUB(195)
	STUB(196)
	STUB(197)
	STUB(198)
	STUB(199)
	STUB(200)
	STUB(201)
	STUB(202)
	STUB(203)
	STUB(204)
	STUB(205)
	STUB(206)
	STUB(207)
	STUB(208)
	STUB(209)
	STUB(210)
	STUB(211)
	STUB(212)
	STUB(213)
	STUB(214)
	STUB(215)
	STUB(216)
	STUB(217)
	STUB(218)
	STUB(219)
	STUB(220)
	STUB(221)
	STUB(222)
	STUB(223)
	STUB(224)
	STUB(225)
	STUB(226)
	STUB(227)
	STUB(228)
	STUB(229)
	STUB(230)
	STUB(231)
	STUB(232)
	STUB(233)
	STUB(234)
	STUB(235)
	STUB(236)
	STUB(237)
	STUB(238)
	STUB(239)
	STUB(240)
	STUB(241)
	STUB(242)
	STUB(243)
	STUB(244)
	STUB(245)
	STUB(246)
	STUB(247)
	STUB(248)
	STUB(249)
	STUB(250)
	STUB(251)
	STUB(252)
	STUB(253)
	STUB(254)
	STUB(255)
	STUB(256)
	STUB(257)
	STUB(258)
	STUB(259)
	STUB(260)
	STUB(261)
	STUB(262)
	STUB(263)
	STUB(264)
	STUB(265)
	STUB(266)
	STUB(267)
	STUB(268)
	STUB(269)
	STUB(270)
	STUB(271)

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
		t[0] = (void*)clIcdGetPlatformIDsKHR;
		t[1] = (void*)clGetPlatformInfoFCL;
		t[2] = (void*)clGetDeviceIDsFCL;
		t[3] = (void*)clGetDeviceInfoFCL;
		t[4] = (void*)clCreateContextFCL;
		t[5] = (void*)stub5;
		t[6] = (void*)clRetainContextFCL;
		t[7] = (void*)clReleaseContextFCL;
		t[8] = (void*)clGetContextInfoFCL;
		t[9] = (void*)clCreateCommandQueueFCL;
		t[10] = (void*)clRetainCommandQueueFCL;
		t[11] = (void*)clReleaseCommandQueueFCL;
		t[12] = (void*)clGetCommandQueueInfoFCL;
		t[13] = (void*)stub13;
		t[14] = (void*)clCreateBufferFCL;
		t[15] = (void*)clCreateImage2DFCL;
		t[16] = (void*)clCreateImage3DFCL;
		t[17] = (void*)clRetainMemObjectFCL;
		t[18] = (void*)clReleaseMemObjectFCL;
		t[19] = (void*)clGetSupportedImageFormatsFCL;
		t[20] = (void*)clGetMemObjectInfoFCL;
		t[21] = (void*)clGetImageInfoFCL;
		t[22] = (void*)stub22;
		t[23] = (void*)stub23;
		t[24] = (void*)stub24;
		t[25] = (void*)stub25;
		t[26] = (void*)clCreateProgramWithSourceFCL;
		t[27] = (void*)stub27;
		t[28] = (void*)clRetainProgramFCL;
		t[29] = (void*)clReleaseProgramFCL;
		t[30] = (void*)clBuildProgramFCL;
		t[31] = (void*)stub31;
		t[32] = (void*)clGetProgramInfoFCL;
		t[33] = (void*)clGetProgramBuildInfoFCL;
		t[34] = (void*)clCreateKernelFCL;
		t[35] = (void*)clCreateKernelsInProgramFCL;
		t[36] = (void*)clRetainKernelFCL;
		t[37] = (void*)clReleaseKernelFCL;
		t[38] = (void*)clSetKernelArgFCL;
		t[39] = (void*)clGetKernelInfoFCL;
		t[40] = (void*)clGetKernelWorkGroupInfoFCL;
		t[41] = (void*)clWaitForEventsFCL;
		t[42] = (void*)clGetEventInfoFCL;
		t[43] = (void*)clRetainEventFCL;
		t[44] = (void*)clReleaseEventFCL;
		t[45] = (void*)clGetEventProfilingInfoFCL;
		t[46] = (void*)clFlushFCL;
		t[47] = (void*)clFinishFCL;
		t[48] = (void*)clEnqueueReadBufferFCL;
		t[49] = (void*)clEnqueueWriteBufferFCL;
		t[50] = (void*)stub50;
		t[51] = (void*)stub51;
		t[52] = (void*)stub52;
		t[53] = (void*)stub53;
		t[54] = (void*)stub54;
		t[55] = (void*)stub55;
		t[56] = (void*)clEnqueueMapBufferFCL;
		t[57] = (void*)stub57;
		t[58] = (void*)clEnqueueUnmapMemObjectFCL;
		t[59] = (void*)stub59;
		t[60] = (void*)stub60;
		t[61] = (void*)stub61;
		t[62] = (void*)clEnqueueMarkerFCL;
		t[63] = (void*)clEnqueueWaitForEventsFCL;
		t[64] = (void*)clEnqueueBarrierFCL;
		t[65] = (void*)stub65;
		t[66] = (void*)stub66;
		t[67] = (void*)stub67;
		t[68] = (void*)stub68;
		t[69] = (void*)stub69;
		t[70] = (void*)stub70;
		t[71] = (void*)stub71;
		t[72] = (void*)stub72;
		t[73] = (void*)stub73;
		t[74] = (void*)stub74;
		t[75] = (void*)stub75;
		t[76] = (void*)stub76;
		t[77] = (void*)stub77;
		t[78] = (void*)stub78;
		t[79] = (void*)stub79;
		t[80] = (void*)stub80;
		t[81] = (void*)clSetEventCallbackFCL;
		t[82] = (void*)stub82;
		t[83] = (void*)clSetMemObjectDestructorCallbackFCL;
		t[84] = (void*)clCreateUserEventFCL;
		t[85] = (void*)clSetUserEventStatusFCL;
		t[86] = (void*)stub86;
		t[87] = (void*)stub87;
		t[88] = (void*)stub88;
		t[89] = (void*)stub89;
		t[90] = (void*)stub90;
		t[91] = (void*)stub91;
		t[92] = (void*)stub92;
		t[93] = (void*)stub93;
		t[94] = (void*)stub94;
		t[95] = (void*)stub95;
		t[96] = (void*)stub96;
		t[97] = (void*)stub97;
		t[98] = (void*)stub98;
		t[99] = (void*)stub99;
		t[100] = (void*)stub100;
		t[101] = (void*)stub101;
		t[102] = (void*)stub102;
		t[103] = (void*)stub103;
		t[104] = (void*)stub104;
		t[105] = (void*)stub105;
		t[106] = (void*)stub106;
		t[107] = (void*)stub107;
		t[108] = (void*)stub108;
		t[109] = (void*)stub109;
		t[110] = (void*)stub110;
		t[111] = (void*)stub111;
		t[112] = (void*)stub112;
		t[113] = (void*)stub113;
		t[114] = (void*)stub114;
		t[115] = (void*)stub115;
		t[116] = (void*)stub116;
		t[117] = (void*)stub117;
		t[118] = (void*)stub118;
		t[119] = (void*)stub119;
		t[120] = (void*)stub120;
		t[121] = (void*)stub121;
		t[122] = (void*)stub122;
		t[123] = (void*)stub123;
		t[124] = (void*)stub124;
		t[125] = (void*)stub125;
		t[126] = (void*)stub126;
		t[127] = (void*)stub127;
		t[128] = (void*)stub128;
		t[129] = (void*)stub129;
		t[130] = (void*)stub130;
		t[131] = (void*)stub131;
		t[132] = (void*)stub132;
		t[133] = (void*)stub133;
		t[134] = (void*)stub134;
		t[135] = (void*)stub135;
		t[136] = (void*)stub136;
		t[137] = (void*)stub137;
		t[138] = (void*)stub138;
		t[139] = (void*)stub139;
		t[140] = (void*)stub140;
		t[141] = (void*)stub141;
		t[142] = (void*)stub142;
		t[143] = (void*)stub143;
		t[144] = (void*)stub144;
		t[145] = (void*)stub145;
		t[146] = (void*)stub146;
		t[147] = (void*)stub147;
		t[148] = (void*)stub148;
		t[149] = (void*)stub149;
		t[150] = (void*)stub150;
		t[151] = (void*)stub151;
		t[152] = (void*)stub152;
		t[153] = (void*)stub153;
		t[154] = (void*)stub154;
		t[155] = (void*)stub155;
		t[156] = (void*)stub156;
		t[157] = (void*)stub157;
		t[158] = (void*)stub158;
		t[159] = (void*)stub159;
		t[160] = (void*)stub160;
		t[161] = (void*)stub161;
		t[162] = (void*)stub162;
		t[163] = (void*)stub163;
		t[164] = (void*)stub164;
		t[165] = (void*)stub165;
		t[166] = (void*)stub166;
		t[167] = (void*)stub167;
		t[168] = (void*)stub168;
		t[169] = (void*)stub169;
		t[170] = (void*)stub170;
		t[171] = (void*)stub171;
		t[172] = (void*)stub172;
		t[173] = (void*)stub173;
		t[174] = (void*)stub174;
		t[175] = (void*)stub175;
		t[176] = (void*)stub176;
		t[177] = (void*)stub177;
		t[178] = (void*)stub178;
		t[179] = (void*)stub179;
		t[180] = (void*)stub180;
		t[181] = (void*)stub181;
		t[182] = (void*)stub182;
		t[183] = (void*)stub183;
		t[184] = (void*)stub184;
		t[185] = (void*)stub185;
		t[186] = (void*)stub186;
		t[187] = (void*)stub187;
		t[188] = (void*)stub188;
		t[189] = (void*)stub189;
		t[190] = (void*)stub190;
		t[191] = (void*)stub191;
		t[192] = (void*)stub192;
		t[193] = (void*)stub193;
		t[194] = (void*)stub194;
		t[195] = (void*)stub195;
		t[196] = (void*)stub196;
		t[197] = (void*)stub197;
		t[198] = (void*)stub198;
		t[199] = (void*)stub199;
		t[200] = (void*)stub200;
		t[201] = (void*)stub201;
		t[202] = (void*)stub202;
		t[203] = (void*)stub203;
		t[204] = (void*)stub204;
		t[205] = (void*)stub205;
		t[206] = (void*)stub206;
		t[207] = (void*)stub207;
		t[208] = (void*)stub208;
		t[209] = (void*)stub209;
		t[210] = (void*)stub210;
		t[211] = (void*)stub211;
		t[212] = (void*)stub212;
		t[213] = (void*)stub213;
		t[214] = (void*)stub214;
		t[215] = (void*)stub215;
		t[216] = (void*)stub216;
		t[217] = (void*)stub217;
		t[218] = (void*)stub218;
		t[219] = (void*)stub219;
		t[220] = (void*)stub220;
		t[221] = (void*)stub221;
		t[222] = (void*)stub222;
		t[223] = (void*)stub223;
		t[224] = (void*)stub224;
		t[225] = (void*)stub225;
		t[226] = (void*)stub226;
		t[227] = (void*)stub227;
		t[228] = (void*)stub228;
		t[229] = (void*)stub229;
		t[230] = (void*)stub230;
		t[231] = (void*)stub231;
		t[232] = (void*)stub232;
		t[233] = (void*)stub233;
		t[234] = (void*)stub234;
		t[235] = (void*)stub235;
		t[236] = (void*)stub236;
		t[237] = (void*)stub237;
		t[238] = (void*)stub238;
		t[239] = (void*)stub239;
		t[240] = (void*)stub240;
		t[241] = (void*)stub241;
		t[242] = (void*)stub242;
		t[243] = (void*)stub243;
		t[244] = (void*)stub244;
		t[245] = (void*)stub245;
		t[246] = (void*)stub246;
		t[247] = (void*)stub247;
		t[248] = (void*)stub248;
		t[249] = (void*)stub249;
		t[250] = (void*)stub250;
		t[251] = (void*)stub251;
		t[252] = (void*)stub252;
		t[253] = (void*)stub253;
		t[254] = (void*)stub254;
		t[255] = (void*)stub255;
		t[256] = (void*)stub256;
		t[257] = (void*)stub257;
		t[258] = (void*)stub258;
		t[259] = (void*)stub259;
		t[260] = (void*)stub260;
		t[261] = (void*)stub261;
		t[262] = (void*)stub262;
		t[263] = (void*)stub263;
		t[264] = (void*)stub264;
		t[265] = (void*)stub265;
		t[266] = (void*)stub266;
		t[267] = (void*)stub267;
		t[268] = (void*)stub268;
		t[269] = (void*)stub269;
		t[270] = (void*)stub270;
		t[271] = (void*)stub271;
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
