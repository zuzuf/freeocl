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
#ifndef __FREEOCL_UTILS_STRING_H__
#define __FREEOCL_UTILS_STRING_H__

#include <string>
#include <sstream>
#include <type_traits>

namespace FreeOCL
{
	template<class T>
	inline std::string to_string(const T &v)
	{
		std::stringstream tmp;
		tmp << v;
		return tmp.str();
	}

	template<int N>	struct exp10	{	enum { value = exp10<N - 1>::value * size_t(10) };	};
	template<>
	struct exp10<0>	{	enum { value = 1 };	};

	inline size_t log10_binary_search(size_t v)
	{
		if (v >= exp10<8>::value)
		{
			if (v >= exp10<12>::value)
			{
				if (v >= exp10<16>::value)
				{
					if (v >= exp10<18>::value)
					{
						if (v >= exp10<19>::value)
							return 19;
						return 18;
					}
					if (v >= exp10<17>::value)
						return 17;
					return 16;
				}
				if (v >= exp10<14>::value)
				{
					if (v >= exp10<15>::value)
						return 15;
					return 16;
				}
				if (v >= exp10<13>::value)
					return 13;
				return 12;
			}
			if (v >= exp10<10>::value)
			{
				if (v >= exp10<11>::value)
					return 11;
				return 10;
			}
			if (v >= exp10<9>::value)
				return 9;
			return 8;
		}
		if (v >= exp10<4>::value)
		{
			if (v >= exp10<6>::value)
			{
				if (v >= exp10<7>::value)
					return 7;
				return 6;
			}
			if (v >= exp10<5>::value)
				return 5;
			return 4;
		}
		if (v >= exp10<2>::value)
		{
			if (v >= exp10<3>::value)
				return 3;
			return 2;
		}
		if (v >= exp10<1>::value)
			return 1;
		return 0;
	}

	inline std::string to_string(const size_t &v)
	{
		if (v == 0)
			return std::string(1, '0');
		const size_t log10_v = log10_binary_search(v) + 1;
		char buf[log10_v];
		size_t w = v;
		for(size_t i = 0 ; i < log10_v ; ++i, w /= 10)
			buf[log10_v - i - 1] = '0' + (w % 10);

		return std::string(buf, log10_v);
	}

	inline std::string to_string(const int &v)
	{
		if (v == 0)
			return std::string(1, '0');
		const bool b_neg = v < 0;
		size_t w = b_neg ? -v : v;
		const size_t log10_v = log10_binary_search(w) + (b_neg ? 2 : 1);
		char buf[log10_v];
		const size_t end = b_neg ? log10_v - 1 : log10_v;
		for(size_t i = 0 ; i < end ; ++i, w /= 10)
			buf[log10_v - i - 1] = '0' + (w % 10);
		if (b_neg)
			buf[0] = '-';

		return std::string(buf, log10_v);
	}
}

#endif
