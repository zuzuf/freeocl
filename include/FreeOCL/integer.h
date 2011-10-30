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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_INTEGER_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_INTEGER_H__
#include "vectors.h"

// Built-in integer functions

inline long clamp(long, long, long);

// for int
//inline uint abs(int x)	{	return x >= 0 ? x : -x;	}
inline uint abs(const int &x)	{	return x >= 0 ? x : -x;	}
inline uint abs_diff(int x, int y)	{	return x > y ? x - y : y - x;	}
inline int add_sat(int x, int y)
{	return int(clamp(long(x) + long(y), -0x80000000, 0x7FFFFFFF));	}

inline int hadd(int x, int y)
{	return int((long(x) + long(y)) >> 1);	}

inline int rhadd(int x, int y)
{	return int((long(x) + long(y) + 1) >> 1);	}

inline int max(int, int);
inline int min(int, int);
inline int clamp(int x, int minval, int maxval)	{	return min(max(x, minval), maxval);	}
inline int clz(int x)
{
	const uint y = (uint)x;
	int c = ((y >> 16) == 0) << 4;
	c += ((y >> (24 - c)) == 0) << 3;
	c += ((y >> (28 - c)) == 0) << 2;
	c += ((y >> (30 - c)) == 0) << 1;
	c += ((y >> (31 - c)) == 0);
	return c;
}
inline int mul_hi(int, int);
inline int mad_hi(int a, int b, int c)	{	return mul_hi(a, b) + c;	}
inline int mad_sat(int a, int b, int c)
{	return int(clamp(long(a) * long(b) + long(c), (long)-0x80000000, (long)0x7FFFFFFF));	}

inline int max(int x, int y)	{	return x > y ? x : y;	}
inline int min(int x, int y)	{	return x < y ? x : y;	}
inline int mul_hi(int x, int y)	{	return int((long long)(x) * (long long)(y) >> 32);	}
inline int rotate(int v, int i)	{	return (v << i) | (v >> (32 - i));	}

inline int sub_sat(int x, int y)
{	return int(clamp(long(x) - long(y), (long)-0x80000000, (long)0x7FFFFFFF));	}

// for long
inline long max(long, long);
inline long min(long, long);
inline long clamp(long x, long minval, long maxval)	{	return min(max(x, minval), maxval);	}

inline long max(long x, long y)	{	return x > y ? x : y;	}
inline long min(long x, long y)	{	return x < y ? x : y;	}

// upsample functions
inline short upsample(char hi, uchar lo)	{	return ((short)hi << 8) | lo;	}
inline ushort upsample(uchar hi, uchar lo)	{	return ((ushort)hi << 8) | lo;	}
inline int upsample(short hi, ushort lo)	{	return ((int)hi << 16) | lo;	}
inline uint upsample(ushort hi, ushort lo)	{	return ((uint)hi << 16) | lo;	}
inline long upsample(int hi, uint lo)	{	return ((long long)hi << 32) | lo;	}
inline ulong upsample(uint hi, uint lo)	{	return ((long long)hi << 32) | lo;	}

// fast integer built-in functions
inline int mad24(int x, int y, int z)	{	return x * y + z;	}
inline int mul24(int x, int y)	{	return x * y;	}

VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_I(clamp)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_I(abs)

#endif
