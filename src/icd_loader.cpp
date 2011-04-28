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
#include "icd_loader.h"
#include <cstdio>
#include <fstream>
#include <dlfcn.h>

namespace FreeOCL
{
	ICDLoader icd_loader;

	std::string ICDLoader::runCommand(const std::string &cmd)
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

	std::string ICDLoader::trim(const std::string &s)
	{
		if (s.empty())
			return s;
		const size_t start = s.find_first_not_of(" \t\n\r");
		const size_t end = s.find_last_not_of(" \t\n\r");
		if (start == end)
			return std::string();
		return s.substr(start, end - start + 1);
	}

	std::deque<std::string> ICDLoader::split(const std::string &s, const std::string &sep)
	{
		std::deque<std::string> result;

		size_t pos = s.find_first_not_of(sep, 0);
		do
		{
			size_t next = s.find_first_of(sep, pos);
			result.push_back(s.substr(pos, next - pos));
			if (next != std::string::npos)
				pos = s.find_first_not_of(sep, next);
			else
				pos = std::string::npos;
		} while(pos != std::string::npos);

		return result;
	}

	ICDLoader::ICDLoader()
	{
		// Get the list of all *.icd files in /etc/OpenCL/vendors/
		std::deque<std::string> files = split(trim(runCommand("ls -1 /etc/OpenCL/vendors/*.icd")), "\n");
		// For each file
		for(std::deque<std::string>::const_iterator i = files.begin() ; i != files.end() ; ++i)
		{
			std::ifstream file(i->c_str(), std::ios_base::in);
			if (!file.is_open())
				continue;
			// Read the corresponding library name
			std::string lib;
			file >> lib;
			// And load it
			load(lib);
			file.close();
		}
	}

	ICDLoader::~ICDLoader()
	{

	}

	void ICDLoader::load(const std::string &lib)
	{
		void *handle = dlopen(lib.c_str(), RTLD_LAZY);

		// If dlopens fails, stop here
		if (!handle)
			return;
		typedef cl_int (*fn_clIcdGetPlatformIDsKHR) (cl_uint, cl_platform_id *, cl_uint *);
		typedef cl_int (*fn_clGetPlatformInfo) (cl_platform_id, cl_platform_info, size_t, void *, size_t *);
		typedef void* (*fn_clGetExtensionFunctionAddress) (const char *funcname);
		fn_clIcdGetPlatformIDsKHR __clIcdGetPlatformIDsKHR = (fn_clIcdGetPlatformIDsKHR) dlsym(handle, "clIcdGetPlatformIDsKHR");
		fn_clGetPlatformInfo __clGetPlatformInfo = (fn_clGetPlatformInfo) dlsym(handle, "clGetPlatformInfo");
		fn_clGetExtensionFunctionAddress __clGetExtensionFunctionAddress = (fn_clGetExtensionFunctionAddress) dlsym(handle, "clGetExtensionFunctionAddress");

		// If any of these function is missing we must stop
		if (!__clGetExtensionFunctionAddress
			|| !__clGetPlatformInfo
			|| !__clIcdGetPlatformIDsKHR)
			return;

		cl_uint n;
		cl_int err;
		err = __clIcdGetPlatformIDsKHR(0, NULL, &n);
		if (err != CL_SUCCESS)
			return;

		if (n == 0)		// If this ICD is useless
			return;

		std::vector<cl_platform_id> platforms;
		platforms.resize(n);
		err = __clIcdGetPlatformIDsKHR(n, &(platforms.front()), NULL);
		if (err != CL_SUCCESS)
			return;

		ICDLib icdlib;
		icdlib.handle = handle;
		icdlib.lib = lib;

		// Check those platforms for the cl_khr_icd extension
		for(size_t i = 0 ; i < n ; ++i)
		{
			size_t size;
			err = __clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 0, NULL, &size);
			if (err != CL_SUCCESS || size == 0)
				continue;
			char *s = (char*)malloc(size);
			if (s == NULL)
				continue;

			err = __clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, size, s, NULL);
			if (err != CL_SUCCESS)
				continue;
			const bool bICDsupported = strstr(s, "cl_khr_icd") != NULL;
			free(s);

			if (!bICDsupported)
				continue;

			err = __clGetPlatformInfo(platforms[i], CL_PLATFORM_ICD_SUFFIX_KHR, 0, NULL, &size);
			if (err != CL_SUCCESS || size == 0)
				continue;

			std::string vendor_icd_extension_suffix;
			vendor_icd_extension_suffix.resize(size);
			err = __clGetPlatformInfo(platforms[i], CL_PLATFORM_ICD_SUFFIX_KHR, size, &(*vendor_icd_extension_suffix.begin()), NULL);
			if (err != CL_SUCCESS)
				continue;

			cl_uint nb_devices;
			err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &nb_devices);
			if (err != CL_SUCCESS || nb_devices == 0)		// We need the device list to check for valid devices
				continue;
			std::vector<cl_device_id> devices;
			devices.resize(nb_devices);
			err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, nb_devices, &(devices.front()), NULL);
			if (err != CL_SUCCESS)
				continue;

			icdlib.platforms.push_back(std::make_pair(platforms[i], vendor_icd_extension_suffix));
			v_platforms.push_back(platforms[i]);
			valid_platforms.insert(platforms[i]);
			valid_devices.insert(devices.begin(), devices.end());
		}

		// All returned platforms were invalid
		if (icdlib.platforms.empty())
			return;

		libs.push_back(icdlib);
	}
}
