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
#include "parser.h"
#include <sstream>

using namespace std;

#define RED		"\033[31;1m"
#define WHITE	"\033[29;1m"
#define GREY	"\033[30;1m"
#define BLUE	"\033[34;1m"
#define YELLOW	"\033[33;1m"
#define GREEN	"\033[32;1m"
#define NORMAL	"\033[0m"

namespace FreeOCL
{
	void Parser::error(const std::string &msg)
	{
		const size_t pos = current_line.size();
		char c;
		while(get(c) && c != '\n');
		if (!current_line.empty() && *current_line.rbegin() != '\n')
			current_line += '\n';
		stringstream tmp;
		tmp << line;
		const string shift = std::string(8 + tmp.tellp() + current_file.size(), ' ');
		err << std::endl
				<< RED << "error " << NORMAL << WHITE << current_file << ":" << line << ": " << NORMAL << current_line
				<< shift << std::string(pos, ' ') << YELLOW << '^' << NORMAL << std::endl
				<< shift << ' ' << BLUE << msg << NORMAL << std::endl;
		bErrors = true;
	}

	void Parser::warning(const std::string &msg)
	{
		const std::string old_line = current_line;
		const size_t pos = current_line.size();
		std::deque<char> chars;
		char c;
		while(get(c) && c != '\n')
			chars.push_back(c);
		putback(c);
		while(!chars.empty())
		{
			putback(chars.back());
			chars.pop_back();
		}
		if (!current_line.empty() && *current_line.rbegin() != '\n')
			current_line += '\n';
		stringstream tmp;
		tmp << line;
		const string shift = std::string(10 + tmp.tellp() + current_file.size(), ' ');
		err << std::endl
				<< BLUE << "warning " << NORMAL << WHITE << current_file << ":" << line << ": " << NORMAL << current_line
				<< shift << std::string(pos, ' ') << YELLOW << '^' << NORMAL << std::endl
				<< shift << ' ' << BLUE << msg << NORMAL << std::endl;
		current_line = old_line;
	}
}
