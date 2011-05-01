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
#include <fcntl.h>
#include <sstream>
#include "freeocl.h"
#include "parser/Parser.h"

namespace FreeOCL
{
	std::string build_program(const std::string &code, std::stringstream &log)
	{
		const std::string preprocessed_code = preprocess_code(code, log);

		if (preprocessed_code.empty())
			return std::string();

		const std::string validated_code = validate_code(preprocessed_code, log);

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
			<< " -ffast-math"
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

	std::string validate_code(const std::string &code, std::stringstream &log)
	{
		log << "code validator log:" << std::endl;
		log << "code:" << std::endl << code << std::endl;
		std::stringstream in(code);
		Parser parser(in, log);
		parser.parse();
		log << "converted code:" << std::endl << parser.getAST().toString() << std::endl;
		return parser.getAST().toString();
	}
}
