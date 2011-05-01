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
					case 'n':	str += '\n';	break;
					case 'r':	str += '\r';	break;
					case 't':	str += '\t';	break;
					default:	str += c;		break;
					};
				}
				else if (c != '\\')
					str += c;
				escape = (c == '\\') && !escape;
			}

			if (!in)                  // end of input
				return 0;
			d_val__ = NULL;
			return STRING;
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
			if (!in || c != '.' || base != 10)
			{
				if (c != '.' || base != 10)
					putback(c);
				d_val__ = NULL;
				return INTEGER;
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
			putback(c);
			d_val__ = NULL;
			return NUMBER;				// return the NUMBER token
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
			d_val__ = NULL;
			return INTEGER;
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
				keywords["while"] = WHILE;
				keywords["for"] = FOR;
				keywords["return"] = RETURN;
				keywords["if"] = IF;
				keywords["else"] = ELSE;
				keywords["switch"] = SWITCH;
				keywords["sizeof"] = SIZEOF;
				keywords["case"] = CASE;
				keywords["default"] = DEFAULT;
				keywords["typedef"] = TYPEDEF;
				keywords["struct"] = STRUCT;
				keywords["union"] = UNION;
			}

			Map<string, int>::type::const_iterator it = keywords.find(name);
			if (it != keywords.end())
				return it->second;
			if (name == "true" || name == "false")
			{
				d_val__ = NULL;
				return BOOLEAN;
			}

			d_val__ = NULL;
			return IDENTIFIER;
		}

		switch(c)
		{
		case '<':
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			if (peek() == '<')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '>':
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			if (peek() == '>')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '=':
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '!':
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '+':
			if (peek() == '+')
			{
				get(c);
				return OPERATOR;
			}
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '-':
			if (peek() == '-')
			{
				get(c);
				return OPERATOR;
			}
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			if (peek() == '>')
			{
				get(c);
				return ARROW;
			}
			break;
		case '*':
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '/':
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '%':
			if (peek() == '=')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '&':
			if (peek() == '&')
			{
				get(c);
				return OPERATOR;
			}
			break;
		case '|':
			if (peek() == '|')
			{
				get(c);
				return OPERATOR;
			}
			break;
		};

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
