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
#include "program.h"
#include "context.h"
#include <cstring>
#include <dlfcn.h>
#include "codebuilder.h"
#include <sstream>

#define SET_STRING(X)	FreeOCL::copy_memory_within_limits(X, strlen(X) + 1, param_value_size, param_value, param_value_size_ret)
#define SET_VAR(X)	FreeOCL::copy_memory_within_limits(&(X), sizeof(X),\
														param_value_size,\
														param_value,\
														param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_program clCreateProgramWithSourceFCL (cl_context context,
										  cl_uint count,
										  const char **strings,
										  const size_t *lengths,
										  cl_int *errcode_ret)
	{
		MSG(clCreateProgramWithSourceFCL);
		if (count == 0 || strings == NULL)
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		std::string source_code;
		for(size_t i = 0 ; i < count ; ++i)
		{
			if (strings[i] == NULL)
			{
				SET_RET(CL_INVALID_VALUE);
				return 0;
			}
			const size_t len = (lengths == NULL || lengths[i] == 0) ? strlen(strings[i]) : lengths[i];
			source_code.append(strings[i], len);
		}

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return 0;
		}
		unlock.handle(context);

		cl_program program = new _cl_program(context);
		program->source_code.swap(source_code);
		program->build_status = CL_BUILD_NONE;
		program->handle = NULL;
		SET_RET(CL_SUCCESS);

		return program;
	}

	cl_program clCreateProgramWithBinaryFCL (cl_context context,
										  cl_uint num_devices,
										  const cl_device_id *device_list,
										  const size_t *lengths,
										  const unsigned char **binaries,
										  cl_int *binary_status,
										  cl_int *errcode_ret)
	{
		MSG(clCreateProgramWithBinaryFCL);
		SET_RET(CL_INVALID_OPERATION);
		return 0;
	}

	cl_int clRetainProgramFCL (cl_program program)
	{
		MSG(clRetainProgramFCL);
		if (!FreeOCL::is_valid(program))
			return CL_INVALID_PROGRAM;
		program->retain();
		program->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseProgramFCL (cl_program program)
	{
		MSG(clReleaseProgramFCL);
		if (!FreeOCL::is_valid(program))
			return CL_INVALID_PROGRAM;
		program->release();
		if (program->get_ref_count() == 0)
		{
			program->invalidate();
			program->unlock();
			delete program;
		}
		else
			program->unlock();
		return CL_SUCCESS;
	}

	cl_int clBuildProgramFCL (cl_program program,
						   cl_uint num_devices,
						   const cl_device_id *device_list,
						   const char *options,
						   void (CL_CALLBACK *pfn_notify)(cl_program program,
														  void *user_data),
						   void *user_data)
	{
		MSG(clBuildProgramFCL);
		if (device_list == NULL && num_devices > 0)
			return CL_INVALID_VALUE;
		if (pfn_notify == NULL && user_data != NULL)
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(program))
			return CL_INVALID_PROGRAM;
		program->retain();

		program->build_status = CL_BUILD_IN_PROGRESS;
		const std::string source_code = program->source_code;

		if (program->handle)
			dlclose(program->handle);
		if (!program->binary_file.empty())
			remove(program->binary_file.c_str());
		program->handle = NULL;
		program->binary_file.clear();
		program->unlock();

		std::stringstream build_log;
		bool b_valid_options = true;
		FreeOCL::set<std::string> kernel_names;
		const std::string binary_file = FreeOCL::build_program(options ? options : std::string(),
															   source_code,
															   build_log,
															   kernel_names,
															   b_valid_options);

		if (!b_valid_options)
			return CL_INVALID_BUILD_OPTIONS;

		if (!FreeOCL::is_valid(program))
		{
			if (!binary_file.empty())
				remove(binary_file.c_str());
			return CL_INVALID_PROGRAM;
		}

		program->binary_file = binary_file;
		program->build_log = build_log.str();

		if (program->binary_file.empty())
		{
			program->build_status = CL_BUILD_ERROR;
			program->unlock();
			clReleaseProgramFCL(program);
			return CL_BUILD_PROGRAM_FAILURE;
		}

		program->handle = dlopen(binary_file.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (!program->handle)
		{
			remove(program->binary_file.c_str());
			program->binary_file.clear();
			program->build_status = CL_BUILD_ERROR;
			program->unlock();
			clReleaseProgramFCL(program);
			return CL_BUILD_PROGRAM_FAILURE;
		}

		program->kernel_names = kernel_names;

		program->build_status = CL_BUILD_SUCCESS;
		program->unlock();
		clReleaseProgramFCL(program);
		return CL_SUCCESS;
	}

	cl_int clGetProgramInfoFCL (cl_program program,
							 cl_program_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		MSG(clGetProgramInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(program))
			return CL_INVALID_PROGRAM;
		unlock.handle(program);

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_PROGRAM_REFERENCE_COUNT:	bTooSmall = SET_VAR(program->get_ref_count());	break;
		case CL_PROGRAM_CONTEXT:			bTooSmall = SET_VAR(program->context);	break;
		case CL_PROGRAM_NUM_DEVICES:
			{
				const cl_uint nb = program->devices.size();
				bTooSmall = SET_VAR(nb);
			}
			break;
		case CL_PROGRAM_DEVICES:
			bTooSmall = FreeOCL::copy_memory_within_limits(&(program->devices.front()),
														   program->devices.size() * sizeof(cl_device_id),
														   param_value_size,
														   param_value,
														   param_value_size_ret);
			break;
		case CL_PROGRAM_SOURCE:
			bTooSmall = FreeOCL::copy_memory_within_limits(program->source_code.c_str(),
														   program->source_code.size() + 1,
														   param_value_size,
														   param_value,
														   param_value_size_ret);
			break;
		case CL_PROGRAM_BINARY_SIZES:
			{
				std::vector<size_t> sizes;
				sizes.resize(program->devices.size(), 0);
				bTooSmall = FreeOCL::copy_memory_within_limits(&(sizes.front()),
															   sizes.size() * sizeof(size_t),
															   param_value_size,
															   param_value,
															   param_value_size_ret);
			}
			break;
		case CL_PROGRAM_BINARIES:
			break;
		case CL_PROGRAM_NUM_KERNELS:
			if (!program->handle)
				return CL_INVALID_PROGRAM_EXECUTABLE;
			else
			{
				const size_t tmp = program->kernel_names.size();
				bTooSmall = SET_VAR(tmp);
			}
			break;
		case CL_PROGRAM_KERNEL_NAMES:
			if (!program->handle)
				return CL_INVALID_PROGRAM_EXECUTABLE;
			else
			{
				std::string names;
				for(FreeOCL::set<std::string>::const_iterator it = program->kernel_names.begin() ; it != program->kernel_names.end() ; ++it)
				{
					if (!names.empty())
						names += ';';
					names += *it;
				}
				bTooSmall = SET_STRING(names.c_str());
			}
			break;
		default:
			return CL_INVALID_VALUE;
		}

		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clGetProgramBuildInfoFCL (cl_program program,
								  cl_device_id device,
								  cl_program_build_info param_name,
								  size_t param_value_size,
								  void *param_value,
								  size_t *param_value_size_ret)
	{
		MSG(clGetProgramBuildInfoFCL);
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(program))
			return CL_INVALID_PROGRAM;
		unlock.handle(program);

		if (!FreeOCL::is_valid(device))
			return CL_INVALID_DEVICE;

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_PROGRAM_BUILD_STATUS:		bTooSmall = SET_VAR(program->build_status);	break;
		case CL_PROGRAM_BUILD_OPTIONS:
			bTooSmall = FreeOCL::copy_memory_within_limits(program->build_options.c_str(),
														   program->build_options.size() + 1,
														   param_value_size,
														   param_value,
														   param_value_size_ret);
			break;
		case CL_PROGRAM_BUILD_LOG:
			bTooSmall = FreeOCL::copy_memory_within_limits(program->build_log.c_str(),
														   program->build_log.size() + 1,
														   param_value_size,
														   param_value,
														   param_value_size_ret);
			break;
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	CL_API_ENTRY cl_program CL_API_CALL clCreateProgramWithBuiltInKernelsFCL(cl_context            context,
																			 cl_uint               num_devices,
																			 const cl_device_id *  device_list,
																			 const char *          kernel_names,
																			 cl_int *              errcode_ret) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clCreateProgramWithBuiltInKernelsFCL);
		if (device_list == NULL && num_devices > 0)
		{
			SET_RET(CL_INVALID_VALUE);
			return NULL;
		}
		if (!kernel_names)
		{
			SET_RET(CL_INVALID_VALUE);
			return NULL;
		}

		for(size_t i = 0 ; i < num_devices ; ++i)
			if (device_list[i] != FreeOCL::device)
			{
				SET_RET(CL_INVALID_DEVICE);
				return NULL;
			}

		const std::deque<std::string> &splitted_kernel_names = FreeOCL::split(kernel_names, ";");
		void *main_program = dlopen(NULL, RTLD_NOLOAD);
		for(std::deque<std::string>::const_iterator i = splitted_kernel_names.begin() ; i != splitted_kernel_names.end() ; ++i)
		{
			if (!dlsym(main_program, ("__FCL_info_" + *i).c_str())
					|| !dlsym(main_program, ("__FCL_kernel_" + *i).c_str()))
			{
				SET_RET(CL_INVALID_VALUE);
				return NULL;
			}
		}

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return NULL;
		}
		unlock.handle(context);

		cl_program program = new _cl_program(context);
		program->source_code.clear();
		program->build_status = CL_BUILD_NONE;
		program->handle = main_program;
		program->binary_file.clear();
		program->devices.push_back(FreeOCL::device);

		FreeOCL::set<std::string> s_kernel_names;
		s_kernel_names.insert(splitted_kernel_names.begin(), splitted_kernel_names.end());

		program->kernel_names = s_kernel_names;

		program->build_status = CL_BUILD_SUCCESS;

		SET_RET(CL_SUCCESS);
		return program;
	}

	CL_API_ENTRY cl_int CL_API_CALL	clCompileProgramFCL(cl_program           program,
														cl_uint              num_devices,
														const cl_device_id * device_list,
														const char *         options,
														cl_uint              num_input_headers,
														const cl_program *   input_headers,
														const char **        header_include_names,
														void (CL_CALLBACK *  pfn_notify)(cl_program /* program */, void * /* user_data */),
														void *               user_data) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clCompileProgramFCL);
	}

	CL_API_ENTRY cl_program CL_API_CALL	clLinkProgramFCL(cl_context           context,
														 cl_uint              num_devices,
														 const cl_device_id * device_list,
														 const char *         options,
														 cl_uint              num_input_programs,
														 const cl_program *   input_programs,
														 void (CL_CALLBACK *  pfn_notify)(cl_program /* program */, void * /* user_data */),
														 void *               user_data,
														 cl_int *             errcode_ret) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clLinkProgramFCL);
	}

	CL_API_ENTRY cl_int CL_API_CALL clUnloadPlatformCompilerFCL(cl_platform_id platform) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clUnloadPlatformCompilerFCL);
		if (platform != FreeOCL::platform)
			return CL_INVALID_PLATFORM;
		return CL_SUCCESS;
	}
}

_cl_program::_cl_program(cl_context context) : context_resource(context), handle(NULL), build_status(CL_BUILD_NONE)
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_programs.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_program::~_cl_program()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_programs.erase(this);
	FreeOCL::global_mutex.unlock();

	if (handle && !binary_file.empty())
		dlclose(handle);
	if (!binary_file.empty())
		remove(binary_file.c_str());
}
