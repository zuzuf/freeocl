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

#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <SDL.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846264
#endif

#include "errors.h"

using namespace std;

const char *source_code_str =
	"#define dt 3e-2f\n"
	"\n"
	"inline float4 computeAcceleration(const float4 pos, const uint n, __global const float4 *in_pos)\n"
	"{\n"
	"	float4 a = (float4)(0.0f, 0.0f, 0.0f, 0.0f);\n"
	"	const uint i = get_global_id(0);\n"
	"	for(uint j = 0 ; j < n ; ++j)\n"
	"	{\n"
	"		if (i == j)	++j;\n"
	"		float4 p = in_pos[j] - pos;	p.w = 0.0f;\n"
	"		const float d2 = dot(p, p);\n"
    "		a += (native_recip(1e-1f + d2) - native_recip(1e-3f + d2 * d2) ) * native_rsqrt(d2 + 1e-2f) * (p);\n"
	"	}\n"
	"	return a;\n"
	"}\n"
	"\n"
	"__kernel void rk1(__global const float4 *in_pos,\n"
	"				   __global const float4 *in_vel,\n"
	"				   __global float4 *out_pos,\n"
	"				   __global float4 *out_acc,\n"
	"				   const uint nb_particles)\n"
	"{\n"
	"	const uint i = get_global_id(0);\n"
	"	if (i >= nb_particles)\n"
	"		return;\n"
	"	const float4 pos = in_pos[i];\n"
	"	const float4 a = computeAcceleration(pos, nb_particles, in_pos);\n"
	"	out_acc[i] = a;\n"
    "	const float4 v = in_vel[i] + 0.5f * dt * a;\n"
    "	out_pos[i] = pos + 0.5f * dt * v;\n"
	"}\n"
	"\n"
	"__kernel void rk2(__global const float4 *in_pos,\n"
	"				   __global const float4 *in_pos0,\n"
	"				   __global const float4 *in_vel,\n"
	"				   __global float4 *out_pos,\n"
	"				   __global float4 *out_acc,\n"
	"				   const uint nb_particles)\n"
	"{\n"
	"	const uint i = get_global_id(0);\n"
	"	if (i >= nb_particles)\n"
	"		return;\n"
	"	const float4 pos = in_pos[i];\n"
	"	const float4 a = computeAcceleration(pos, nb_particles, in_pos);\n"
    "	out_acc[i] += a * 2.0f;\n"
    "	const float4 v = in_vel[i] + 0.5f * dt * a;\n"
    "	out_pos[i] = in_pos0[i] + 0.5f * dt * v;\n"
	"}\n"
	"\n"
	"__kernel void rk3(__global const float4 *in_pos,\n"
	"				   __global const float4 *in_pos0,\n"
	"				   __global const float4 *in_vel,\n"
	"				   __global float4 *out_pos,\n"
	"				   __global float4 *out_acc,\n"
	"				   const uint nb_particles)\n"
	"{\n"
	"	const uint i = get_global_id(0);\n"
	"	if (i >= nb_particles)\n"
	"		return;\n"
	"	const float4 pos = in_pos[i];\n"
	"	const float4 a = computeAcceleration(pos, nb_particles, in_pos);\n"
    "	out_acc[i] += a * 2.0f;\n"
	"	const float4 v = in_vel[i] + dt * a;\n"
	"	out_pos[i] = in_pos0[i] + dt * v;\n"
	"}\n"
	"\n"
	"__kernel void rk4(__global const float4 *in_pos,\n"
	"				   __global const float4 *in_pos0,\n"
	"				   __global const float4 *in_acc,\n"
	"				   __global float4 *out_pos,\n"
	"				   __global float4 *out_vel,\n"
	"				   const uint nb_particles)\n"
	"{\n"
	"	const uint i = get_global_id(0);\n"
	"	if (i >= nb_particles)\n"
	"		return;\n"
	"	const float4 pos = in_pos[i];\n"
	"	float4 a = computeAcceleration(pos, nb_particles, in_pos);\n"
	"	a = (a + in_acc[i]) * (1.0f / 6.0f);\n"
	"	const float4 v = out_vel[i] + dt * a;\n"
	"	out_vel[i] = v;\n"
	"	out_pos[i] = in_pos0[i] + dt * v;\n"
	"}\n"
	;

class KernelFunctor
{
public:
    KernelFunctor(const cl::Kernel &kernel, const cl::CommandQueue &queue, const cl::NDRange &global, const cl::NDRange &local)
        : queue(queue),
          kernel(kernel),
          global(global),
          local(local)
    {}

    template<typename T0, typename T1, typename T2, typename T3, typename T4>
    cl_int operator()(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4)
    {
        kernel.setArg(0, v0);
        kernel.setArg(1, v1);
        kernel.setArg(2, v2);
        kernel.setArg(3, v3);
        kernel.setArg(4, v4);
        return queue.enqueueNDRangeKernel(kernel, cl::NDRange(), global, local);
    }

    template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
    cl_int operator()(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3, const T4 &v4, const T5 &v5)
    {
        kernel.setArg(0, v0);
        kernel.setArg(1, v1);
        kernel.setArg(2, v2);
        kernel.setArg(3, v3);
        kernel.setArg(4, v4);
        kernel.setArg(5, v5);
        return queue.enqueueNDRangeKernel(kernel, cl::NDRange(), global, local);
    }

private:
    const cl::CommandQueue &queue;
    cl::Kernel kernel;
    const cl::NDRange global;
    const cl::NDRange local;
};

int main(int argc, char *argv[])
{
	int platform_id = -1;
	for(int i = 0 ; i < argc ; ++i)
	{
		if (!strcmp(argv[i], "--platform")
				|| !strcmp(argv[i], "-p"))
		{
			++i;
			if (i < argc)
				platform_id = atoi(argv[i]);
		}
	}

	init_signal_handler();

	try
	{
		vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		if (platform_id == -1)
		{
			size_t p_id = -1;
			for(size_t i = 0 ; i < platforms.size() ; ++i)
			{
				const cl::Platform &p = platforms[i];

				if (p.getInfo<CL_PLATFORM_NAME>() == "FreeOCL")
					platform_id = i;

				vector<cl::Device> devices;
				p.getDevices(CL_DEVICE_TYPE_ALL, &devices);
				if (devices.empty())
					continue;

				p_id = i;

				cout << "platform " << i << " : " << p.getInfo<CL_PLATFORM_NAME>() << endl;
			}
			if (platform_id == -1)
				platform_id = p_id;
		}

		if (platform_id == -1 || platform_id >= (int)platforms.size())
		{
			cerr << "error : no platform found" << endl;
			return 0;
		}

		cout << "platform selected : " << platforms[platform_id].getInfo<CL_PLATFORM_NAME>() << endl;

		const cl_uint nb_particles = 8192;
		const double r = 1e2;

		vector<cl::Device> devices;
		platforms[platform_id].getDevices(CL_DEVICE_TYPE_ALL, &devices);
		devices.resize(1);
		cl_context_properties properties[] = { CL_CONTEXT_PLATFORM,
											   (cl_context_properties)(platforms[platform_id])(),
											   0};
		cl::Context context(devices, properties);
		cl::CommandQueue queue(context, devices.front());

		cl::Program::Sources sources;
		sources.push_back(make_pair(source_code_str, strlen(source_code_str)));
		cl::Program program(context, sources);
		try
		{
			program.build(devices, "-cl-fast-relaxed-math");
		}
		catch(const cl::Error &err)
		{
			std::cerr << "build error :" << err.what() << '(' << get_error_as_string(err.err()) << ')' << endl;
			std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << std::endl;
		}

		cl::Kernel k_rk1(program, "rk1");
		cl::Kernel k_rk2(program, "rk2");
		cl::Kernel k_rk3(program, "rk3");
		cl::Kernel k_rk4(program, "rk4");

        KernelFunctor rk1(k_rk1, queue, cl::NDRange(nb_particles), cl::NDRange());
        KernelFunctor rk2(k_rk2, queue, cl::NDRange(nb_particles), cl::NDRange());
        KernelFunctor rk3(k_rk3, queue, cl::NDRange(nb_particles), cl::NDRange());
        KernelFunctor rk4(k_rk4, queue, cl::NDRange(nb_particles), cl::NDRange());

		cl::Buffer pos0(context, CL_MEM_READ_WRITE, sizeof(cl_float4) * nb_particles);
		cl::Buffer pos1(context, CL_MEM_READ_ONLY, sizeof(cl_float4) * nb_particles);
		cl::Buffer pos2(context, CL_MEM_READ_ONLY, sizeof(cl_float4) * nb_particles);
		cl::Buffer acc(context, CL_MEM_READ_ONLY, sizeof(cl_float4) * nb_particles);
		cl::Buffer vel(context, CL_MEM_READ_WRITE, sizeof(cl_float4) * nb_particles);
		cl_float4 *p_pos = (cl_float4*)queue.enqueueMapBuffer(pos0,
															  true,
															  CL_MEM_WRITE_ONLY,
															  0,
															  sizeof(cl_float4) * nb_particles);
		cl_float4 *p_vel = (cl_float4*)queue.enqueueMapBuffer(vel,
															  true,
															  CL_MEM_WRITE_ONLY,
															  0,
															  sizeof(cl_float4) * nb_particles);

		for(size_t i = 0 ; i < nb_particles ; ++i)
		{
			const double rho = r * pow((double(rand()) / RAND_MAX), 0.75);
//			const double rho = r * (double(rand()) / RAND_MAX);
			double theta = (double(rand()) / RAND_MAX) * (M_PI * 2.0);
			theta = (0.5 * cos(2.0 * theta) + theta - 1e-2 * rho);
			const double z = 2.0 * (double(rand()) / RAND_MAX) - 1.0;
			const double c = cos(theta);
			const double s = sin(theta);
			p_pos[i].s[0] = rho * c;
			p_pos[i].s[1] = rho * s;
			p_pos[i].s[2] = z;
			p_pos[i].s[3] = 1.0;

			const double a = 6.0e-2 * rho;//(rho <= 1e-1 ? 0.0 : rho);
			p_vel[i].s[0] = -a * s;
			p_vel[i].s[1] = a * c;
			p_vel[i].s[2] = 0.0f;
			p_vel[i].s[3] = 0.0f;
		}

		queue.enqueueUnmapMemObject(pos0, p_pos);
		queue.enqueueUnmapMemObject(vel, p_vel);
		queue.finish();

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTTHREAD);

		SDL_Surface *screen = SDL_SetVideoMode(1024, 768, 32, SDL_OPENGL | SDL_DOUBLEBUF | SDL_ASYNCBLIT);
		bool done = false;
		glDisable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glClearColor(0,0,0,0);
		glColor4f(0.06f, 0.08f, 1.0f, 5e-2f * 4.0);
		glPointSize(2.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, 4.0 / 3.0, 1e0, 1e4);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0, 0.0, 450.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		while(!done)
		{

			SDL_Event e;
			while(SDL_PollEvent(&e))
			{
				switch(e.type)
				{
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						done = true;
						break;
					default:
						break;
					}

					break;
				}
			}

			rk1(pos0, vel, pos1, acc, nb_particles);
			rk2(pos1, pos0, vel, pos2, acc, nb_particles);
			rk3(pos2, pos0, vel, pos1, acc, nb_particles);
			rk4(pos1, pos0, acc, pos2, vel, nb_particles);
			std::swap(pos0, pos2);
			queue.finish();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			p_pos = (cl_float4*)queue.enqueueMapBuffer(pos0,
													   true,
													   CL_MAP_READ,
													   0,
													   sizeof(cl_float4) * nb_particles);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(4, GL_FLOAT, 0, p_pos);

			glDrawArrays(GL_POINTS, 0, nb_particles);
			glFinish();
			SDL_GL_SwapBuffers();

			queue.enqueueUnmapMemObject(pos0, p_pos);
		}
	}
	catch(cl::Error err)
	{
		cerr << "error: " << err.what() << '(' << get_error_as_string(err.err()) << ')' << endl;
		return -1;
	}

	return 0;
}
