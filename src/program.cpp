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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "codebuilder.h"
#include <sstream>
#include <fstream>

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
		if (num_devices == 0 || device_list == NULL
				|| lengths == NULL || binaries == NULL)
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		if (binary_status)
		{
			bool b_error = false;
			for(size_t i = 0 ; i < num_devices ; ++i)
			{
				if (lengths[i] == 0 || binaries[i] == NULL)
				{
					binary_status[i] = CL_INVALID_VALUE;
					b_error = true;
					continue;
				}
				binary_status[i] = CL_SUCCESS;
			}
			if (b_error)
			{
				SET_RET(CL_INVALID_VALUE);
				return 0;
			}
		}

		for(size_t i = 0 ; i < num_devices ; ++i)
			if (device_list[i] != FreeOCL::device)
			{
				SET_RET(CL_INVALID_DEVICE);
				return 0;
			}

		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return 0;
		}
		unlock.handle(context);

		cl_program program = new _cl_program(context);
		for(size_t i = 0 ; i < num_devices ; ++i)
			program->devices.push_back(device_list[i]);
		program->build_status = CL_BUILD_SUCCESS;
		program->handle = NULL;

		const unsigned char *ptr = binaries[0];
		size_t offset = 0;
		program->binary_type = *(const cl_program_binary_type*)ptr;	offset += sizeof(cl_program_binary_type);
		const size_t size_of_binary_data = *(const size_t*)(ptr + offset);	offset += sizeof(size_t);
		// Creates a unique temporary file to write the binary data
		size_t n = 0;
		int fd_out = -1;
		std::string filename_out;
		while(fd_out == -1)
		{
			++n;
			if (n > 0x10000)
			{
				delete program;
				SET_RET(CL_OUT_OF_RESOURCES);
				return 0;
			}
			char buf[1024];
			filename_out = tmpnam(buf);
			switch(program->binary_type)
			{
			case CL_PROGRAM_BINARY_TYPE_COMPILED_OBJECT:
				filename_out += ".o";
				break;
			case CL_PROGRAM_BINARY_TYPE_LIBRARY:
				filename_out += ".a";
				break;
			case CL_PROGRAM_BINARY_TYPE_EXECUTABLE:
			default:
				filename_out += ".so";
			}

			fd_out = open(filename_out.c_str(), O_EXCL | O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IXUSR);
		}
		const size_t written_bytes = write(fd_out, ptr + offset, size_of_binary_data);	offset += size_of_binary_data;
		close(fd_out);
		program->binary_file = filename_out;

		const size_t number_of_kernels = *(const size_t*)(ptr + offset);	offset += sizeof(size_t);
		for(size_t i = 0 ; i < number_of_kernels ; ++i)
		{
			const size_t kernel_name_size = *(const size_t*)(ptr + offset);	offset += sizeof(size_t);
			program->kernel_names.insert(std::string((const char*)(ptr + offset), kernel_name_size));	offset += kernel_name_size;
		}

		const size_t build_options_size = *(const size_t*)(ptr + offset);	offset += sizeof(size_t);
		program->build_options = std::string((const char*)(ptr + offset), build_options_size);

		if (program->binary_type == CL_PROGRAM_BINARY_TYPE_EXECUTABLE)
		{
			program->handle = dlopen(program->binary_file.c_str(), RTLD_NOW | RTLD_LOCAL);
			if (!program->handle)
			{
				remove(program->binary_file.c_str());
				delete program;
				SET_RET(CL_INVALID_BINARY);
				return 0;
			}
		}

		SET_RET(CL_SUCCESS);
		return program;
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
		if (program->binary_type != CL_PROGRAM_BINARY_TYPE_NONE)
		{
			const bool b_success = (program->build_status == CL_BUILD_SUCCESS);
			program->unlock();
			return b_success ? CL_SUCCESS : CL_INVALID_BINARY;
		}
		if (program->build_status != CL_BUILD_NONE)
		{
			program->unlock();
			return CL_INVALID_OPERATION;
		}
		program->retain();

		program->build_status = CL_BUILD_IN_PROGRESS;
		const std::string &source_code = program->source_code;

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
															   b_valid_options,
															   false,
															   FreeOCL::map<std::string, std::string>(),
															   &(program->temporary_file));

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
			// In case of error do not delete temporary code file
			program->temporary_file.clear();
			program->build_status = CL_BUILD_ERROR;
			program->unlock();
			clReleaseProgramFCL(program);
			return CL_BUILD_PROGRAM_FAILURE;
		}

		program->handle = dlopen(binary_file.c_str(), RTLD_NOW | RTLD_LOCAL);
		if (!program->handle)
		{
			// In case of error do not delete temporary code file
			program->temporary_file.clear();
			remove(program->binary_file.c_str());
			program->binary_file.clear();
			program->build_status = CL_BUILD_ERROR;
			program->unlock();
			clReleaseProgramFCL(program);
			return CL_BUILD_PROGRAM_FAILURE;
		}

		program->kernel_names = kernel_names;

		for(size_t i = 0 ; i < num_devices ; ++i)
			program->devices.push_back(device_list[i]);

		program->build_options = options ? options : "";
		program->build_status = CL_BUILD_SUCCESS;
		program->binary_type = CL_PROGRAM_BINARY_TYPE_EXECUTABLE;
		program->unlock();
		if (pfn_notify)
			pfn_notify(program, user_data);
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
				struct stat file_stat;
				file_stat.st_size = 0;
				stat(program->binary_file.c_str(), &file_stat);
				const size_t binary_file_size = file_stat.st_size;
				size_t kernel_names_size = 0;
				for(FreeOCL::set<std::string>::const_iterator it = program->kernel_names.begin()
					; it != program->kernel_names.end()
					; ++it)
					kernel_names_size += sizeof(size_t) + it->size();
				const size_t binary_size = sizeof(program->binary_type)
										   + sizeof(size_t) + binary_file_size
										   + sizeof(size_t) + kernel_names_size
										   + sizeof(size_t) + program->build_options.size();
				sizes.resize(program->devices.size(), binary_size);
				bTooSmall = FreeOCL::copy_memory_within_limits(&(sizes.front()),
															   sizes.size() * sizeof(size_t),
															   param_value_size,
															   param_value,
															   param_value_size_ret);
			}
			break;
		case CL_PROGRAM_BINARIES:
			for(size_t i = 0 ; i < program->devices.size() ; ++i)
			{
				char *ptr = ((char**)param_value)[i];
				// Skip device
				if (!ptr)
					continue;
				size_t offset = 0;
				// Write binary type
				*(cl_program_binary_type*)ptr = program->binary_type;	offset += sizeof(cl_program_binary_type);

				// Write binary data
				struct stat file_stat;
				file_stat.st_size = 0;
				stat(program->binary_file.c_str(), &file_stat);
				const size_t binary_file_size = file_stat.st_size;
				*(size_t*)(ptr + offset) = binary_file_size;	offset += sizeof(size_t);

				std::fstream binary_file(program->binary_file.c_str(), std::ios_base::in | std::ios_base::binary);
				binary_file.read(ptr + offset, binary_file_size);	offset += binary_file_size;
				binary_file.close();

				// Write kernel names
				*(size_t*)(ptr + offset) = program->kernel_names.size();	offset += sizeof(size_t);
				for(FreeOCL::set<std::string>::const_iterator it = program->kernel_names.begin()
					; it != program->kernel_names.end()
					; ++it)
				{
					*(size_t*)(ptr + offset) = it->size();	offset += sizeof(size_t);
					memcpy(ptr + offset, it->data(), it->size());	offset += it->size();
				}

				// Write build options
				*(size_t*)(ptr + offset) = program->build_options.size();	offset += sizeof(size_t);
				memcpy(ptr + offset, program->build_options.data(), program->build_options.size());	offset += program->build_options.size();
			}
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
		case CL_PROGRAM_BINARY_TYPE:		bTooSmall = SET_VAR(program->binary_type);	break;
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
					|| !dlsym(main_program, ("__FCL_init_" + *i).c_str())
					|| !dlsym(main_program, ("__FCL_setwg_" + *i).c_str())
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
		if (device_list == NULL && num_devices > 0)
			return CL_INVALID_VALUE;
		if (pfn_notify == NULL && user_data != NULL)
			return CL_INVALID_VALUE;
		if ((num_input_headers == 0 || input_headers == NULL || header_include_names == NULL)
				&& (num_input_headers != 0 || input_headers != NULL || header_include_names != NULL))
			return CL_INVALID_VALUE;

		if (!FreeOCL::is_valid(program))
			return CL_INVALID_PROGRAM;
		if (program->source_code.empty() || program->build_status != CL_BUILD_NONE)
		{
			program->unlock();
			if (pfn_notify)	pfn_notify(program, user_data);
			return CL_INVALID_OPERATION;
		}

		program->build_status = CL_BUILD_IN_PROGRESS;

		if (program->handle)
			dlclose(program->handle);
		if (!program->binary_file.empty())
			remove(program->binary_file.c_str());
		program->handle = NULL;
		program->binary_file.clear();

		const std::string &source_code = program->source_code;
		FreeOCL::map<std::string, std::string> headers;
		for(size_t i = 0 ; i < num_input_headers ; ++i)
		{
			if (input_headers[i] == NULL
					|| header_include_names[i] == NULL
					|| !FreeOCL::is_valid(input_headers[i]))
			{
				program->unlock();
				if (pfn_notify)	pfn_notify(program, user_data);
				return CL_INVALID_VALUE;
			}
			headers[header_include_names[i]] = input_headers[i]->source_code;
			input_headers[i]->unlock();
		}

		std::stringstream build_log;
		bool b_valid_options = true;
		FreeOCL::set<std::string> kernel_names;
		const std::string binary_file = FreeOCL::build_program(options ? options : std::string(),
															   source_code,
															   build_log,
															   kernel_names,
															   b_valid_options,
															   true,
															   headers,
															   &(program->temporary_file));

		if (!b_valid_options)
		{
			program->unlock();
			if (pfn_notify)	pfn_notify(program, user_data);
			return CL_INVALID_BUILD_OPTIONS;
		}

		program->binary_file = binary_file;
		program->build_log = build_log.str();

		if (program->binary_file.empty())
		{
			// Don't delete temporary files in case of errors
			program->temporary_file.clear();
			program->build_status = CL_BUILD_ERROR;
			if (pfn_notify)	pfn_notify(program, user_data);
			return CL_COMPILE_PROGRAM_FAILURE;
		}

		program->kernel_names = kernel_names;

		program->build_status = CL_BUILD_SUCCESS;
		program->binary_type = CL_PROGRAM_BINARY_TYPE_COMPILED_OBJECT;
		program->unlock();
		if (pfn_notify)	pfn_notify(program, user_data);
		return CL_SUCCESS;
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
		if (pfn_notify == NULL && user_data != NULL)
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		if ((num_devices == 0) ^ (device_list == NULL))
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		if (num_input_programs == 0 || input_programs == NULL)
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		if (num_devices > 0 && device_list[0] != FreeOCL::device)
		{
			SET_RET(CL_INVALID_DEVICE);
			return 0;
		}
		FreeOCL::unlocker unlock;
		if (!FreeOCL::is_valid(context))
		{
			SET_RET(CL_INVALID_CONTEXT);
			return 0;
		}
		unlock.handle(context);

		FreeOCL::set<std::string> kernel_names;
		std::vector<std::string> files_to_link;
		files_to_link.reserve(num_input_programs);
		for(size_t i = 0 ; i < num_input_programs ; ++i)
		{
			if (!FreeOCL::is_valid(input_programs[i]))
			{
				SET_RET(CL_INVALID_PROGRAM);
				return 0;
			}
			unlock.handle(input_programs[i]);
			if (input_programs[i]->binary_type == CL_PROGRAM_BINARY_TYPE_NONE
					|| input_programs[i]->binary_type == CL_PROGRAM_BINARY_TYPE_EXECUTABLE)
			{
				SET_RET(CL_INVALID_PROGRAM);
				return 0;
			}
			files_to_link.push_back(input_programs[i]->binary_file);
			kernel_names.insert(input_programs[i]->kernel_names.begin(), input_programs[i]->kernel_names.end());
		}

		std::stringstream log;
		bool b_valid_options = true;
		const std::string &binary_file = FreeOCL::link_program(options ? options : "",
															   files_to_link,
															   log,
															   b_valid_options);
		if (!b_valid_options)
		{
			SET_RET(CL_INVALID_LINKER_OPTIONS);
			return 0;
		}
		if (binary_file.empty())
		{
			SET_RET(CL_LINK_PROGRAM_FAILURE);
			return 0;
		}

		cl_program program = new _cl_program(context);
		program->binary_file = binary_file;
		program->binary_type = (*binary_file.rbegin() == 'a')
							   ? CL_PROGRAM_BINARY_TYPE_LIBRARY
							   : CL_PROGRAM_BINARY_TYPE_EXECUTABLE;
		program->build_options = options ? options : "";
		program->build_log = log.str();
		program->kernel_names.swap(kernel_names);
		program->build_status = CL_BUILD_SUCCESS;

		if (program->binary_type == CL_PROGRAM_BINARY_TYPE_EXECUTABLE)
		{
			program->handle = dlopen(program->binary_file.c_str(), RTLD_NOW | RTLD_LOCAL);
			if (!program->handle)
			{
				delete program;
				SET_RET(CL_LINK_PROGRAM_FAILURE);
				return 0;
			}
		}
		return program;
	}

	CL_API_ENTRY cl_int CL_API_CALL clUnloadPlatformCompilerFCL(cl_platform_id platform) CL_API_SUFFIX__VERSION_1_2
	{
		MSG(clUnloadPlatformCompilerFCL);
		if (platform != FreeOCL::platform)
			return CL_INVALID_PLATFORM;
		return CL_SUCCESS;
	}
}

_cl_program::_cl_program(cl_context context)
	: context_resource(context),
	  binary_type(CL_PROGRAM_BINARY_TYPE_NONE),
	  handle(NULL),
	  build_status(CL_BUILD_NONE)
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
	if (!temporary_file.empty())
		remove(temporary_file.c_str());
}
