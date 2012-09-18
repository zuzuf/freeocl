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

using namespace std;

namespace FreeOCL
{

	void preprocessor::tokenize(const std::string &s)
	{
		tokens.clear();
		identifiers.clear();

#define get(c)	(_i < s.size() ? ((c = s[_i++]), 1) : (_i++, 0))
#define peek()	(_i < s.size() ? s[_i] : -1)
#define putback(c)	(--_i)

		for(size_t _i = 0 ; _i < s.size() ;)
		{
			char c = '\0';
			// get the next non-ws character
			while (get(c) && (c == ' ' || c == '\t' || c == '\n' || c == '\r'));

			if (_i > s.size())		// no characters were obtained
				continue;

			if (c == '"')
				error("string literals are invalid in preprocessor expressions");

			int i = 0;
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
						++_i;
						base = 16;
						break;
					case 'b':
					case 'B':		// Binary integer
						++_i;
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
					tokens.push_back(make_pair(i, CONSTANT));
					continue;
				}
				else if (_i >= s.size() || (c != '.' && c != 'e' && c != 'E'))
				{
					if (c != '.' || base != 10)
						putback(c);
					tokens.push_back(make_pair(i, CONSTANT));
					continue;
				}
				b_integer_parsed = true;
			}
			if ((c == '.' && (isdigit(peek(), base) || (peek() == 'f' && b_integer_parsed))) || (b_integer_parsed && (c == 'e' || c == 'E')))		// a float
				error("float constant in preprocessor expression");

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
					error("multichar char constant in preprocessor expression");

				tokens.push_back(make_pair(v, CONSTANT));
				continue;
			}

			if (isalpha(c) || c == '_')		// an identifier ?
			{
				string name(1, c);
				while (get(c) && (isalnum(c) || c == '_'))
					name += c;
				putback(c);

				if (name == "true" || name == "false")
				{
					tokens.push_back(make_pair(name == "true" ? 1 : 0, CONSTANT));
					continue;
				}

				if (name == "defined")
				{
					tokens.push_back(make_pair(0, DEFINED));
					continue;
				}

				tokens.push_back(make_pair(identifiers.size(), IDENTIFIER));
				identifiers.push_back(name);
				continue;
			}

			switch(c)
			{
			case '<':
				if (peek() == '=')
				{
					get(c);
					tokens.push_back(make_pair(0, LE_OP));
					continue;
				}
				if (peek() == '<')
				{
					get(c);
					if (peek() == '=')
					{
						get(c);
						error("invalid token in preprocessor expression");
					}
					tokens.push_back(make_pair(0, LEFT_OP));
					continue;
				}
				tokens.push_back(make_pair(0, L_OP));
				continue;

			case '>':
				if (peek() == '=')
				{
					get(c);
					tokens.push_back(make_pair(0, GE_OP));
					continue;
				}
				if (peek() == '>')
				{
					get(c);
					if (peek() == '=')
					{
						get(c);
						error("invalid token in preprocessor expression");
					}
					tokens.push_back(make_pair(0, RIGHT_OP));
					continue;
				}
				tokens.push_back(make_pair(0, G_OP));
				continue;

			case '=':
				if (peek() == '=')
				{
					get(c);
					tokens.push_back(make_pair(0, EQ_OP));
					continue;
				}
				error("invalid token in preprocessor expression");
				break;

			case '~':
				tokens.push_back(make_pair(0, NOT_OP));
				continue;

			case '!':
				if (peek() == '=')
				{
					get(c);
					tokens.push_back(make_pair(0, NE_OP));
					continue;
				}
				tokens.push_back(make_pair(0, LNOT_OP));
				continue;

			case '+':
				if (peek() == '+' || peek() == '=')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, ADD_OP));
				continue;

			case '-':
				if (peek() == '-' || peek() == '=' || peek() == '>')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, SUB_OP));
				continue;

			case '*':
				if (peek() == '=')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, MUL_OP));
				continue;

			case '/':
				if (peek() == '=')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, DIV_OP));
				continue;

			case '%':
				if (peek() == '=')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, MOD_OP));
				continue;

			case '&':
				if (peek() == '&')
				{
					get(c);
					tokens.push_back(make_pair(0, LAND_OP));
					continue;
				}
				if (peek() == '=')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, AND_OP));
				continue;

			case '|':
				if (peek() == '|')
				{
					get(c);
					tokens.push_back(make_pair(0, LOR_OP));
					continue;
				}
				if (peek() == '=')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, OR_OP));
				continue;

			case '^':
				if (peek() == '=')
				{
					get(c);
					error("invalid token in preprocessor expression");
				}
				tokens.push_back(make_pair(0, XOR_OP));
				continue;
			case '(':
				tokens.push_back(make_pair(0, LP));
				continue;
			case ')':
				tokens.push_back(make_pair(0, RP));
				continue;
			case '?':
				tokens.push_back(make_pair(0, TER_0));
				continue;
			case ':':
				tokens.push_back(make_pair(0, TER_1));
				continue;
			};

			error(string("unexpected char in preprocessor expression '") + c + "'");
		}
	}
}
