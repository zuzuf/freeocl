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
#include "icd_loader.h"
#include "platform.h"
#include <cstdio>
#include <fstream>
#include <dlfcn.h>
#include <cstdlib>
#include <dirent.h>
#ifdef FREEOCL_OS_WINDOWS
#include <windows.h>
#endif

namespace FreeOCL
{
	icd_loader icd_loader_instance;

	std::deque<std::string> icd_loader::list_files(const std::string &path, const std::string &ext)
	{
		std::deque<std::string> result;

		DIR *dir = opendir(path.c_str());
		if (!dir)
			return result;

		struct dirent *ent;

		while ((ent = readdir(dir)) != NULL)
		{
			const std::string filename = ent->d_name;
			if (filename.find('.') == std::string::npos || filename.size() < 4)
				continue;

			if (filename.substr(filename.size() - 4) == ext)
				result.push_back(path + filename);
		}

		closedir(dir);
		return result;
	}

	icd_loader::icd_loader()
	{
#ifdef FREEOCL_ALWAYS_LOAD_FREEOCL
        bool b_FreeOCL_loaded = false;
#endif
#ifdef FREEOCL_OS_WINDOWS
        HKEY key;
        RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\OpenCL\\Vendors", 0, KEY_QUERY_VALUE | KEY_READ, &key);
        for(int i = 0 ; true ; ++i)
        {
            char valueName[261];
            memset(valueName, 0, sizeof(valueName));
            DWORD valueNameSize = sizeof(valueName) - 1;
            DWORD type;
            DWORD data;
            DWORD data_size = sizeof(data);
            if (RegEnumValueA(key, i, valueName, &valueNameSize, NULL, &type, (LPBYTE)&data, &data_size) != 0)
                break;
            if (type == REG_DWORD && data == 0)
            {
                valueName[valueNameSize] = 0;
                load(valueName);
#ifdef FREEOCL_ALWAYS_LOAD_FREEOCL
                b_FreeOCL_loaded |= (std::string(valueName).find("libFreeOCL.dll") != std::string::npos);
#endif
            }
        }

        RegCloseKey(key);

#ifdef FREEOCL_ALWAYS_LOAD_FREEOCL
        if (!b_FreeOCL_loaded)
            load("libFreeOCL.dll");
#endif

#else
		// Get the list of all *.icd files in /etc/OpenCL/vendors/
		const std::deque<std::string> &files = list_files("/etc/OpenCL/vendors/", ".icd");
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
#ifdef FREEOCL_ALWAYS_LOAD_FREEOCL
            b_FreeOCL_loaded |= (lib.find("libFreeOCL.so") != std::string::npos);
#endif
        }
#ifdef FREEOCL_ALWAYS_LOAD_FREEOCL
        if (!b_FreeOCL_loaded)
            load("libFreeOCL.so");
#endif

#endif
	}

	icd_loader::~icd_loader()
	{

	}

	void icd_loader::load(const std::string &lib)
	{
#define FAIL()	return

		void *handle = dlopen(lib.c_str(), RTLD_NOW | RTLD_LOCAL);

		// If dlopens fails, stop here
		if (!handle)
		{
			std::cerr << dlerror() << std::endl;
			FAIL();
		}
		typedef cl_int (*fn_clIcdGetPlatformIDsKHR) (cl_uint, cl_platform_id *, cl_uint *);
		typedef cl_int (*fn_clGetPlatformInfo) (cl_platform_id, cl_platform_info, size_t, void *, size_t *);
		typedef void* (*fn_clGetExtensionFunctionAddress) (const char *funcname);
		fn_clIcdGetPlatformIDsKHR __clIcdGetPlatformIDsKHR = (fn_clIcdGetPlatformIDsKHR) dlsym(handle, "clIcdGetPlatformIDsKHR");
		fn_clGetPlatformInfo __clGetPlatformInfo = (fn_clGetPlatformInfo) dlsym(handle, "clGetPlatformInfo");
		fn_clGetExtensionFunctionAddress __clGetExtensionFunctionAddress = (fn_clGetExtensionFunctionAddress) dlsym(handle, "clGetExtensionFunctionAddress");

		// If we don't find all the required functions but if we have
		// the one which can let us query them, let's do it this way
		if (__clGetExtensionFunctionAddress)
		{
			if (!__clIcdGetPlatformIDsKHR)
				__clIcdGetPlatformIDsKHR = (fn_clIcdGetPlatformIDsKHR) __clGetExtensionFunctionAddress("clIcdGetPlatformIDsKHR");
			if (!__clGetPlatformInfo)
				__clGetPlatformInfo = (fn_clGetPlatformInfo) __clGetExtensionFunctionAddress("clGetPlatformInfo");
		}

		// If any of these function is missing we must stop
		if (!__clGetExtensionFunctionAddress
			|| !__clGetPlatformInfo
			|| !__clIcdGetPlatformIDsKHR)
			FAIL();

		cl_uint n;
		cl_int err;
		err = __clIcdGetPlatformIDsKHR(0, NULL, &n);
		if (err != CL_SUCCESS)
			FAIL();

		if (n == 0)		// If this ICD is useless
			FAIL();

		std::vector<cl_platform_id> platforms;
		platforms.resize(n);
		err = __clIcdGetPlatformIDsKHR(n, &(platforms.front()), NULL);
		if (err != CL_SUCCESS)
			FAIL();

		icd_lib icdlib;
		icdlib.handle = handle;
		icdlib.lib = lib;
		icdlib.__clGetExtensionFunctionAddress = __clGetExtensionFunctionAddress;

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
			const bool b_icd_supported = strstr(s, "cl_khr_icd") != NULL;
			free(s);

			if (!b_icd_supported)
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
			err = platforms[i]->dispatch->clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &nb_devices);
			if (err != CL_SUCCESS || nb_devices == 0)		// We need the device list to check for valid devices
				continue;
			std::vector<cl_device_id> devices;
			devices.resize(nb_devices);
			err = platforms[i]->dispatch->clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, nb_devices, &(devices.front()), NULL);
			if (err != CL_SUCCESS)
				continue;

			icdlib.platforms.push_back(std::make_pair(platforms[i], vendor_icd_extension_suffix));
			v_platforms.push_back(platforms[i]);
			valid_platforms.insert(platforms[i]);
			valid_devices.insert(devices.begin(), devices.end());
		}

		// All returned platforms were invalid
		if (icdlib.platforms.empty())
			FAIL();

		libs.push_back(icdlib);
	}
}
