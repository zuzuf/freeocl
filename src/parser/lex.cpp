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
#include <utils/map.h>
#include <utils/string.h>
#include <cmath>
#include <sstream>
#include "value.h"
#include "token.h"
#include "symbol_table.h"
#include <stdint.h>
#include "macros.h"
#include <limits>

using namespace std;

namespace
{
	inline bool isdigit(int c, int base)
	{
		switch(base)
		{
		case 10:	return c >= '0' && c <= '9';
		case 2:		return c == '0' || c == '1';
		case 8:		return c >= '0' && c < '8';
		case 16:	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
		default:
			return false;
		}
	}
}

namespace FreeOCL
{
	int parser::lex()
	{
lex_start:
		char c;
		// get the next non-ws character
		while (get(c) && (c == ' ' || c == '\t' || c == '\n'));

		if (!in)					// no characters were obtained
			return _EOF;             // indicate End Of Input

		if (c == '#')		// Preprocessor directive, skip line
		{
			if (!get(c))
				return _EOF;
			if (c == 'p')
			{
				std::string line;
				line += c;
				while(get(c) && c != '\n')	line += c;
				parse_pragma(line);
				goto lex_start;
			}
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
				goto lex_start;
			}
			while(get(c) && c != '\n');
			goto lex_start;
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
			d_val__ = new value<string>('"' + str + '"');
			return STRING_LITERAL;
		}

		long long i = 0;
		int base = 10;
		bool b_integer_parsed = false;
		if (isdigit(c))		// integer or float
		{
			const int pf = peek();
			if (c == '0')
			{
				switch(pf)
				{
				case 'x':
				case 'X':		// Hexadecimal integer
					get();
					base = 16;
					break;
				case 'b':
				case 'B':		// Binary integer
					get();
					base = 2;
					break;
				default:
					if (isdigit(pf))		// Octal integer
						base = 8;
					break;
				}
			}
			else
				i = c - '0';
			while (get(c) && isdigit(c, base))
				i = i * base + (isdigit(c) ? c - '0' : (isupper(c) ? c - 'A' : c - 'a') + 10);
			if (c == 'U' || c == 'u')
			{
				c = peek();
				if (c == 'l' || c == 'L')
				{
					get();
					d_val__ = new value<uint64_t>(i);
				}
				else
					d_val__ = new value<uint32_t>(i);
				return CONSTANT;
			}
			else if (c == 'L' || c == 'l')
			{
				d_val__ = new value<int64_t>(i);
				return CONSTANT;
			}
			else if (!in || (c != '.' && c != 'e' && c != 'E'))
			{
				if (c != '.' || base != 10)
					putback(c);
				if (i <= 0)
				{
					if (i >= std::numeric_limits<int>::min())
						d_val__ = new value<int32_t>(i);
					else
						d_val__ = new value<int64_t>(i);
				}
				else
				{
					if (i <= std::numeric_limits<int>::max())
						d_val__ = new value<int32_t>(i);
					else if (i <= std::numeric_limits<uint32_t>::max())
						d_val__ = new value<uint32_t>(i);
					else if (i <= std::numeric_limits<int64_t>::max())
						d_val__ = new value<int64_t>(i);
					else
						d_val__ = new value<uint64_t>(i);
				}
				return CONSTANT;
			}
			b_integer_parsed = true;
		}
		if ((c == '.' && (isdigit(peek(), base) || (peek() == 'f' && b_integer_parsed))) || (b_integer_parsed && (c == 'e' || c == 'E')))		// a float
		{
			double f = 0.0;
			const double base_div = 1.0 / base;
			if (c == '.')
				for(double p = base_div ; get(c) && isdigit(c, base) ; p *= base_div)
					f += (isdigit(c) ? c - '0' : (isupper(c) ? c - 'A' : c - 'a') + 10) * p;
			f += i;
			if (((c == 'e' || c == 'E') && base <= 10) || ((c == 'p' || c == 'P') && base == 16))
			{
				int sign = 1;
				int exponent = 0;
				if (peek() == '+')	get();
				else if (peek() == '-')
				{
					get();
					sign = -1;
				}
				while (get(c) && isdigit(c))
					exponent = exponent * 10 + c - '0';
				exponent *= sign;
				if (exponent && base == 16)
					f *= exp2(exponent);
				else
					f *= pow(10.0, exponent);
			}
			if (c == 'f' || c == 'F')
				d_val__ = new value<float>((float)f);
			else
			{
				putback(c);
				d_val__ = new value<double>(f);
			}
			return CONSTANT;				// return the CONSTANT token
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
			const int v = c;
			get(c);
			if (c != '\'')
				return 0;
			d_val__ = new value<char>(v);
			return CONSTANT;
		}

		if (isalpha(c) || c == '_')		// an identifier ?
		{
			string name(1, c);
			while (get(c) && (isalnum(c) || c == '_'))
				name += c;
			putback(c);

			static bool b_init_keywords = true;
			static FreeOCL::map<string, int> keywords;
			static FreeOCL::set<string> reserved;
			if (b_init_keywords)
			{
				b_init_keywords = false;
				const char *vreserved[] = { "bool2","bool3","bool4","bool8","bool16",
											"half2","half3","half4","half8","half16",
											"quad", "quad2","quad3","quad4","quad8","quad16",
											"complex", "imaginary"};
				reserved.insert(vreserved, vreserved + sizeof(vreserved) / sizeof(*vreserved));
				for(int i = 1 ; i < 16 ; ++i)
				{
					for(int j = 1 ; j < 16 ; ++j)
					{
						const std::string suffix = FreeOCL::to_string(i) + "x" + FreeOCL::to_string(j);
						reserved.insert("float" + suffix);
						reserved.insert("double" + suffix);
					}
				}
				const char *c99_reserved[] = {"auto", "register", "static",
											  "entry", "extern" };
				reserved.insert(c99_reserved, c99_reserved + sizeof(c99_reserved) / sizeof(*c99_reserved));

				keywords["typedef"] = TYPEDEF;
				keywords["static"] = STATIC;
				keywords["extern"] = EXTERN;
				keywords["sizeof"] = SIZEOF;

				keywords["sampler_t"] = SAMPLER_T;
				keywords["image1d_t"] = IMAGE1D_T;
				keywords["image1d_buffer_t"] = IMAGE1D_BUFFER_T;
				keywords["image1d_array_t"] = IMAGE1D_ARRAY_T;
				keywords["image2d_t"] = IMAGE2D_T;
				keywords["image2d_array_t"] = IMAGE2D_ARRAY_T;
				keywords["image3d_t"] = IMAGE3D_T;
				keywords["event_t"] = EVENT_T;

				keywords["bool"] = BOOL;
				keywords["half"] = HALF;

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
				if (sizeof(void*) == 8)		// 64bits platform
				{
					keywords["ptrdiff_t"] = LONG;
					keywords["intptr_t"] = LONG;
					keywords["uintptr_t"] = ULONG;
				}
				else
				{
					keywords["ptrdiff_t"] = INT;
					keywords["intptr_t"] = INT;
					keywords["uintptr_t"] = UINT;
				}
				keywords["const"] = CONST;
				keywords["__const"] = CONST;
				keywords["volatile"] = VOLATILE;
				keywords["restrict"] = RESTRICT;
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

				keywords["__read_only"] = __READ_ONLY;
				keywords["__write_only"] = __WRITE_ONLY;
				keywords["__read_write"] = __READ_WRITE;
				keywords["read_only"] = __READ_ONLY;
				keywords["write_only"] = __WRITE_ONLY;
				keywords["read_write"] = __READ_WRITE;

				keywords["inline"] = INLINE;
				keywords["__attribute__"] = __ATTRIBUTE__;
			}

			if (reserved.count(name))
				ERROR("'" + name + "' is reserved");

			FreeOCL::map<string, int>::const_iterator it = keywords.find(name);
			if (it != keywords.end())
			{
				d_val__ = new token(name, it->second);
				return it->second;
			}
			if (name == "true" || name == "false")
			{
				d_val__ = new value<int>(name == "true" ? 1 : 0);
				return CONSTANT;
			}

			if ((d_val__ = symbols->get<type>(name)))
				return TYPE_NAME;

			d_val__ = new token(name, IDENTIFIER);
			return IDENTIFIER;
		}

		switch(c)
		{
		case '<':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("<=", LE_OP);
				return LE_OP;
			}
			if (peek() == '<')
			{
				get(c);
				if (peek() == '=')
				{
					get(c);
					d_val__ = new token("<<=", LEFT_ASSIGN);
					return LEFT_ASSIGN;
				}
				d_val__ = new token("<<", LEFT_OP);
				return LEFT_OP;
			}
			break;
		case '>':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token(">=", GE_OP);
				return GE_OP;
			}
			if (peek() == '>')
			{
				get(c);
				if (peek() == '=')
				{
					get(c);
					d_val__ = new token(">>=", RIGHT_ASSIGN);
					return RIGHT_ASSIGN;
				}
				d_val__ = new token(">>", RIGHT_OP);
				return RIGHT_OP;
			}
			break;
		case '=':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("==", EQ_OP);
				return EQ_OP;
			}
			break;
		case '!':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("!=", NE_OP);
				return NE_OP;
			}
			break;
		case '+':
			if (peek() == '+')
			{
				get(c);
				d_val__ = new token("++", INC_OP);
				return INC_OP;
			}
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("+=", ADD_ASSIGN);
				return ADD_ASSIGN;
			}
			break;
		case '-':
			if (peek() == '-')
			{
				get(c);
				d_val__ = new token("--", DEC_OP);
				return DEC_OP;
			}
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("-=", SUB_ASSIGN);
				return SUB_ASSIGN;
			}
			if (peek() == '>')
			{
				get(c);
				d_val__ = new token("->", PTR_OP);
				return PTR_OP;
			}
			break;
		case '*':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("*=", MUL_ASSIGN);
				return MUL_ASSIGN;
			}
			break;
		case '/':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("/=", DIV_ASSIGN);
				return DIV_ASSIGN;
			}
			break;
		case '%':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("%=", MOD_ASSIGN);
				return MOD_ASSIGN;
			}
			break;
		case '&':
			if (peek() == '&')
			{
				get(c);
				d_val__ = new token("&&", AND_OP);
				return AND_OP;
			}
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("&=", AND_ASSIGN);
				return AND_ASSIGN;
			}
			break;
		case '|':
			if (peek() == '|')
			{
				get(c);
				d_val__ = new token("||", OR_OP);
				return OR_OP;
			}
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("|=", OR_ASSIGN);
				return OR_ASSIGN;
			}
			break;
		case '^':
			if (peek() == '=')
			{
				get(c);
				d_val__ = new token("^=" , XOR_ASSIGN);
				return XOR_ASSIGN;
			}
			break;
		};

		d_val__ = new token(std::string(1, c), c);
		return c;                       // otherwise return the extracted char.
	}
}
