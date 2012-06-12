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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_PRINTF_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_PRINTF_H__

#include <unistd.h>
#include <cstdarg>

#define FLAG_MINUS			0x1		// -
#define FLAG_PLUS			0x2		// +
#define FLAG_ALTERNATIVE	0x4		// #
#define FLAG_0				0x8		// 0

#define LENGTH_L			0b00
#define LENGTH_H			0b01
#define LENGTH_HH			0b11
#define LENGTH_HL			0b10

inline int __flag(int c)
{
	switch(c)
	{
	case '-':	return FLAG_MINUS;
	case '+':	return FLAG_PLUS;
	case '#':	return FLAG_ALTERNATIVE;
	case '0':	return FLAG_0;
	default:
		return 0;
	}
}

inline void __print_char(char c)
{
	write(STDOUT_FILENO, &c, 1);
}

void __print_uint(unsigned long i, int base)
{
	char buf[20];
	int l = 0;
	if (i == 0)
	{
		__print_char('0');
		return;
	}
	if (i < 0)
		i = -i;
	const char digit[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	for(i = (i < 0 ? -i : i) ; i != 0 ; i /= base, ++l)
		buf[l] = digit[i % base];
	for(--l ; l >= 0 ; --l)
		__print_char(buf[l]);
}

void __print_int(long i, int base)
{
	if (i < 0)
	{
		__print_char('-');
		i = -i;
	}
	__print_uint(i, base);
}

void __print_double(double d, int precision)
{
	if (d < 0.0)
	{
		__print_char('-');
		d = -d;
	}
	if (isinf(d))
	{
		__print_char('i');
		__print_char('n');
		__print_char('f');
		return;
	}
	if (isnan(d))
	{
		__print_char('n');
		__print_char('a');
		__print_char('n');
		return;
	}
	precision = min(16, max(0, precision));
	double p = 1.0;
	for(int i = 0 ; i < precision ; ++i)
		p *= 10.0;
	__print_uint(d, 10);
	__print_char('.');
	__print_uint((d - (unsigned long)d) * p, 10);
}

void __print_double_exp(double d, int precision)
{
	if (isinf(d) || isnan(d))
	{
		if (d < 0.0)
		{
			__print_char('-');
			d = -d;
		}
		if (isinf(d))
		{
			__print_char('i');
			__print_char('n');
			__print_char('f');
			return;
		}
		if (isnan(d))
		{
			__print_char('n');
			__print_char('a');
			__print_char('n');
			return;
		}
	}
	if (d == 0.0)
	{
		__print_char('0');
		__print_char('.');
		__print_char('0');
		return;
	}
	const double v = log10(d);
	const int e = v;
	const double f = exp10(v - e);
	__print_double(f, precision);
	__print_char('e');
	__print_int(e, 10);
}

// Built-in printf function
int printf(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	bool b_error = false;
	// Thread safety is not required by OpenCL 1.2 but it would make printf almost useless
#pragma omp critical
	for(int i = 0 ; format[i] != 0 && !b_error ; ++i)
	{
		switch(format[i])
		{
		case '%':	// Directive
			{
				int flags = 0;
				for(++i ; __flag(format[i]) ; ++i)		// Parse flags
					flags |= __flag(format[i]);

				if (format[i] == 0)
				{
					b_error;
					continue;
				}

				int field_width = 0;
				if (format[i] > '0' && format[i] <= '9')
					for( ; format[i] != 0 && format[i] >= '0' && format[i] <= '9' ; ++i)		// Parse field width
						field_width = field_width * 10 + format[i] - '0';

				int precision = -1;
				if (format[i] == '.')
				{
					precision = 0;
					for(++i ; format[i] != 0 && format[i] >= '0' && format[i] <= '9' ; ++i)		// Parse precision
						precision = precision * 10 + format[i] - '0';
				}

				int dimension = 1;
				const char vector_field_separator = ',';
				if (format[i] == 'v')
				{
					dimension = 0;
					for(++i ; format[i] != 0 && format[i] >= '0' && format[i] <= '9' ; ++i)	// Parse vector specifier
						dimension = dimension * 10 + format[i] - '0';
				}

				int length_modifier = -1;
				if (format[i] == 'h' || format[i] == 'l')			// Parse length modifier
				{
					length_modifier = (format[i] == 'h') ? LENGTH_H : LENGTH_L;
					++i;
					if (format[i] == 'h' || format[i] == 'l')
					{
						++i;
						if (length_modifier == LENGTH_L && format[i] == 'l')
						{
							b_error = true;
							continue;
						}
						length_modifier = (length_modifier << 1);
						length_modifier |= (format[i] == 'h') ? LENGTH_H : LENGTH_L;
					}
				}

				if (length_modifier == -1 && dimension > 1)
				{
					b_error = true;
					continue;
				}

#define READ_ARG(T)\
	switch(dimension)\
	{\
		case 2:\
			{\
				typedef T##2 _type;\
				const _type a = va_arg(ap, _type);\
				for(int j = 0 ; j < dimension ; ++j)\
				v[j] = a.v[j];\
			}\
			break;\
		case 3:\
			{\
				typedef T##3 _type;\
				const _type a = va_arg(ap, _type);\
				for(int j = 0 ; j < dimension ; ++j)\
					v[j] = a.v[j];\
			}\
			break;\
		case 4:\
			{\
				typedef T##4 _type;\
				const _type a = va_arg(ap, _type);\
				for(int j = 0 ; j < dimension ; ++j)\
					v[j] = a.v[j];\
			}\
			break;\
		case 8:\
			{\
				typedef T##8 _type;\
				const _type a = va_arg(ap, _type);\
				for(int j = 0 ; j < dimension ; ++j)\
					v[j] = a.v[j];\
			}\
			break;\
		case 16:\
			{\
				typedef T##16 _type;\
				const _type a = va_arg(ap, _type);\
				for(int j = 0 ; j < dimension ; ++j)\
					v[j] = a.v[j];\
			}\
			break;\
		}

				const int convertion_specifier = format[i];
				switch(convertion_specifier)
				{
				case 'd':
				case 'i':
					{
						long v[16];
						if (dimension == 1)
							v[0] = va_arg(ap, long);
						else
						{
							switch(length_modifier)
							{
							case LENGTH_HH:		// char
								READ_ARG(__char)
								break;
							case LENGTH_H:		// short
								READ_ARG(__short)
								break;
							case LENGTH_HL:		// int
								READ_ARG(__int)
								break;
							case LENGTH_L:		// long
								READ_ARG(__long)
								break;
							}
						}
						for(int j = 0 ; j < dimension ; ++j)
						{
							__print_int(v[j], 10);
							if (j + 1 < dimension)
								__print_char(vector_field_separator);
						}
					}
					break;
				case 'o':
				case 'u':
				case 'x':
				case 'X':
					{
						unsigned long v[16];
						if (dimension == 1)
							v[0] = va_arg(ap, unsigned long);
						else
						{
							switch(length_modifier)
							{
							case LENGTH_HH:		// uchar
								READ_ARG(__uchar)
								break;
							case LENGTH_H:		// ushort
								READ_ARG(__ushort)
								break;
							case LENGTH_HL:		// uint
								READ_ARG(__uint)
								break;
							case LENGTH_L:		// ulong
								READ_ARG(__ulong)
								break;
							}
						}
						const int base = convertion_specifier == 'o' ? 8
																	 : convertion_specifier == 'u' ? 10
																								   : 16;
						for(int j = 0 ; j < dimension ; ++j)
						{
							__print_uint(v[j], base);
							if (j + 1 < dimension)
								__print_char(vector_field_separator);
						}
					}
					break;
				case 'f':
				case 'F':
					{
						double v[16];
						if (dimension == 1)
							v[0] = va_arg(ap, double);
						else
						{
							switch(length_modifier)
							{
							case LENGTH_HH:		// 8bit float oO ?
							case LENGTH_H:		// halfn isn't supported either
								b_error = true;
								continue;
							case LENGTH_HL:		// float
								READ_ARG(__float)
								break;
							case LENGTH_L:		// double
								READ_ARG(__double)
								break;
							}
						}
						for(int j = 0 ; j < dimension ; ++j)
						{
							__print_double(v[j], precision);
							if (j + 1 < dimension)
								__print_char(vector_field_separator);
						}
					}
					break;
				case 'g':		// For now g,G,a,A are equivalent to e,E (in order to avoid argument shifts)
				case 'G':
				case 'a':
				case 'A':
				case 'e':
				case 'E':
					{
						double v[16];
						if (dimension == 1)
							v[0] = va_arg(ap, double);
						else
						{
							switch(length_modifier)
							{
							case LENGTH_HH:		// 8bit float oO ?
							case LENGTH_H:		// halfn isn't supported either
								b_error = true;
								continue;
							case LENGTH_HL:		// float
								READ_ARG(__float)
								break;
							case LENGTH_L:		// double
								READ_ARG(__double)
								break;
							}
						}
						for(int j = 0 ; j < dimension ; ++j)
						{
							__print_double_exp(v[j], precision);
							if (j + 1 < dimension)
								__print_char(vector_field_separator);
						}
					}
					break;
				case 'c':
					__print_char(va_arg(ap, char));
					break;
				case 's':
					{
						const char *s = va_arg(ap, const char*);
						for(size_t j = 0 ; s[j] != 0 ; ++j)
							__print_char(s[j]);
					}
					break;
				case 'p':
					__print_char('0');
					__print_char('x');
					__print_uint((__size_t)va_arg(ap, const void*), 16);
					break;
				case '%':
					__print_char('%');
					break;
				default:
					b_error = true;
					continue;
				}
			}
			break;
		default:
			__print_char(format[i]);
		}
	}

	va_end(ap);
	return b_error ? -1 : 0;
}

#endif
