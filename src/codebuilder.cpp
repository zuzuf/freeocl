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
#include "codebuilder.h"
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "freeocl.h"
#include "device.h"
#include "parser/parser.h"
#include "parser/chunk.h"
#include "parser/token.h"
#include "parser/pointer_type.h"
#include "parser/native_type.h"
#include "utils/string.h"

namespace FreeOCL
{
	std::string build_program(const std::string &options,
							  const std::string &code,
							  std::stringstream &log,
							  FreeOCL::set<std::string> &kernels,
							  bool &b_valid_options,
							  const bool b_compile_only,
							  const FreeOCL::map<std::string, std::string> &headers)
	{
		b_valid_options = true;

		std::string macros;
		std::string compiler_extra_args;
		if (b_compile_only)
			compiler_extra_args += " -c";

		std::stringstream coptions(options);
		while(coptions)
		{
			std::string word;
			coptions >> word;

			if (word.empty() && !coptions)
				break;

			if (word == "-D")		//	macro
			{
				if (!coptions)
				{
					b_valid_options = false;
					return std::string();
				}
				coptions >> word;
				macros += " -D " + word;
			}
			else if (word.size() > 2 && word.substr(0, 2) == "-D")		//	macro
			{
				macros += ' ' + word;
			}
			else if (word == "-I")		//	include path
			{
				if (!coptions)
				{
					b_valid_options = false;
					return std::string();
				}
				coptions >> word;
				macros += " -I " + word;
			}
			else if (word.size() > 2 && word.substr(0, 2) == "-I")		//	include path
			{
				macros += ' ' + word;
			}
			else if (word == "-cl-single-precision-constant")
			{
				compiler_extra_args += " -fsingle-precision-constant";
			}
			else if (word == "-cl-denorms-are-zero")
			{
			}
			else if (word == "-cl-opt-disable")
			{
				compiler_extra_args += " -O 0";
			}
			else if (word == "-cl-mad-enable")
			{
			}
			else if (word == "-cl-no-signed-zeros")
			{
				compiler_extra_args += " -fno-signed-zeros";
			}
			else if (word == "-cl-unsafe-math-optimizations")
			{
				compiler_extra_args += " -funsafe-math-optimizations";
			}
			else if (word == "-cl-finite-math-only")
			{
				compiler_extra_args += " -ffinite-math-only";
			}
			else if (word == "-cl-fast-relaxed-math")
			{
				macros += " -D__FAST_RELAXED_MATH__=1";
				compiler_extra_args += " -ffast-math -D__FAST_RELAXED_MATH__=1";
			}
			else if (word == "-cl-kernel-arg-infos")
			{
			}
			else if (word == "-w")
			{
			}
			else if (word == "-Werror")
			{
			}
			else if (word.size() > 8 && word.substr(0, 8) == "-cl-std=")
			{
			}
			else
			{
				b_valid_options = false;
				return std::string();
			}
		}

		const std::string preprocessed_code = preprocess_code(code, macros, log);

		if (preprocessed_code.empty())
			return std::string();

		const std::string validated_code = validate_code(preprocessed_code, log, kernels);

		if (validated_code.empty())
			return std::string();

		char buf[1024];		// Buffer for tmpnam (to make it thread safe)
		int fd_in = -1;
		int fd_out = -1;
		std::string filename_in;
		std::string filename_out;

		// Open a unique temporary file to write the code
		size_t n = 0;
		while(fd_in == -1)
		{
			++n;
			if (n > 0x10000)
			{
				log << "error: impossible to get a temporary file as compiler input" << std::endl;
				return filename_out;
			}
			filename_in = tmpnam(buf);
			fd_in = open(filename_in.c_str(), O_EXCL | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
		}

		FILE *file_in = fdopen(fd_in, "w");
		(void)fputs("#include <FreeOCL/opencl_c.h>\n", file_in);
		(void)fwrite(validated_code.c_str(), 1, validated_code.size(), file_in);
		(void)fflush(file_in);

		// Creates a unique temporary file to write the binary data
		n = 0;
		while(fd_out == -1)
		{
			++n;
			if (n > 0x10000)
			{
				fclose(file_in);
				remove(filename_in.c_str());
				log << "error: impossible to get a temporary file as compiler output" << std::endl;
				filename_out.clear();
				return filename_out;
			}
			filename_out = tmpnam(buf);
			filename_out += b_compile_only ? ".o" : ".so";
			fd_out = open(filename_out.c_str(), O_EXCL | O_CREAT | O_RDONLY, S_IWUSR | S_IRUSR | S_IXUSR);
		}

		std::stringstream cmd;
		cmd << FREEOCL_CXX_COMPILER
			<< FREEOCL_CXX_FLAGS
			<< compiler_extra_args
			<< " -o " << filename_out
			<< ' ' << filename_in
			<< " 2>&1";			// Redirects everything to stdout in order to read all logs
		int ret = 0;
		log << run_command(cmd.str(), &ret) << std::endl;

		close(fd_out);
		fclose(file_in);
		// Remove the input file which is now useless
		remove(filename_in.c_str());

		if (ret != 0)
		{
			remove(filename_out.c_str());
			filename_out.clear();
		}
		return filename_out;
	}

	std::string preprocess_code(const std::string &code, const std::string &options, std::stringstream &log)
	{
		log << "preprocessor log:" << std::endl;
		char buf[1024];		// Buffer for tmpnam (to make it thread safe)
		int fd_in = -1;
		int fd_out = -1;
		std::string filename_in;
		std::string filename_out;
		std::string out;

		// Open a unique temporary file to write the code
		size_t n = 0;
		while(fd_in == -1)
		{
			++n;
			if (n > 0x10000)
			{
				log << "error: impossible to get a temporary file as preprocessor input" << std::endl;
				return out;
			}
			filename_in = tmpnam(buf);
			fd_in = open(filename_in.c_str(), O_EXCL | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
		}

		FILE *file_in = fdopen(fd_in, "w");
		(void)fwrite(code.c_str(), 1, code.size(), file_in);
		(void)fflush(file_in);

		// Creates a unique temporary file to write the binary data
		n = 0;
		while(fd_out == -1)
		{
			++n;
			if (n > 0x10000)
			{
				fclose(file_in);
				remove(filename_in.c_str());
				log << "error: impossible to get a temporary file as preprocessor output" << std::endl;
				return out;
			}
			filename_out = tmpnam(buf);
			fd_out = open(filename_out.c_str(), O_EXCL | O_CREAT | O_RDWR, S_IWUSR | S_IRUSR | S_IXUSR);
		}

		std::stringstream cmd;
		cmd << "cpp"
			<< " -x c --std=c99"
			<< " -D__OPENCL_VERSION__=110 -DCL_VERSION_1_0=100 -DCL_VERSION_1_1=110";
		if (device->endian_little)
			cmd	<< " -D__ENDIAN_LITTLE__=1";
		cmd	<< " -D__IMAGE_SUPPORT__=1"
			<< ' ' << options
			<< " -o " << filename_out
			<< " " << filename_in
			<< " 2>&1";			// Redirects everything to stdout in order to read all logs
		int ret = 0;
		log << run_command(cmd.str(), &ret) << std::endl;

		fclose(file_in);
		// Remove the input file which is now useless
		remove(filename_in.c_str());

		if (ret == 0)
			out = run_command("cat " + filename_out);
		close(fd_out);
		remove(filename_out.c_str());
		return out;
	}

	std::string validate_code(const std::string &code, std::stringstream &log, FreeOCL::set<std::string> &kernels)
	{
		log << "code validator log:" << std::endl;
		log << "code:" << std::endl << code << std::endl;
		std::stringstream in(code);
		parser p(in, log);
		const u_int64_t timer = usec_timer();
		p.parse();
		std::clog << usec_timer() - timer << "µs" << std::endl;
		if (p.errors())
			return std::string();

		static FreeOCL::set<std::string> types;
		static bool b_init = true;
		if (b_init)
		{
			b_init = false;
			const char *base_types[] = { "bool", "char", "uchar", "short", "ushort", "int", "uint", "long", "ulong", "float", "double" };
			for(size_t i = 0 ; i < 10 ; ++i)
				types.insert(std::string(base_types[i]));
			int n[] = { 2, 3, 4, 8, 16 };
			for(size_t j = 0 ; j < 5 ; ++j)
				for(size_t i = 0 ; i < 10 ; ++i)
					types.insert(std::string(base_types[i]) + to_string(n[j]));
			types.insert("*");
		}

		std::stringstream gen;
		if (!p.get_ast())
			return std::string();
		p.get_ast()->write(gen);

		gen << std::endl;
		for(FreeOCL::map<std::string, smartptr<kernel> >::const_iterator i = p.get_kernels().begin(), end = p.get_kernels().end() ; i != end ; ++i)
		{
			kernels.insert(i->first);

			smartptr<chunk> params = new chunk;
			*params = *(i->second->get_arguments());
			for(size_t j = 0 ; j < params->size() ; ++j)
			{
				smartptr<chunk> cur = (*params)[j].as<chunk>();
				smartptr<type> p_type = cur->front().as<type>();
				if (cur->back().as<chunk>())
				{
					smartptr<chunk> back = cur->back().as<chunk>()->back().as<chunk>();
					if (back)
					{
						const type::address_space addr_space = p_type->get_address_space();
						if (back->size() > 0 && !back->front().as<chunk>())
							back = new chunk(back);
						for(size_t i = 0 ; i < back->size() ; ++i)
						{
							const smartptr<chunk> ch = (*back)[i].as<chunk>();
							if (!ch)
								continue;
							if (ch->front().as<token>() && ch->front().as<token>()->get_id() == '[')
								p_type = new pointer_type(p_type->clone(p_type->is_const(), addr_space), false, type::PRIVATE);
						}
					}
				}
				cur->front() = p_type;
			}

			gen << "extern \"C\" size_t __FCL_info_" << i->first << "(size_t idx, int *type, const char **name, const char **type_name, int *type_qualifier, int *type_access_qualifier)" << std::endl
				<< "{" << std::endl
				<< "\t*name = 0;" << std::endl
				<< "\t*type_name = 0;" << std::endl
				<< "\t*type_qualifier = 0;" << std::endl
				<< "\t*type_access_qualifier = " << CL_KERNEL_ARG_ACCESS_NONE << ';' << std::endl
				<< "\tswitch(idx)" << std::endl
				<< "\t{" << std::endl;
			for(size_t j = 0 ; j < params->size() ; ++j)
			{
				const smartptr<chunk> cur = (*params)[j].as<chunk>();
				const smartptr<native_type> native = cur->front().as<native_type>();
				const smartptr<pointer_type> ptr = cur->front().as<pointer_type>();
				const bool b_pointer = ptr;
				const bool b_local = b_pointer && ptr->get_base_type()->get_address_space() == type::LOCAL;
				const bool b_const = cur->front().as<type>()->is_const();
				const std::string name = cur->back().as<token>()
										 ? cur->back().as<token>()->get_string()
										 : cur->back().as<chunk>()->front().as<token>()->get_string();
				std::string type_name = cur->front().as<type>()->get_name();
				const bool b_restrict = FreeOCL::contains_word(type_name, "restrict");
				const bool b_volatile = FreeOCL::contains_word(b_pointer ? ptr->get_base_type()->get_name() : type_name, "volatile");
				const char *keywords_to_be_removed[] = { "__global", "__local", "__constant", "__private",
														 "global", "local", "constant", "private",
														 "volatile", "restrict", "const",
														 "__read_only", "__read_write", "__write_only",
														 "read_only", "read_write", "write_only", 0 };
				FreeOCL::remove_words(type_name, keywords_to_be_removed);
				int type_id = 0;
				if (b_pointer)
				{
					switch(ptr->get_base_type()->get_address_space())
					{
					case type::LOCAL:
						type_id = CL_KERNEL_ARG_ADDRESS_LOCAL;
						break;
					case type::GLOBAL:
						type_id = CL_KERNEL_ARG_ADDRESS_GLOBAL;
						break;
					case type::CONSTANT:
						type_id = CL_KERNEL_ARG_ADDRESS_CONSTANT;
						break;
					case type::PRIVATE:
						type_id = CL_KERNEL_ARG_ADDRESS_PRIVATE;
						break;
					}
				}
				if (native)
				{
					switch(native->get_type_id())
					{
					case native_type::SAMPLER_T:
						type_id = CL_UNORM_INT_101010;
						break;
					case native_type::IMAGE1D_T:
						type_id = CL_MEM_OBJECT_IMAGE1D;
						break;
					case native_type::IMAGE1D_BUFFER_T:
						type_id = CL_MEM_OBJECT_IMAGE1D_BUFFER;
						break;
					case native_type::IMAGE1D_ARRAY_T:
						type_id = CL_MEM_OBJECT_IMAGE1D_ARRAY;
						break;
					case native_type::IMAGE2D_T:
						type_id = CL_MEM_OBJECT_IMAGE2D;
						break;
					case native_type::IMAGE2D_ARRAY_T:
						type_id = CL_MEM_OBJECT_IMAGE2D_ARRAY;
						break;
					case native_type::IMAGE3D_T:
						type_id = CL_MEM_OBJECT_IMAGE3D;
						break;
					}
				}
				gen	<< "\tcase " << j << ":" << std::endl
					<< "\t\t*type = " << type_id << ';' << std::endl
					<< "\t\t*name = \"" << name << "\";" << std::endl
					<< "\t\t*type_name = \"" << type_name << "\";" << std::endl;
				if (b_const)
					gen << "\t\t*type_qualifier |= " << CL_KERNEL_ARG_TYPE_CONST << ';' << std::endl;
				if (b_restrict)
					gen << "\t\t*type_qualifier |= " << CL_KERNEL_ARG_TYPE_RESTRICT << ';' << std::endl;
				if (b_volatile)
					gen << "\t\t*type_qualifier |= " << CL_KERNEL_ARG_TYPE_VOLATILE << ';' << std::endl;
				gen	<< "\t\treturn sizeof(";
				if (b_pointer)	gen << "void*";
				else			gen << *(cur->front());
				gen << ");" << std::endl;
			}
			gen	<< "\t}" << std::endl
				<< "\treturn 0;" << std::endl
				<< "}" << std::endl;

			gen << "extern \"C\" void __FCL_kernel_" << i->first << "(const void * const args, const size_t dim, const size_t * const global_offset, const size_t * const global_size, const size_t * const local_size)" << std::endl
				<< "{" << std::endl
				<< "\tconst size_t num = local_size[0] * local_size[1] * local_size[2];" << std::endl;
#ifdef FREEOCL_USE_OPENMP
			bool b_needs_sync = false;
			static const char *sync_functions[] = {"barrier", "mem_fence", "read_mem_fence", "write_mem_fence",
												   "wait_group_events", "async_work_group_copy", "async_work_group_strided_copy"};
			for(size_t j = 0 ; j < sizeof(sync_functions) / sizeof(sync_functions[0]) && !b_needs_sync ; ++j)
				b_needs_sync = i->second->has_references_to(sync_functions[j]);
			gen	<< "\tomp_set_dynamic(0);" << std::endl
				<< (b_needs_sync ?
					"\tomp_set_num_threads(num);"
				  : "\tomp_set_num_threads(omp_get_num_procs());") << std::endl;
#endif
			gen	<< "\tFreeOCL::dim = dim;" << std::endl
				<< "\tfor(size_t i = 0 ; i < 3 ; ++i)" << std::endl
				<< "\t{" << std::endl
				<< "\t\tFreeOCL::global_offset[i] = global_offset[i];" << std::endl
				<< "\t\tFreeOCL::global_size[i] = global_size[i];" << std::endl
				<< "\t\tFreeOCL::local_size[i] = local_size[i];" << std::endl
				<< "\t\tFreeOCL::num_groups[i] = global_size[i] / local_size[i];" << std::endl
				<< "\t}" << std::endl
				<< std::endl;
			int last_shift = -1;
			std::stringstream _cat;
			_cat << '0';
			for(size_t j = 0 ; j < params->size() ; ++j)
			{
				const smartptr<chunk> cur = (*params)[j].as<chunk>();
				const smartptr<type> p_type = cur->front().as<type>();
				const smartptr<pointer_type> ptr = p_type.as<pointer_type>();
				const bool b_pointer = ptr;
				const bool b_local = b_pointer && ptr->get_base_type()->get_address_space() == type::LOCAL;
				if (b_local)
				{
					gen << "\tconst size_t __shift" << j << " = ";
					if (last_shift >= 0)
						gen << "__shift" << last_shift << " - ";
					else
						gen << "0x100000 - ";
					gen << "*(const size_t*)((const char*)args + " << _cat.str() << ");" << std::endl;
					_cat << " + sizeof(size_t)";
					last_shift = j;
				}
				else
					_cat << " + sizeof(" << *(p_type) << ")";
			}
			bool b_has_local_parameters = false;
			for(size_t j = 0 ; j < params->size() && !b_has_local_parameters ; ++j)
			{
				const smartptr<chunk> cur = (*params)[j].as<chunk>();
				const smartptr<type> p_type = cur->front().as<type>();
				const smartptr<pointer_type> ptr = p_type.as<pointer_type>();
				const bool b_pointer = ptr;
				const bool b_local = b_pointer && ptr->get_base_type()->get_address_space() == type::LOCAL;
				b_has_local_parameters |= b_local;
			}
			if (b_has_local_parameters)
				gen << "\tstatic char local_memory[0x100000];" << std::endl;
#ifdef FREEOCL_USE_OPENMP
			gen << "#pragma omp parallel" << std::endl
				<< "\t{" << std::endl;
#endif
			gen	<< "\tfor(size_t x = 0 ; x < FreeOCL::num_groups[0] ; ++x)" << std::endl
				<< "\t{" << std::endl
				<< "\t\tFreeOCL::group_id[0] = x;" << std::endl
				<< "\t\tfor(size_t y = 0 ; y < FreeOCL::num_groups[1] ; ++y)" << std::endl
				<< "\t\t{" << std::endl
				<< "\t\t\tFreeOCL::group_id[1] = y;" << std::endl
				<< "\t\t\tfor(size_t z = 0 ; z < FreeOCL::num_groups[2] ; ++z)" << std::endl
				<< "\t\t\t{" << std::endl
				<< "\t\t\t\tFreeOCL::group_id[2] = z;" << std::endl;
#ifdef FREEOCL_USE_OPENMP
			gen	<< "#pragma omp for nowait" << std::endl
				<< "\t\t\t\tfor(size_t thread_id = 0 ; thread_id < num ; thread_id++)" << std::endl
#else
			gen	<< "\t\t\t\tfor(FreeOCL::thread_num = 0 ; FreeOCL::thread_num < num ; FreeOCL::thread_num++)" << std::endl
#endif
				<< "\t\t\t\t{" << std::endl
#ifdef FREEOCL_USE_OPENMP
				<< "\t\t\t\t\tFreeOCL::thread_num = thread_id;" << std::endl
#endif
				<< "\t\t\t\t\t" << i->first << "(";
			std::stringstream cat;
			cat << '0';
			for(size_t j = 0 ; j < params->size() ; ++j)
			{
				const smartptr<chunk> cur = (*params)[j].as<chunk>();
				const smartptr<type> p_type = cur->front().as<type>();
				const smartptr<pointer_type> ptr = p_type.as<pointer_type>();
				const bool b_pointer = ptr;
				const bool b_local = b_pointer && ptr->get_base_type()->get_address_space() == type::LOCAL;

				if (j)
					gen << ",\n\t\t\t\t\t\t";
				if (b_local)
					gen << "(" << *p_type << ")(local_memory + __shift" << j << ")";
				else
					gen << "*(" << *p_type << "*)((const char*)args + " << cat.str() << ')';
				cat << " + sizeof(" << *p_type << ")";
			}
			gen << ");" << std::endl;
			gen << "\t\t\t\t}" << std::endl
				<< "\t\t\t}" << std::endl
				<< "\t\t}" << std::endl
				<< "\t}" << std::endl;
#ifdef FREEOCL_USE_OPENMP
			gen << "\t}" << std::endl;
#endif
			gen	<< "}" << std::endl;
		}

		log << "converted code:" << std::endl << gen.str() << std::endl;
		return gen.str();
	}

	std::string link_program(const std::string &options,
							 const std::vector<std::string> &files_to_link,
							  std::stringstream &log,
							  bool &b_valid_options)
	{
		b_valid_options = true;

		std::string compiler_extra_args;
		std::stringstream coptions(options);
		bool b_library = false;
		while(coptions)
		{
			std::string word;
			coptions >> word;

			if (word.empty() && !coptions)
				break;

			if (word == "-create-library")
			{
				b_library = true;
			}
			else if (word == "-enable-link-options")
			{
			}
			else if (word == "-cl-denorms-are-zero")
			{
			}
			else if (word == "-cl-no-signed-zeros")
			{
				compiler_extra_args += " -fno-signed-zeros";
			}
			else if (word == "-cl-unsafe-math-optimizations")
			{
				compiler_extra_args += " -funsafe-math-optimizations";
			}
			else if (word == "-cl-finite-math-only")
			{
				compiler_extra_args += " -ffinite-math-only";
			}
			else if (word == "-cl-fast-relaxed-math")
			{
				compiler_extra_args += " -ffast-math -D__FAST_RELAXED_MATH__=1";
			}
			else
			{
				b_valid_options = false;
				return std::string();
			}
		}

		char buf[1024];		// Buffer for tmpnam (to make it thread safe)
		int fd_out = -1;
		std::string filename_out;

		size_t n = 0;
		while(fd_out == -1)
		{
			++n;
			if (n > 0x10000)
			{
				log << "error: impossible to get a temporary file as linker output" << std::endl;
				filename_out.clear();
				return filename_out;
			}
			filename_out = tmpnam(buf);
			filename_out += b_library ? ".a" : ".so";
			fd_out = open(filename_out.c_str(), O_EXCL | O_CREAT | O_RDONLY, S_IWUSR | S_IRUSR | S_IXUSR);
		}

		std::stringstream cmd;
		if (b_library)	// Static library (CL_PROGRAM_BINARY_TYPE_LIBRARY)
		{
			cmd << "ar rcs "
				<< filename_out;
		}
		else			// Shared library (CL_PROGRAM_BINARY_TYPE_EXECUTABLE)
		{
			cmd << FREEOCL_CXX_COMPILER
				<< FREEOCL_CXX_FLAGS
				<< compiler_extra_args
				<< " -o " << filename_out;
		}
		for(size_t i = 0 ; i < files_to_link.size() ; ++i)
			cmd	<< ' ' << files_to_link[i];
		cmd	<< " 2>&1";			// Redirects everything to stdout in order to read all logs
		int ret = 0;
		log << run_command(cmd.str(), &ret) << std::endl;

		close(fd_out);

		if (ret != 0)
		{
			remove(filename_out.c_str());
			filename_out.clear();
		}
		return filename_out;
	}
}
