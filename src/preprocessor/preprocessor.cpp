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
#include "preprocessor.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <utils/string.h>
#include <ctime>
#include <deque>

namespace FreeOCL
{
	preprocessor::preprocessor(std::ostream &out, std::ostream &err)
		: out(out), err(err), line(0), b_errors(false)
	{
		time_t t = time(NULL);
		const std::string datetime = ctime(&t);
		macros["__DATE__"].value = '"' + datetime.substr(4, 6) + datetime.substr(19, 5) + '"';
		macros["__DATE__"].params.clear();
		macros["__TIME__"].value = '"' + datetime.substr(11, 8) + '"';
		macros["__TIME__"].params.clear();
	}

	void preprocessor::parse(std::istream &in, const std::string &filename)
	{
		macros["__FILE__"].value = '"' + filename + '"';
		macros["__FILE__"].params.clear();
		current_file = filename;
		line = 1;

		macros["__LINE__"].value = to_string(line);
		macros["__LINE__"].params.clear();

		out << "# 1 \"" << filename << '"' << std::endl;

		this->in.push_back(&in);

		std::vector<int> if_level_state;

		std::string buf;

		for (int v = peek() ; v != -1 ; v = peek())
		{
			const char c = v;
			switch(c)
			{
			case ' ':
			case '\t':
			case '\n':
				buf += char(get());
				break;
			case '\r':
				get();
				break;
			case '#':
				{
					get();
					std::string word = get_word();
					do
					{
						if (word.empty())
						{
							get_line();
						}
						else if (word == "pragma")
						{
							out << c << word << ' ' << get_line();
						}
						else if (word == "include")
						{
							skip_whitespaces();
							const std::string &path = get_path();
							get_line();

							const size_t cur_line = line;

							if (headers.count(path))
							{
								std::stringstream include_data(fix_end_of_lines(headers[path]));
								parse(include_data, path);
							}
							else
							{
								std::fstream include_file;
								for(size_t i = 0 ; i < include_paths.size() && !include_file.is_open() ; ++i)
									include_file.open((include_paths[i] + '/' + path).c_str(), std::ios_base::in);

								if (!include_file.is_open())
									error("could not open file '" + path + '\'');

								include_file.seekg(0, std::ios_base::end);
								const size_t file_size = include_file.tellg();
								include_file.seekg(0);
								std::string file_data;
								file_data.resize(file_size);
								include_file.read(&(file_data[0]), file_size);

								std::stringstream include_data(fix_end_of_lines(file_data));
								parse(include_data, path);
							}

							line = cur_line;
							macros["__LINE__"].value = to_string(line);
							macros["__LINE__"].params.clear();

							current_file = filename;
							macros["__FILE__"].value = '"' + filename + '"';
							macros["__FILE__"].params.clear();

							out << "\n# " << line << " \"" << current_file << "\"\n";
						}
						else if (word == "define")
						{
							skip_whitespaces();
							const std::string &macro_name = get_word();
							if (macro_name == "defined")
								error("'defined' cannot be used as macro name");
							macro m;
							if (peek() == '(')
							{
								get();
								skip_whitespaces();
								while(peek() != ')')
								{
									if (peek() == ',')
									{
										get();
										skip_whitespaces();
									}
									m.params.push_back(get_word());
									if (m.params.back().empty())
										error("expected identifier");
									skip_whitespaces();
									if (peek() != ',' && peek() != ')')
										error("expected ',' or ')'");
								}
								get();
								if (m.params.empty())
									m.params.push_back(std::string());
							}
							else
								skip_whitespaces();
							m.value = get_line();
							macros[macro_name] = m;
						}
						else if (word == "undef")
						{
							skip_whitespaces();
							const std::string &macro_name = get_word();
							macros.erase(macro_name);
							get_line();
						}
						else if (word == "if")
						{
							skip_whitespaces();
							int v = eval(macro_expansion(get_line(), set<std::string>(), true));
							if (!v)
							{
								if_level_state.push_back(0);
								word = skip_bloc();
								continue;
							}
							if_level_state.push_back(1);
							out << "\n# " << line << " \"" << current_file << "\"\n";
						}
						else if (word == "elif")
						{
							if (if_level_state.empty())
								error("#elif without #if");
							if (if_level_state.back() == 2)
								error("#elif after #else");
							if (if_level_state.back())
							{
								word = skip_bloc();
								continue;
							}
							skip_whitespaces();
							int v = eval(macro_expansion(get_line(), set<std::string>(), true));
							if (!v)
							{
								word = skip_bloc();
								continue;
							}
							out << "\n# " << line << " \"" << current_file << "\"\n";
							if_level_state.back() = 1;
						}
						else if (word == "else")
						{
							if (if_level_state.empty())
								error("#else without #if");
							get_line();
							if (if_level_state.back())
							{
								word = skip_bloc();
								continue;
							}
							out << "\n# " << line << " \"" << current_file << "\"\n";
							if_level_state.back() = 2;
						}
						else if (word == "endif")
						{
							if (if_level_state.empty())
								error("#endif without #if");
							get_line();
							if_level_state.pop_back();
							out << "\n# " << line << " \"" << current_file << "\"\n";
						}
						else if (word == "ifdef")
						{
							skip_whitespaces();
							const std::string &identifier = get_word();
							int v = macros.count(identifier) > 0;
							if (!v)
							{
								if_level_state.push_back(0);
								word = skip_bloc();
								continue;
							}
							if_level_state.push_back(1);
							out << "\n# " << line << " \"" << current_file << "\"\n";
						}
						else if (word == "ifndef")
						{
							skip_whitespaces();
							const std::string &identifier = get_word();
							int v = macros.count(identifier) == 0;
							if (!v)
							{
								if_level_state.push_back(0);
								word = skip_bloc();
								continue;
							}
							if_level_state.push_back(1);
							out << "\n# " << line << " \"" << current_file << "\"\n";
						}
						else if (word == "line")
						{
							get_line();
							out << "# " << line << " \"" << current_file << '"';
						}
						else if (word == "error")
						{
							error("#error" + get_line());
						}
						else if (word == "warning")
						{
							warning("#warning" + get_line());
						}
						break;
					} while(true);
				}
				break;
			case '/':
				get();
				switch(peek())
				{
				case '/':		// Remove // comments
					get_line();
					break;
				case '*':		// Remove /* */ comments
					get();
					for(v = get() ; v != -1 && (v != '*' || peek() != '/') ; v = get())	{}
					get();
					break;
				default:
					buf += c;
				}

				break;
			default:
				buf += get_line();
				if (valid_chunk_for_macro_expansion(buf))
				{
					out << macro_expansion(buf);
					buf.clear();
				}
			}
		}

		if (buf.size())
			out << macro_expansion(buf);

		this->in.pop_back();
	}

	std::string preprocessor::skip_bloc()
	{
		int if_level = 0;
		for (int v = peek() ; v != -1 ; v = peek())
		{
			const char c = v;
			switch(c)
			{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				get();
				break;
			case '#':
				{
					get();
					const std::string &word = get_word();
					if (word.empty())
					{
						get_line();
					}
					else if (word == "pragma")
					{
						get_line();
					}
					else if (word == "include")
					{
						get_line();
					}
					else if (word == "define")
					{
						get_line();
					}
					else if (word == "undef")
					{
						get_line();
					}
					else if (word == "if")
					{
						++if_level;
						get_line();
					}
					else if (word == "elif")
					{
						if (if_level == 0)
							return word;
						get_line();
					}
					else if (word == "else")
					{
						if (if_level == 0)
							return word;
						get_line();
					}
					else if (word == "endif")
					{
						if (if_level == 0)
							return word;
						--if_level;
						get_line();
					}
					else if (word == "ifdef")
					{
						++if_level;
						get_line();
					}
					else if (word == "ifndef")
					{
						++if_level;
						get_line();
					}
					else if (word == "line")
					{
						get_line();
					}
					else if (word == "error")
					{
						get_line();
					}
					else if (word == "warning")
					{
						get_line();
					}
				}
				break;
			case '/':
				get();
				switch(peek())
				{
				case '/':		// Remove // comments
					get_line();
					break;
				case '*':		// Remove /* */ comments
					get();
					for(v = get() ; v != -1 && (v != '*' || peek() != '/') ; v = get())	{}
					get();
					break;
				}

				break;
			default:
				get_line();
			}
		}
		return std::string();
	}

	std::string preprocessor::get_line()
	{
		std::string line;
		int v;
		for(v = peek() ; v != -1 && v != '\n' ; v = peek())
		{
			if (v == '/')
			{
				get();
				if (peek() == '/')
				{
					for(v = peek() ; v != 1 && v != '\n' ; v = peek())
						get();
					return line;
				}
				if (peek() == '*')
				{
					get();
					for(v = get() ; v != 1 && (v != '*' || peek() != '/') ; v = get())	{}
					get();
					continue;
				}
				line += char(v);
				continue;
			}
			if (v == '\\')
			{
				get();
				if (peek() == '\n')
				{
					line += '\n';
					get();
				}
				line += char(v);
				continue;
			}
			line += char(get());
		}
		return line;
	}

	std::string preprocessor::get_word()
	{
		std::string word;
		if (isdigit(peek()))
			return word;
		for(int v = peek() ; v != -1 && (isalnum(v) || v == '_') ; v = peek())
			word += char(get());
		return word;
	}

	std::string preprocessor::get_path()
	{
		std::string path;
		char end_type;
		switch(get())
		{
		case '<':
			end_type = '>';
			break;
		case '"':
			end_type = '"';
			break;
		default:
			error("expected '<' or '\"'");
		}

		for(int v = peek() ; v != -1 && v != end_type && v != '\n' ; v = peek())
			path += char(get());
		if (peek() == end_type)
			get();
		else if (peek() == '\n')
			error(std::string("expected '") + end_type + '\'');
		return path;
	}

	void preprocessor::skip_whitespaces()
	{
		while(isblank(peek()))
			get();
	}

	void preprocessor::error(const std::string &msg)	// called on (syntax) errors
	{
#define RED		"\033[31;1m"
#define WHITE	"\033[29;1m"
#define GREY	"\033[30;1m"
#define BLUE	"\033[34;1m"
#define YELLOW	"\033[33;1m"
#define GREEN	"\033[32;1m"
#define NORMAL	"\033[0m"
		const size_t pos = current_line.size();
		while(peek() != -1 && peek() != '\n')	get();
		if (!current_line.empty() && *current_line.rbegin() != '\n')
			current_line += '\n';
		std::stringstream tmp;
		tmp << line;
		const std::string shift = std::string(8 + tmp.tellp() + current_file.size(), ' ');
		err << std::endl
			<< RED << "error " << NORMAL << WHITE << current_file << ":" << line << ": " << NORMAL << current_line
			<< shift << std::string(pos, ' ') << YELLOW << '^' << NORMAL << std::endl
			<< shift << ' ' << BLUE << msg << NORMAL << std::endl;
		b_errors = true;
		throw msg;
	}

	void preprocessor::warning(const std::string &msg)
	{
		const std::string old_line = current_line;
		const size_t pos = current_line.size();
		std::deque<char> chars;
		while(peek() != -1 && peek() != '\n')
			chars.push_back(get());
		while(!chars.empty())
		{
			putback(chars.back());
			chars.pop_back();
		}
		if (!current_line.empty() && *current_line.rbegin() != '\n')
			current_line += '\n';
		std::stringstream tmp;
		tmp << line;
		const std::string shift = std::string(10 + tmp.tellp() + current_file.size(), ' ');
		err << std::endl
				<< BLUE << "warning " << NORMAL << WHITE << current_file << ":" << line << ": " << NORMAL << current_line
				<< shift << std::string(pos, ' ') << YELLOW << '^' << NORMAL << std::endl
				<< shift << ' ' << BLUE << msg << NORMAL << std::endl;
		current_line = old_line;
#undef RED
#undef WHITE
#undef GREY
#undef BLUE
#undef YELLOW
#undef GREEN
#undef NORMAL
	}

	// support functions for parse()
	int preprocessor::get()
	{
		if (in.empty())
			return -1;
		const int ret = in.back()->get();
		if (ret == '\n')
		{
			++line;
			macros["__LINE__"].value = to_string(line);
			macros["__LINE__"].params.clear();
		}
		if (!current_line.empty() && *current_line.rbegin() == '\n')
			current_line.clear();
		if (ret != -1)
			current_line += char(ret);
		return ret;
	}

	int preprocessor::peek()
	{
		if (in.empty())
			return -1;
		return in.back()->peek();
	}

	void preprocessor::putback(char c)
	{
		if (in.empty())
			return;
		in.back()->putback(c);
	}


	void preprocessor::define(const std::string &name, const std::string &value)
	{
		macros[name].value = value;
	}

	void preprocessor::define(const std::string &name, const std::vector<std::string> &params, const std::string &value)
	{
		macros[name].params = params;
		macros[name].value = value;
	}

	void preprocessor::set_include_paths(const std::vector<std::string> &include_paths)
	{
		this->include_paths = include_paths;
	}

	void preprocessor::set_headers(const map<std::string, std::string> &headers)
	{
		this->headers = headers;
	}

	std::string preprocessor::fix_end_of_lines(const std::string &s)
	{
		std::string ret;
		ret.reserve(s.size());
		for(size_t i = 0 ; i < s.size() ; ++i)
		{
			if (s[i] == '\r')
			{
				if (i + 1 < s.size() && s[i + 1] == '\n')
					continue;
				else
					ret += '\n';
			}
			else
				ret += s[i];
		}
		return ret;
	}
}
