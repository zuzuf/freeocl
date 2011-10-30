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
#include <fcntl.h>
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
	std::string build_program(const std::string &options, const std::string &code, std::stringstream &log, FreeOCL::set<std::string> &kernels, bool &b_valid_options)
	{
		b_valid_options = true;

		std::string macros;

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
			}
			else if (word == "-cl-denorms-are-zero")
			{
			}
			else if (word == "-cl-opt-disable")
			{
			}
			else if (word == "-cl-mad-enable")
			{
			}
			else if (word == "-cl-no-signed-zeros")
			{
			}
			else if (word == "-cl-unsafe-math-optimizations")
			{
			}
			else if (word == "-cl-finite-math-only")
			{
			}
			else if (word == "-cl-fast-relaxed-math")
			{
				macros += " -D__FAST_RELAXED_MATH__=1";
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
			filename_out += ".so";
			fd_out = open(filename_out.c_str(), O_EXCL | O_CREAT | O_RDONLY, S_IWUSR | S_IRUSR | S_IXUSR);
		}

		std::stringstream cmd;
		cmd << FREEOCL_CXX_COMPILER
			<< FREEOCL_CXX_FLAGS
			<< " -o " << filename_out
			<< ' ' << filename_in
			<< " 2>&1";			// Redirects everything to stdout in order to read all logs
		int ret = 0;
		log << run_command(cmd.str(), &ret) << std::endl;

		close(fd_out);
		fclose(file_in);
		// Remove the input file which is now useless
//		remove(filename_in.c_str());

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

			gen << "extern \"C\" size_t __FCL_info_" << i->first << "(size_t idx, int *type)" << std::endl
				<< "{" << std::endl
				<< "\tswitch(idx)" << std::endl
				<< "\t{" << std::endl;
			for(size_t j = 0 ; j < params->size() ; ++j)
			{
				const smartptr<chunk> cur = (*params)[j].as<chunk>();
				const smartptr<native_type> native = cur->front().as<native_type>();
				const smartptr<pointer_type> ptr = cur->front().as<pointer_type>();
				const bool b_pointer = ptr;
				const bool b_local = b_pointer && ptr->get_base_type()->get_address_space() == type::LOCAL;
				int type_id = 0;
				if (b_pointer)
				{
					if (b_local)
						type_id = CL_LOCAL;
					else
						type_id = CL_GLOBAL;
				}
				if (native)
				{
					switch(native->get_type_id())
					{
					case native_type::SAMPLER_T:
						type_id = CL_UNORM_INT_101010;
						break;
					case native_type::IMAGE2D_T:
						type_id = CL_MEM_OBJECT_IMAGE2D;
						break;
					case native_type::IMAGE3D_T:
						type_id = CL_MEM_OBJECT_IMAGE3D;
						break;
					}
				}
				gen	<< "\tcase " << j << ":" << std::endl
					<< "\t\t*type = " << type_id << ';' << std::endl
					<< "\t\treturn sizeof(";
				if (b_pointer)	gen << "void*";
				else			gen << *(cur->front());
				gen << ");" << std::endl;
			}
			gen	<< "\t}" << std::endl
				<< "\treturn 0;" << std::endl
				<< "}" << std::endl;

			gen << "extern \"C\" void __FCL_kernel_" << i->first << "(const void *args, size_t dim, size_t *global_offset, size_t *global_size, size_t *local_size)" << std::endl
				<< "{" << std::endl
				<< "\tconst size_t num = local_size[0] * local_size[1] * local_size[2];" << std::endl
#ifdef FREEOCL_USE_OPENMP
				<< "\tomp_set_dynamic(0);" << std::endl
				<< "\tomp_set_num_threads(num);" << std::endl
#endif
				<< "\tFreeOCL::dim = dim;" << std::endl
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
			gen	<< "\tfor(size_t x = 0 ; x < FreeOCL::num_groups[0] ; ++x)" << std::endl
				<< "\tfor(size_t y = 0 ; y < FreeOCL::num_groups[1] ; ++y)" << std::endl
				<< "\tfor(size_t z = 0 ; z < FreeOCL::num_groups[2] ; ++z)" << std::endl
				<< "\t{" << std::endl
				<< "\t\tFreeOCL::group_id[0] = x;" << std::endl
				<< "\t\tFreeOCL::group_id[1] = y;" << std::endl
				<< "\t\tFreeOCL::group_id[2] = z;" << std::endl
				<< "\t\tstatic char local_memory[0x100000];" << std::endl
#ifdef FREEOCL_USE_OPENMP
				<< "#pragma omp parallel" << std::endl
#else
				<< "for(FreeOCL::thread_num = 1 ; FreeOCL::thread_num <= num ; FreeOCL::thread_num++)" << std::endl
#endif
				<< "\t\t{" << std::endl
				<< "\t\t\t" << i->first << "(";
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
					gen << ',' << '\n';
				if (b_local)
					gen << "(" << *p_type << ")(local_memory + __shift" << j << ")";
				else
					gen << "*(" << *p_type << "*)((const char*)args + " << cat.str() << ')';
				cat << " + sizeof(" << *p_type << ")";
			}
			gen << ");" << std::endl;
			gen << "\t\t}" << std::endl
				<< "\t}" << std::endl;
			gen	<< "}" << std::endl;
		}

		log << "converted code:" << std::endl << gen.str() << std::endl;
		return gen.str();
	}
}
