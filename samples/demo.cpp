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
#define __CL_ENABLE_EXCEPTIONS

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <iostream>
#include <sstream>
#include <iomanip>

inline std::string memSuffix(const size_t s)
{
	std::stringstream buf;
	buf.precision(3);
	if (s >= 0x10000000000LU)
		buf << (double)(s >> 30) / 1024.0 << "TB";
	else if (s >= 0x40000000LU)
		buf << (double)(s >> 20) / 1024.0 << "GB";
	else if (s >= 0x100000LU)
		buf << (double)(s >> 10) / 1024.0 << "MB";
	else if (s >= 0x400LU)
		buf << (double)s / 1024.0 << "KB";
	else
		buf << s << "B";
	return buf.str();
}

int main(void)
{
	try
	{
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);

		std::cout << platforms.size() << " platform" << (platforms.size() > 1 ? "s" : "") << " found:" << std::endl;
		for(size_t i = 0 ; i < platforms.size() ; ++i)
		{
			const cl::Platform &p = platforms[i];
			std::cout
					<< "\tplatform " << i << ':' << std::endl
					<< "\t\tname: " << p.getInfo<CL_PLATFORM_NAME>() << std::endl
					<< "\t\tprofile: " << p.getInfo<CL_PLATFORM_PROFILE>() << std::endl
					<< "\t\tversion: " << p.getInfo<CL_PLATFORM_VERSION>() << std::endl
					<< "\t\tvendor: " << p.getInfo<CL_PLATFORM_VENDOR>() << std::endl
					<< "\t\textensions: " << p.getInfo<CL_PLATFORM_EXTENSIONS>() << std::endl;

			std::vector<cl::Device> devices;
			p.getDevices(CL_DEVICE_TYPE_ALL, &devices);

			std::cout << "\t\t" << devices.size() << " device" << (devices.size() > 1 ? "s" : "") << " found:" << std::endl;
			for(size_t j = 0 ; j < devices.size() ; ++j)
			{
				cl::Device &dev = devices[j];
				std::cout
						<< "\t\t\tdevice " << j << ':' << std::endl
						<< "\t\t\t\tname: " << dev.getInfo<CL_DEVICE_NAME>() << std::endl
						<< "\t\t\t\tversion: " << dev.getInfo<CL_DEVICE_VERSION>() << std::endl
						<< "\t\t\t\tvendor: " << dev.getInfo<CL_DEVICE_VENDOR>() << std::endl
						<< "\t\t\t\tmemory: " << memSuffix(dev.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()) << std::endl
						<< "\t\t\t\tendianess: " << (dev.getInfo<CL_DEVICE_ENDIAN_LITTLE>() ? "little endian" : "big endian") << std::endl
						<< "\t\t\t\textensions: " << dev.getInfo<CL_DEVICE_EXTENSIONS>() << std::endl;
			}
		}
	}
	catch(cl::Error err)
	{
		std::cerr << err.what() << std::endl;
		return -1;
	}

	return 0;
}
