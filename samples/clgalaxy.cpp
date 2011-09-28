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

#include <GL/gl.h>
#include <GL/glu.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <SDL.h>

#include "errors.h"

using namespace std;

const char *source_code_str =
	"#define dt 1e-2f\n"
	"__kernel void gravity(__global const float4 *in_pos, __global float4 *out_pos,\n"
	"					  __global const float4 *in_vel, __global float4 *out_vel,\n"
	"					  const uint nb_particles,\n"
	"					  const uint step)\n"
	"{\n"
	"	const uint i = get_global_id(0);\n"
	"	if (i >= nb_particles)\n"
	"		return;\n"
	"	float4 pos = in_pos[i];\n"
	"	float4 a = (float4)(0.0f, 0.0f, 0.0f, 0.0f);\n"
	"	for(uint j = 0 ; j < nb_particles ; j += step)\n"
	"	{\n"
	"		float4 p = in_pos[j] - pos;\n"
	"		p.w = 0.0f;\n"
	"		float d2 = dot(p, p);\n"
	"		p /= sqrt(d2);\n"
	"		if (isnan(p.x) || isnan(p.y) || isnan(p.z) || i == j)\n"
	"			continue;\n"
	"		p.w = 0.0f;\n"
	"		d2 = max(d2, 1e0f);\n"
	"		a += (1.0f / d2) * p;\n"
	"	}\n"
	"	a *= step;\n"
	"	float4 v = in_vel[i] + dt * a;\n"
	"	out_vel[i] = v;\n"
	"	out_pos[i] = pos + dt * v;\n"
	"}\n"
	;

int main(int argc, const char **argv)
{
	size_t platform_id = -1;
	for(size_t i = 0 ; i < argc ; ++i)
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

		if (platform_id == -1 || platform_id >= platforms.size())
		{
			cerr << "error : no platform found" << endl;
			return 0;
		}

		cout << "platform selected : " << platforms[platform_id].getInfo<CL_PLATFORM_NAME>() << endl;

		const cl_uint step = 1024;
		const cl_uint nb_particles = 512 * step;
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
			program.build(devices);
		}
		catch(const cl::Error &err)
		{
			std::cerr << "build error :" << err.what() << '(' << get_error_as_string(err.err()) << ')' << endl;
			std::cerr << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.front()) << std::endl;
		}

		cl::Kernel gravity(program, "gravity");

		cl::Buffer pos0(context, CL_MEM_READ_WRITE, sizeof(cl_float4) * nb_particles);
		cl::Buffer pos1(context, CL_MEM_READ_ONLY, sizeof(cl_float4) * nb_particles);
		cl::Buffer vel0(context, CL_MEM_READ_WRITE, sizeof(cl_float4) * nb_particles);
		cl::Buffer vel1(context, CL_MEM_READ_ONLY, sizeof(cl_float4) * nb_particles);
		cl_float4 *p_pos = (cl_float4*)queue.enqueueMapBuffer(pos0,
															  true,
															  CL_MEM_WRITE_ONLY,
															  0,
															  sizeof(cl_float4) * nb_particles);
		cl_float4 *p_vel = (cl_float4*)queue.enqueueMapBuffer(vel0,
															  true,
															  CL_MEM_WRITE_ONLY,
															  0,
															  sizeof(cl_float4) * nb_particles);

		for(size_t i = 0 ; i < nb_particles ; ++i)
		{
			const double rho = r * pow((double(rand()) / RAND_MAX), 0.75);
			double theta = (double(rand()) / RAND_MAX) * (M_PI * 2.0);
			theta = (0.5 * cos(2.0 * theta) + theta - 1e-2 * rho);
			const double z = 2.0 * (double(rand()) / RAND_MAX) - 1.0;
			const double c = cos(theta);
			const double s = sin(theta);
			p_pos[i].s[0] = rho * c;
			p_pos[i].s[1] = rho * s;
			p_pos[i].s[2] = z;
			p_pos[i].s[3] = 1.0;

			const double a = 1.0e0 * (rho <= 1e-1 ? 0.0 : rho);
			p_vel[i].s[0] = -a * s;
			p_vel[i].s[1] = a * c;
			p_vel[i].s[2] = 0.0f;
			p_vel[i].s[3] = 0.0f;
		}

		queue.enqueueUnmapMemObject(pos0, p_pos);
		queue.enqueueUnmapMemObject(vel0, p_vel);
		queue.finish();

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTTHREAD);

		SDL_Surface *screen = SDL_SetVideoMode(1024, 768, 32, SDL_OPENGL | SDL_DOUBLEBUF);
		bool done = false;
		glDisable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glClearColor(0,0,0,0);
		glColor4f(0.3f, 0.4f, 1.0f, 5e-2f);
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
					}

					break;
				}
			}

			gravity.setArg(0, pos0);
			gravity.setArg(1, pos1);
			gravity.setArg(2, vel0);
			gravity.setArg(3, vel1);
			gravity.setArg(4, nb_particles);
			gravity.setArg(5, step);
			queue.enqueueNDRangeKernel(gravity, cl::NDRange(), cl::NDRange(nb_particles), cl::NDRange(512));
			std::swap(pos0, pos1);
			std::swap(vel0, vel1);
			queue.finish();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			p_pos = (cl_float4*)queue.enqueueMapBuffer(pos0,
													   true,
													   CL_MEM_READ_ONLY,
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
