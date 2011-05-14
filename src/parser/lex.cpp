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
#include "Parser.h"
#include <map>
#include <cmath>
#include <sstream>

template<class Key, class Value>
struct Map
{
	typedef std::map<Key, Value>	type;
};

using namespace std;

namespace FreeOCL
{
	int Parser::lex()
	{
		char c;
										// get the next non-ws character
		while (get(c) && (c == ' ' || c == '\t' || c == '\n'));

		if (!in)					// no characters were obtained
			return 0;               // indicate End Of Input

		if (c == '#')		// Preprocessor directive, skip line
		{
			if (!get(c))
				return 0;
			if (isspace(c))		// read line number and file name
			{
				std::stringstream line;
				while(get(c) && c != '\n')	line << c;
				line >> this->line >> current_file;
				this->line--;
				if (current_file.size() >= 2)
				{
					if (current_file[0] == '"')
						current_file.erase(0, 1);
					if (*current_file.rbegin() == '"')
						current_file.erase(current_file.size() - 1, 1);
				}
				return lex();
			}
			while(get(c) && c != '\n');
			return lex();
		}

		if (c == '"')
		{
			string str;
			bool escape = false;
			while (get(c) && (c != '"' || escape))
			{
				if (escape)
				{
					switch(c)
					{
					case 'n':	str += "\\n";	break;
					case 'r':	str += "\\r";	break;
					case 't':	str += "\\t";	break;
					default:	str += std::string("\\") + c;		break;
					};
				}
				else if (c != '\\')
					str += c;
				escape = (c == '\\') && !escape;
			}

			if (!in)                  // end of input
				return 0;
			d_val__ = Node('"' + str + '"');
			return STRING_LITERAL;
		}

		int i = 0;
		if (isdigit(c))		// integer or float
		{
			int base = 10;
			if (c == '0' && peek() == 'x')		// Hexadecimal integer
			{
				get();
				base = 16;
			}
			else if (c == '0' && peek() == 'b')		// Binary integer
			{
				get();
				base = 2;
			}
			else if (c == '0' && isdigit(peek()))		// Octal integer
			{
				base = 8;
			}

			i = c - '0';
			while (get(c)
				&& ((isdigit(c) && c - '0' < base)
					|| (((c >= 'a' && c <= 'f')
						 || (c >= 'A' && c <= 'F'))
						&& base == 16)))
				i = i * base + (isdigit(c) ? c - '0' : (isupper(c) ? c - 'A' : c - 'a') + 10);
			if (c == 'U')
			{
				d_val__ = Node(Node(i).toString() + 'U');
				return CONSTANT;
			}
			else if (!in || c != '.' || base != 10)
			{
				if (c != '.' || base != 10)
					putback(c);
				d_val__ = Node(i);
				return CONSTANT;
			}
		}
		if (c == '.' && isdigit(peek()))		// a float
		{
			double f = 0.0;
			for(double p = 0.1 ; get(c) && isdigit(c) ; p *= 0.1)
				f += (c - '0') * p;
			f += i;
			if (c == 'e')
			{
				int sign = 1;
				int exponent = 0;
				if (peek() == '+')	get();
				else if (peek() == '-')
				{
					get();
					sign = -1;
				}
				while(get(c) && isdigit(c))
					exponent = exponent * 10 + (c - '0');
				exponent *= sign;
				if (exponent)
					f *= pow10(exponent);
			}
			if (c == 'f')
				d_val__ = Node((float)f);
			else
			{
				putback(c);
				d_val__ = Node(f);
			}
			return CONSTANT;				// return the NUMBER token
		}

		if (c == '\'')		// A char
		{
			get(c);
			if (c == '\\')
			{
				get(c);
				switch(c)
				{
				case 'n':	c = '\n';	break;
				case 'r':	c = '\r';	break;
				case 't':	c = '\t';	break;
				}
			}
			int v = c;
			get(c);
			if (c != '\'')
				return 0;
			d_val__ = Node(v);
			return CONSTANT;
		}

		if (isalpha(c) || c == '_')		// an identifier ?
		{
			string name(1, c);
			while (get(c) && (isalnum(c) || c == '_'))
				name += c;
			putback(c);

			static bool bInitKeywords = true;
			static Map<string, int>::type keywords;
			if (bInitKeywords)
			{
				bInitKeywords = false;
				keywords["typedef"] = TYPEDEF;

				keywords["char"] = CHAR;
				keywords["short"] = SHORT;
				keywords["int"] = INT;
				keywords["long"] = LONG;
				keywords["uchar"] = UCHAR;
				keywords["ushort"] = USHORT;
				keywords["uint"] = UINT;
				keywords["ulong"] = ULONG;
				keywords["float"] = FLOAT;
				keywords["double"] = DOUBLE;
#define IMPLEMENT_VECTOR(N)\
				keywords["char"#N] = CHAR##N;\
				keywords["short"#N] = SHORT##N;\
				keywords["int"#N] = INT##N;\
				keywords["long"#N] = LONG##N;\
				keywords["uchar"#N] = UCHAR##N;\
				keywords["ushort"#N] = USHORT##N;\
				keywords["uint"#N] = UINT##N;\
				keywords["ulong"#N] = ULONG##N;\
				keywords["float"#N] = FLOAT##N;\
				keywords["double"#N] = DOUBLE##N
				IMPLEMENT_VECTOR(2);
				IMPLEMENT_VECTOR(3);
				IMPLEMENT_VECTOR(4);
				IMPLEMENT_VECTOR(8);
				IMPLEMENT_VECTOR(16);
#undef IMPLEMENT_VECTOR

				keywords["signed"] = SIGNED;
				keywords["unsigned"] = UNSIGNED;
				keywords["size_t"] = SIZE_T;
				keywords["const"] = CONST;
				keywords["volatile"] = VOLATILE;
				keywords["void"] = VOID;
				keywords["struct"] = STRUCT;
				keywords["union"] = UNION;
				keywords["enum"] = ENUM;
				keywords["case"] = CASE;
				keywords["default"] = DEFAULT;
				keywords["if"] = IF;
				keywords["else"] = ELSE;
				keywords["switch"] = SWITCH;
				keywords["while"] = WHILE;
				keywords["do"] = DO;
				keywords["for"] = FOR;
				keywords["goto"] = GOTO;
				keywords["continue"] = CONTINUE;
				keywords["break"] = BREAK;
				keywords["return"] = RETURN;

				keywords["__kernel"] = __KERNEL;
				keywords["__global"] = __GLOBAL;
				keywords["__local"] = __LOCAL;
				keywords["__private"] = __PRIVATE;
				keywords["__constant"] = __CONSTANT;
				keywords["kernel"] = __KERNEL;
				keywords["global"] = __GLOBAL;
				keywords["local"] = __LOCAL;
				keywords["private"] = __PRIVATE;
				keywords["constant"] = __CONSTANT;
			}

			Map<string, int>::type::const_iterator it = keywords.find(name);
			if (it != keywords.end())
			{
				d_val__ = Node(name);
				return it->second;
			}
			if (name == "true" || name == "false")
			{
				d_val__ = Node(name == "true" ? 1 : 0);
				return CONSTANT;
			}

			std::cerr << name << std::endl;
			d_val__ = Node(name);
			return IDENTIFIER;
		}

		switch(c)
		{
		case '<':
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("<=");
				return LE_OP;
			}
			if (peek() == '<')
			{
				get(c);
				d_val__ = Node("<<");
				return LEFT_OP;
			}
			break;
		case '>':
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node(">=");
				return GE_OP;
			}
			if (peek() == '>')
			{
				get(c);
				d_val__ = Node(">>");
				return RIGHT_OP;
			}
			break;
		case '=':
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("==");
				return EQ_OP;
			}
			break;
		case '!':
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("!=");
				return NE_OP;
			}
			break;
		case '+':
			if (peek() == '+')
			{
				get(c);
				d_val__ = Node("++");
				return INC_OP;
			}
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("+=");
				return ADD_ASSIGN;
			}
			break;
		case '-':
			if (peek() == '-')
			{
				get(c);
				d_val__ = Node("--");
				return DEC_OP;
			}
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("-=");
				return SUB_ASSIGN;
			}
			if (peek() == '>')
			{
				get(c);
				d_val__ = Node("->");
				return PTR_OP;
			}
			break;
		case '*':
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("*=");
				return MUL_ASSIGN;
			}
			break;
		case '/':
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("/=");
				return DIV_ASSIGN;
			}
			break;
		case '%':
			if (peek() == '=')
			{
				get(c);
				d_val__ = Node("%=");
				return MOD_ASSIGN;
			}
			break;
		case '&':
			if (peek() == '&')
			{
				get(c);
				d_val__ = Node("&&");
				return AND_OP;
			}
			break;
		case '|':
			if (peek() == '|')
			{
				get(c);
				d_val__ = Node("||");
				return OR_OP;
			}
			break;
		};

		d_val__ = Node(std::string(1, c));
		return c;                       // otherwise return the extracted char.
	}

	int Parser::get()
	{
			const int c = in.get();
			if (c == '\n')
					++line;
			if (!current_line.empty() && *current_line.rbegin() == '\n')
					current_line.clear();
			if (c != -1)
					current_line += char(c);
			return c;
	}

	std::istream &Parser::get(char &c)
	{
			c = 0;
			bool ok = in.get(c);
			if (c == '\n')
					++line;
			if (!current_line.empty() && *current_line.rbegin() == '\n')
					current_line.clear();
			if (ok)
					current_line += c;
			return in;
	}

	void Parser::putback(char c)
	{
			in.putback(c);
			if (c == '\n')
					--line;
			if (!current_line.empty())
					current_line.erase(current_line.size() - 1, 1);
	}

	int Parser::peek()
	{
			return in.peek();
	}
}
