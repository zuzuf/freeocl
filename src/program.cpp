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
#include "program.h"
#include "context.h"
#include <cstring>

#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)
#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)

extern "C"
{
	cl_program clCreateProgramWithSource (cl_context context,
										  cl_uint count,
										  const char **strings,
										  const size_t *lengths,
										  cl_int *errcode_ret)
	{
		return context->dispatch->clCreateProgramWithSource(context,
															count,
															strings,
															lengths,
															errcode_ret);
	}

	cl_program clCreateProgramWithSourceFCL (cl_context context,
										  cl_uint count,
										  const char **strings,
										  const size_t *lengths,
										  cl_int *errcode_ret)
	{
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
			const size_t len = (lengths[i] == 0) ? strlen(strings[i]) : lengths[i];
			source_code.append(strings[i], len);
		}

		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return 0;
		}
		unlock.handle(context);

		cl_program program = new _cl_program;
		program->context = context;
		program->source_code.swap(source_code);
		SET_RET(CL_SUCCESS);

		return program;
	}

	cl_program clCreateProgramWithBinary (cl_context context,
										  cl_uint num_devices,
										  const cl_device_id *device_list,
										  const size_t *lengths,
										  const unsigned char **binaries,
										  cl_int *binary_status,
										  cl_int *errcode_ret)
	{
		return context->dispatch->clCreateProgramWithBinary(context,
															num_devices,
															device_list,
															lengths,
															binaries,
															binary_status,
															errcode_ret);
	}

	cl_program clCreateProgramWithBinaryFCL (cl_context context,
										  cl_uint num_devices,
										  const cl_device_id *device_list,
										  const size_t *lengths,
										  const unsigned char **binaries,
										  cl_int *binary_status,
										  cl_int *errcode_ret)
	{

	}

	cl_int clRetainProgram (cl_program program)
	{
		return program->dispatch->clRetainProgram(program);
	}

	cl_int clRetainProgramFCL (cl_program program)
	{
		if (!FreeOCL::isValid(program))
			return CL_INVALID_PROGRAM;
		program->retain();
		program->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseProgram (cl_program program)
	{
		return program->dispatch->clReleaseProgram(program);
	}

	cl_int clReleaseProgramFCL (cl_program program)
	{
		if (!FreeOCL::isValid(program))
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

	cl_int clBuildProgram (cl_program program,
						   cl_uint num_devices,
						   const cl_device_id *device_list,
						   const char *options,
						   void (CL_CALLBACK *pfn_notify)(cl_program program,
														  void *user_data),
						   void *user_data)
	{
		return program->dispatch->clBuildProgram(program,
												 num_devices,
												 device_list,
												 options,
												 pfn_notify,
												 user_data);
	}

	cl_int clBuildProgramFCL (cl_program program,
						   cl_uint num_devices,
						   const cl_device_id *device_list,
						   const char *options,
						   void (CL_CALLBACK *pfn_notify)(cl_program program,
														  void *user_data),
						   void *user_data)
	{
		if (device_list == NULL && num_devices > 0)
			return CL_INVALID_VALUE;
		if (pfn_notify == NULL && user_data != NULL)
			return CL_INVALID_VALUE;

		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(program))
			return CL_INVALID_PROGRAM;
		unlock.handle(program);

		return CL_COMPILER_NOT_AVAILABLE;
	}

	cl_int clUnloadCompiler (void)
	{
		// Guess what ? We never load the compiler as it is an external program!
		return CL_SUCCESS;
	}

	cl_int clGetProgramInfo (cl_program program,
							 cl_program_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		return program->dispatch->clGetProgramInfo(program,
												   param_name,
												   param_value_size,
												   param_value,
												   param_value_size_ret);
	}

	cl_int clGetProgramInfoFCL (cl_program program,
							 cl_program_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(program))
			return CL_INVALID_PROGRAM;
		unlock.handle(program);

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_PROGRAM_REFERENCE_COUNT:	bTooSmall = SET_VAR(program->get_ref_count());	break;
		case CL_PROGRAM_CONTEXT:			bTooSmall = SET_VAR(program->context);	break;
		case CL_PROGRAM_NUM_DEVICES:
		case CL_PROGRAM_DEVICES:
			return CL_INVALID_VALUE;
		case CL_PROGRAM_SOURCE:
			bTooSmall = FreeOCL::copyMemoryWithinLimits(program->source_code.c_str(), program->source_code.size() + 1, param_value_size, param_value, param_value_size_ret);
			break;
		case CL_PROGRAM_BINARY_SIZES:
		case CL_PROGRAM_BINARIES:
		default:
			return CL_INVALID_VALUE;
		}

		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clGetProgramBuildInfo (cl_program program,
								  cl_device_id device,
								  cl_program_build_info param_name,
								  size_t param_value_size,
								  void *param_value,
								  size_t *param_value_size_ret)
	{
		return program->dispatch->clGetProgramBuildInfo(program,
														device,
														param_name,
														param_value_size,
														param_value,
														param_value_size_ret);
	}

	cl_int clGetProgramBuildInfoFCL (cl_program program,
								  cl_device_id device,
								  cl_program_build_info param_name,
								  size_t param_value_size,
								  void *param_value,
								  size_t *param_value_size_ret)
	{
		FreeOCL::unlocker unlock;
		if (!FreeOCL::isValid(program))
			return CL_INVALID_PROGRAM;
		unlock.handle(program);

		if (!FreeOCL::isValid(device))
			return CL_INVALID_DEVICE;

		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_PROGRAM_BUILD_STATUS:
		case CL_PROGRAM_BUILD_OPTIONS:
		case CL_PROGRAM_BUILD_LOG:
		default:
			return CL_INVALID_VALUE;
		}
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}
}

_cl_program::_cl_program()
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
}
