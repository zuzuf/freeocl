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
#ifndef __FREEOCL_ICD_LOADER_H__
#define __FREEOCL_ICD_LOADER_H__

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <string>
#include <deque>
#include <utility>
#include <vector>
#include <set>

namespace FreeOCL
{
	struct ICDLib
	{
		std::string	lib;
		void *handle;
		std::deque<std::pair<cl_platform_id, std::string> > platforms;
		void* (*__clGetExtensionFunctionAddress) (const char *funcname);
	};

	class ICDLoader
	{
	public:
		ICDLoader();
		~ICDLoader();

		void load(const std::string &lib);

		inline const std::vector<cl_platform_id> &get_platforms() const	{	return v_platforms;	}

		inline const bool isValid(cl_device_id dev) const
		{
			return valid_devices.count(dev) != 0;
		}

		inline const bool isValid(cl_platform_id platform) const
		{
			return valid_platforms.count(platform) != 0;
		}

		inline const std::deque<ICDLib> &getLibs() const	{	return libs;	}

	private:
		std::deque<ICDLib> libs;
		std::vector<cl_platform_id> v_platforms;
		std::set<cl_platform_id> valid_platforms;
		std::set<cl_device_id> valid_devices;

	private:
		static std::string runCommand(const std::string &cmd);
		static std::string trim(const std::string &s);
		static std::deque<std::string> split(const std::string &s, const std::string &sep);
	};

	extern ICDLoader icd_loader;
}

#endif
