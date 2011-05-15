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
#include "parser/parser.h"
#include "utils/string.h"

namespace FreeOCL
{
	std::string build_program(const std::string &code, std::stringstream &log, std::set<std::string> &kernels)
	{
		const std::string preprocessed_code = preprocess_code(code, log);

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
		(void)fputs("#line 0\n", file_in);
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
			<< ' ' << FREEOCL_OMP_FLAGS
			<< " -shared -fPIC -pipe"// -Wall"
			<< " -m" << (sizeof(void*) * 8)
			<< " -I./include"
			<< " -o " << filename_out
			<< " -g -O0 -march=native -mtune=native"
			<< " -rdynamic"
//			<< " -ffast-math"
			<< " -x c++ " << filename_in
			<< " 2>&1";			// Redirects everything to stdout in order to read all logs
		int ret = 0;
		log << runCommand(cmd.str(), &ret) << std::endl;

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

	std::string preprocess_code(const std::string &code, std::stringstream &log)
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
			<< " -D__OPENCL_VERSION__=110 -DCL_VERSION_1_0=100 -DCL_VERSION_1_1=110"
			<< " -D__ENDIAN_LITTLE__=1"
//			<< " -D__IMAGE_SUPPORT__=1"
//			<< " -D__FAST_RELAXED_MATH__=1"
			<< " -o " << filename_out
			<< " " << filename_in
			<< " 2>&1";			// Redirects everything to stdout in order to read all logs
		int ret = 0;
		log << runCommand(cmd.str(), &ret) << std::endl;

		fclose(file_in);
		// Remove the input file which is now useless
		remove(filename_in.c_str());

		if (ret == 0)
			out = runCommand("cat " + filename_out);
		close(fd_out);
		remove(filename_out.c_str());
		return out;
	}

	std::string validate_code(const std::string &code, std::stringstream &log, std::set<std::string> &kernels)
	{
		log << "code validator log:" << std::endl;
		log << "code:" << std::endl << code << std::endl;
		std::stringstream in(code);
		Parser parser(in, log);
		parser.parse();
		if (parser.errors())
			return std::string();

		static std::set<std::string> types;
		static bool bInit = true;
		if (bInit)
		{
			bInit = false;
			const char *base_types[] = { "char", "uchar", "short", "ushort", "int", "uint", "long", "ulong", "float", "double" };
			for(size_t i = 0 ; i < 10 ; ++i)
				types.insert(std::string(base_types[i]));
			int n[] = { 2, 3, 4, 8, 16 };
			for(size_t j = 0 ; j < 5 ; ++j)
				for(size_t i = 0 ; i < 10 ; ++i)
					types.insert(std::string(base_types[i]) + toString(n[j]));
			types.insert("*");
		}

		std::stringstream gen;
		gen << parser.getAST().toString();

		gen << std::endl;
		for(std::map<std::string, Node>::const_iterator i = parser.getKernels().begin(), end = parser.getKernels().end() ; i != end ; ++i)
		{
			kernels.insert(i->first);

			std::cout << i->second.toString() << std::endl;

			std::deque<Node> params;
			Node n = i->second;
			while(n.size() != 0)
			{
				if (n.size() != 3 || n.getChilds()[1].getValue() != ",")
				{
					params.push_front(n);
					break;
				}
				params.push_front(n.back());
				n = n.front();
			}
			for(size_t j = 0 ; j < params.size() ; ++j)
			{
				Node *cur = &(params[j]);
				while (cur->size() > 0)
				{
					if (cur->back().size() == 0)
					{
						if (types.count(cur->back().getValue()) == 0)
							cur->pop_back();
						break;
					}
					cur = &(cur->back());
				}
			}
			gen << "extern \"C\" size_t __FCL_info_" << i->first << "(size_t idx, int *type)" << std::endl
				<< "{" << std::endl
				<< "\tswitch(idx)" << std::endl
				<< "\t{" << std::endl;
			for(size_t j = 0 ; j < params.size() ; ++j)
			{
				const std::string str = params[j].toString();
				const bool bPointer = !str.empty() && *str.rbegin() == '*';
				const bool bLocal = bPointer && (FreeOCL::containsWord(str, "local") || FreeOCL::containsWord(str, "__local"));
				int type = 0;
				if (bPointer)
				{
					if (bLocal)
						type = CL_LOCAL;
					else
						type = CL_GLOBAL;
				}
				gen	<< "\tcase " << j << ":" << std::endl
					<< "\t\t*type = " << type << ';' << std::endl
					<< "\t\treturn sizeof(" << (bPointer ? "void*" : params[j].toString()) << ");" << std::endl;
			}
			gen	<< "\t}" << std::endl
				<< "\treturn 0;" << std::endl
				<< "}" << std::endl;

			gen << "extern \"C\" void __FCL_kernel_" << i->first << "(const void *args, size_t dim, size_t *global_offset, size_t *global_size, size_t *local_size)" << std::endl
				<< "{" << std::endl
				<< "\tconst size_t num = local_size[0] * local_size[1] * local_size[2];" << std::endl
				<< "\tomp_set_dynamic(0);" << std::endl
				<< "\tomp_set_num_threads(num);" << std::endl
				<< "\tFreeOCL::dim = dim;" << std::endl
				<< "\tfor(size_t i = 0 ; i < 3 ; ++i)" << std::endl
				<< "\t{" << std::endl
				<< "\t\tFreeOCL::global_offset[i] = global_offset[i];" << std::endl
				<< "\t\tFreeOCL::global_size[i] = global_size[i];" << std::endl
				<< "\t\tFreeOCL::local_size[i] = local_size[i];" << std::endl
				<< "\t\tFreeOCL::num_groups[i] = global_size[i] / local_size[i];" << std::endl
				<< "\t}" << std::endl
				<< std::endl;
			int lastShift = -1;
			std::stringstream _cat;
			_cat << '0';
			for(size_t j = 0 ; j < params.size() ; ++j)
			{
				const std::string str = params[j].toString();
				const bool bPointer = !str.empty() && *str.rbegin() == '*';
				const bool bLocal = bPointer && (FreeOCL::containsWord(str, "local") || FreeOCL::containsWord(str, "__local"));
				if (bLocal)
				{
					gen << "\tconst size_t __shift" << j << " = ";
					if (lastShift >= 0)
						gen << "__shift" << lastShift << " - ";
					else
						gen << "0x100000 - ";
					gen << "*(const size_t*)((const char*)args + " << _cat.str() << ");" << std::endl;
					_cat << " + sizeof(" << params[j].toString() << ")";
					lastShift = j;
				}
			}
			gen	<< "\tfor(size_t x = 0 ; x < FreeOCL::num_groups[0] ; ++x)" << std::endl
				<< "\tfor(size_t y = 0 ; y < FreeOCL::num_groups[1] ; ++y)" << std::endl
				<< "\tfor(size_t z = 0 ; z < FreeOCL::num_groups[2] ; ++z)" << std::endl
				<< "\t{" << std::endl
				<< "\t\tFreeOCL::group_id[0] = x;" << std::endl
				<< "\t\tFreeOCL::group_id[1] = y;" << std::endl
				<< "\t\tFreeOCL::group_id[2] = z;" << std::endl
				<< "\t\tstatic char local_memory[0x100000];" << std::endl
				<< "#pragma omp parallel" << std::endl
				<< "\t\t{" << std::endl
				<< "\t\t\t" << i->first << "(";
			std::stringstream cat;
			cat << '0';
			for(size_t j = 0 ; j < params.size() ; ++j)
			{
				const std::string str = params[j].toString();
				const bool bPointer = !str.empty() && *str.rbegin() == '*';
				const bool bLocal = bPointer && (FreeOCL::containsWord(str, "local") || FreeOCL::containsWord(str, "__local"));

				if (j)
					gen << ',';
				if (bLocal)
					gen << "(" << params[j].toString() << ")(local_memory + __shift" << j << ")";
				else
					gen << "*(" << params[j].toString() << "*)((const char*)args + " << cat.str() << ')';
				cat << " + sizeof(" << params[j].toString() << ")";
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
