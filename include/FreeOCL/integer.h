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

static inline __uchar abs_diff(__char x, __char y)	{	return x > y ? x - y : y - x;	}
static inline __uchar abs_diff(__uchar x, __uchar y)	{	return x > y ? x - y : y - x;	}
static inline __ushort abs_diff(__short x, __short y)	{	return x > y ? x - y : y - x;	}
static inline __ushort abs_diff(__ushort x, __ushort y)	{	return x > y ? x - y : y - x;	}
static inline __uint abs_diff(__int x, __int y)	{	return x > y ? x - y : y - x;	}
static inline __uint abs_diff(__uint x, __uint y)	{	return x > y ? x - y : y - x;	}
static inline __ulong abs_diff(__long x, __long y)	{	return x > y ? x - y : y - x;	}
static inline __ulong abs_diff(__ulong x, __ulong y)	{	return x > y ? x - y : y - x;	}

static inline __uchar __abs(const __char &x)	{	return x >= 0 ? x : -x;	}
static inline __uchar __abs(const __uchar &x)	{	return x >= 0 ? x : -x;	}
static inline __ushort __abs(const __short &x)	{	return x >= 0 ? x : -x;	}
static inline __ushort __abs(const __ushort &x)	{	return x >= 0 ? x : -x;	}
static inline __uint __abs(const __int &x)	{	return x >= 0 ? x : -x;	}
static inline __uint __abs(const __uint &x)	{	return x >= 0 ? x : -x;	}
static inline __ulong __abs(const __long &x)	{	return x >= 0 ? x : -x;	}
static inline __ulong __abs(const __ulong &x)	{	return x >= 0 ? x : -x;	}

#define DEFINE(X, N)						\
static inline __u##X##N __abs(__##X##N x)\
{\
	__u##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
	    ret.v[i] = __abs(x.v[i]);\
	return ret;\
}\
static inline __u##X##N __abs(__u##X##N x)\
{\
	__u##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
	    ret.v[i] = __abs(x.v[i]);\
	return ret;\
}\
static inline __u##X##N abs_diff(__##X##N x, __##X##N y)	\
{\
	__u##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
	    ret.v[i] = abs_diff(x.v[i], y.v[i]); \
	return ret;\
}\
static inline __u##X##N abs_diff(__u##X##N x, __u##X##N y)	\
{\
	__u##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
	    ret.v[i] = abs_diff(x.v[i], y.v[i]); \
	return ret;\
}

#define MAKE_VECTOR(X)\
DEFINE(X, 2)\
DEFINE(X, 3)\
DEFINE(X, 4)\
DEFINE(X, 8)\
DEFINE(X, 16)

MAKE_VECTOR(char)
MAKE_VECTOR(short)
MAKE_VECTOR(int)
MAKE_VECTOR(long)

#undef DEFINE
#undef MAKE_VECTOR

#define abs(x)	__abs(x)

static inline __long clz(__long x)
{
	const __ulong y = (__ulong)x;
	__long c = ((y >> 32) == 0) << 5;
	c += ((y >> (48 - c)) == 0) << 4;
	c += ((y >> (56 - c)) == 0) << 3;
	c += ((y >> (60 - c)) == 0) << 2;
	c += ((y >> (62 - c)) == 0) << 1;
	c += ((y >> (63 - c)) == 0);
	c += (y == 0);
	return c;
}
static inline __ulong clz(__ulong x)
{
	const __ulong y = x;
	__ulong c = ((y >> 32) == 0) << 5;
	c += ((y >> (48 - c)) == 0) << 4;
	c += ((y >> (56 - c)) == 0) << 3;
	c += ((y >> (60 - c)) == 0) << 2;
	c += ((y >> (62 - c)) == 0) << 1;
	c += ((y >> (63 - c)) == 0);
	c += (y == 0);
	return c;
}
static inline __int clz(__int x)
{
	const __uint y = (__uint)x;
	__int c = ((y >> 16) == 0) << 4;
	c += ((y >> (24 - c)) == 0) << 3;
	c += ((y >> (28 - c)) == 0) << 2;
	c += ((y >> (30 - c)) == 0) << 1;
	c += ((y >> (31 - c)) == 0);
	c += (y == 0);
	return c;
}
static inline __uint clz(__uint x)
{
	const __uint y = x;
	__uint c = ((y >> 16) == 0) << 4;
	c += ((y >> (24 - c)) == 0) << 3;
	c += ((y >> (28 - c)) == 0) << 2;
	c += ((y >> (30 - c)) == 0) << 1;
	c += ((y >> (31 - c)) == 0);
	c += (y == 0);
	return c;
}
static inline __short clz(__short x)
{
	const __ushort y = (__ushort)x;
	__short c = ((y >> 8) == 0) << 3;
	c += ((y >> (12 - c)) == 0) << 2;
	c += ((y >> (14 - c)) == 0) << 1;
	c += ((y >> (15 - c)) == 0);
	c += (y == 0);
	return c;
}
static inline __ushort clz(__ushort x)
{
	const __ushort y = x;
	__ushort c = ((y >> 8) == 0) << 3;
	c += ((y >> (12 - c)) == 0) << 2;
	c += ((y >> (14 - c)) == 0) << 1;
	c += ((y >> (15 - c)) == 0);
	c += (y == 0);
	return c;
}
static inline __char clz(__char x)
{
	const __uchar y = (__uchar)x;
	__char c = ((y >> 4) == 0) << 2;
	c += ((y >> (6 - c)) == 0) << 1;
	c += ((y >> (7 - c)) == 0);
	c += (y == 0);
	return c;
}
static inline __uchar clz(__uchar x)
{
	const __uchar y = x;
	__uchar c = ((y >> 4) == 0) << 2;
	c += ((y >> (6 - c)) == 0) << 1;
	c += ((y >> (7 - c)) == 0);
	c += (y == 0);
	return c;
}

static inline __ulong hadd(__ulong x, __ulong y);
static inline __long hadd(__long x, __long y);

static inline __uchar mul_hi(__uchar x, __uchar y)	{	return __uchar((unsigned long long)(x) * (unsigned long long)(y) >> 8);	}
static inline __char mul_hi(__char x, __char y)	{	return __char((long long)(x) * (long long)(y) >> 8);	}
static inline __ushort mul_hi(__ushort x, __ushort y)	{	return __ushort((unsigned long long)(x) * (unsigned long long)(y) >> 16);	}
static inline __short mul_hi(__short x, __short y)	{	return __short((long long)(x) * (long long)(y) >> 16);	}
static inline __uint mul_hi(__uint x, __uint y)	{	return __uint((unsigned long long)(x) * (unsigned long long)(y) >> 32);	}
static inline __int mul_hi(__int x, __int y)	{	return __int((long long)(x) * (long long)(y) >> 32);	}
static inline __ulong mul_hi(__ulong x, __ulong y)
{
	__ulong x_hi = x >> 32;
	__ulong x_lo = x & UINT_MAX;
	__ulong y_hi = y >> 32;
	__ulong y_lo = y & UINT_MAX;

	return ((x_hi * y_hi) + (hadd((x_hi * y_lo), ((x_lo * y_hi) + ((x_lo * y_lo) >> 32))) >> 31));
}
static inline __long mul_hi(__long x, __long y)
{
	__long x_hi = x >> 32;
	__long x_lo = x & UINT_MAX;
	__long y_hi = y >> 32;
	__long y_lo = y & UINT_MAX;

	return ((x_hi * y_hi) + (hadd((x_hi * y_lo), ((x_lo * y_hi) + (__long)((__ulong)(x_lo * y_lo) >> 32))) >> 31));
}

static inline __uchar rotate(__uchar v, __uchar i)	{	return (v << (i % 8) | (v >> (8 - (i % 8))));	}
static inline __char rotate(__char v, __char i)	{ return (__char)rotate(__uchar(v), __uchar(i)); }
static inline __ushort rotate(__ushort v, __ushort i)	{	return (v << (i % 16)) | (v >> (16 - (i % 16)));	}
static inline __short rotate(__short v, __short i)	{ return (__short)rotate(__ushort(v), __ushort(i)); }
#ifdef __arm__
static inline __uint rotate(__uint v, __uint i)	{	return (v << i) | (v >> (32 - i));	}
#else
static inline __uint rotate(__uint v, __uint i)	{	return (v << (i % 32)) | (v >> (32 - (i % 32)));	}
#endif
static inline __int rotate(__int v, __int i)	{ return (__int)rotate(__uint(v), __uint(i)); }
static inline __ulong rotate(__ulong v, __ulong i)	{	return (v << (i % 64)) | (v >> (64 - (i % 64)));	}
static inline __long rotate(__long v, __long i)	{ return (__long)rotate(__ulong(v), __ulong(i)); }

#define DEFINE(TYPE)\
static inline TYPE max(TYPE x, TYPE y)	{	return x > y ? x : y;	}\
static inline TYPE min(TYPE x, TYPE y)	{	return x < y ? x : y;	}\
static inline TYPE mad_hi(TYPE a, TYPE b, TYPE c)	{	return mul_hi(a, b) + c;	}\
static inline TYPE hadd(TYPE x, TYPE y)\
{	return ((x >> 1) + (y >> 1) + (x & y & (TYPE)1));	}\
static inline TYPE rhadd(TYPE x, TYPE y)\
{	return ((x >> 1) + (y >> 1) + ((x & (TYPE)1) | (y & (TYPE)1)));	}

DEFINE(__char)
DEFINE(__uchar)
DEFINE(__short)
DEFINE(__ushort)
DEFINE(__int)
DEFINE(__uint)
DEFINE(__long)
DEFINE(__ulong)
#undef DEFINE

template<class Return, typename Val1, typename Val2> static inline Return clamp(Return x, Val1 minval, Val2 maxval)	{	return min(max(x, (Return)minval), (Return)maxval);	}

#define DEFINE(X, N)\
template<> inline X##N clamp<X##N>(X##N x, X minval, X maxval)\
{\
	X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
	    ret.v[i] = clamp(x.v[i], minval, maxval);\
	return ret;\
}

#define MAKE_VECTOR(X)\
DEFINE(X, 2)\
DEFINE(X, 3)\
DEFINE(X, 4)\
DEFINE(X, 8)\
DEFINE(X, 16)

MAKE_VECTOR(__char)
MAKE_VECTOR(__uchar)
MAKE_VECTOR(__short)
MAKE_VECTOR(__ushort)
MAKE_VECTOR(__int)
MAKE_VECTOR(__uint)
MAKE_VECTOR(__long)
MAKE_VECTOR(__ulong)

#undef DEFINE
#undef MAKE_VECTOR

static inline __char mad_sat(__char a, __char b, __char c)
{	return clamp((__short)a * (__short)b + (__short)c, CHAR_MIN, CHAR_MAX);	}
static inline __uchar mad_sat(__uchar a, __uchar b, __uchar c)
{	return clamp((__ushort)a * (__ushort)b + (__ushort)c, (__ushort)0, UCHAR_MAX);	}
static inline __short mad_sat(__short a, __short b, __short c)
{	return clamp((__int)a * (__int)b + (__int)c, SHRT_MIN, SHRT_MAX);	}
static inline __ushort mad_sat(__ushort a, __ushort b, __ushort c)
{	return clamp((__uint)a * (__uint)b + (__uint)c, (__uint)0, USHRT_MAX);	}
static inline __int mad_sat(__int a, __int b, __int c)
{	return clamp((__long)a * (__long)b + (__long)c, INT_MIN, INT_MAX);	}
static inline __uint mad_sat(__uint a, __uint b, __uint c)
{	return clamp((__ulong)a * (__ulong)b + (__ulong)c, (__ulong)0, UINT_MAX);	}
static inline __long mad_sat(__long a, __long b, __long c)
{	return clamp(a * b + c, LONG_MIN, LONG_MAX);	}
static inline __ulong mad_sat(__ulong a, __ulong b, __ulong c)
{	return clamp(a * b + c, (__ulong)0, ULONG_MAX);	}

static inline __char add_sat(__char x, __char y)
{	return clamp((__short)x + (__short)y, CHAR_MIN, CHAR_MAX);	}
static inline __uchar add_sat(__uchar x, __uchar y)
{	return clamp((__ushort)x + (__ushort)y, (__ushort)0, UCHAR_MAX);	}
static inline __short add_sat(__short x, __short y)
{	return clamp((__int)x + (__int)y, SHRT_MIN, SHRT_MAX);	}
static inline __ushort add_sat(__ushort x, __ushort y)
{	return clamp((__uint)x + (__uint)y, (__uint)0, USHRT_MAX);	}
static inline __int add_sat(__int x, __int y)
{	return clamp((__long)x + (__long)y, INT_MIN, INT_MAX);	}
static inline __uint add_sat(__uint x, __uint y)
{	return clamp((__ulong)x + (__ulong)y, (__ulong)0, UINT_MAX);	}
static inline __long add_sat(__long x, __long y)
{	return clamp(x + y, LONG_MIN, LONG_MAX);	}
static inline __ulong add_sat(__ulong x, __ulong y)
{	return clamp(x + y, (__ulong)0, ULONG_MAX);	}

static inline __char sub_sat(__char x, __char y)
{	return clamp((__short)x - (__short)y, CHAR_MIN, CHAR_MAX);	}
static inline __uchar sub_sat(__uchar x, __uchar y)
    {	return clamp((__short)x - (__short)y, (__short)0, UCHAR_MAX);	}
static inline __short sub_sat(__short x, __short y)
{	return clamp((__int)x - (__int)y, SHRT_MIN, SHRT_MAX);	}
static inline __ushort sub_sat(__ushort x, __ushort y)
{	return clamp((__int)x - (__int)y, (__int)0, USHRT_MAX);	}
static inline __int sub_sat(__int x, __int y)
{	return clamp((__long)x - (__long)y, INT_MIN, INT_MAX);	}
static inline __uint sub_sat(__uint x, __uint y)
{	return clamp((__long)x - (__long)y, (__ulong)0, UINT_MAX);	}
static inline __long sub_sat(__long x, __long y)
{	return clamp(x - y, LONG_MIN, LONG_MAX);	}
static inline __ulong sub_sat(__ulong x, __ulong y)
{	return clamp(x - y, (__long)0, ULONG_MAX);	}

// upsample functions
static inline __short upsample(__char hi, __uchar lo)	{	return ((__short)hi << 8) | lo;	}
static inline __ushort upsample(__uchar hi, __uchar lo)	{	return ((__ushort)hi << 8) | lo;	}
static inline __int upsample(__short hi, __ushort lo)	{	return ((__int)hi << 16) | lo;	}
static inline __uint upsample(__ushort hi, __ushort lo)	{	return ((__uint)hi << 16) | lo;	}
static inline __long upsample(__int hi, __uint lo)	{	return ((long long)hi << 32) | lo;	}
static inline __ulong upsample(__uint hi, __uint lo)	{	return ((long long)hi << 32) | lo;	}

#define DEFINE(X, Y, N)\
static inline __##X##N upsample(__##Y##N hi, __u##Y##N lo)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
	    ret.v[i] = upsample(hi.v[i], lo.v[i]);\
	return ret;\
}\
static inline __u##X##N upsample(__u##Y##N hi, __u##Y##N lo)\
{\
	__u##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
	    ret.v[i] = upsample(hi.v[i], lo.v[i]);\
	return ret;\
}

#define MAKE_VECTOR(X, Y)\
DEFINE(X, Y, 2)\
DEFINE(X, Y, 3)\
DEFINE(X, Y, 4)\
DEFINE(X, Y, 8)\
DEFINE(X, Y, 16)

MAKE_VECTOR(short, char)
MAKE_VECTOR(int, short)
MAKE_VECTOR(long, int)

#undef DEFINE
#undef MAKE_VECTOR

static inline __ulong popcount(__ulong x)
{
#ifdef __GNUG__
	return __builtin_popcount(x);
#else
	const __ulong m1 = 0x5555555555555555;
	const __ulong m2 = 0x3333333333333333;
	const __ulong m4 = 0x0f0f0f0f0f0f0f0f;
	x -= (x >> 1) & m1;
	x = (x & m2) + ((x >> 2) & m2);
	x = (x + (x >> 4)) & m4;
	return (x * h01) >> 56;
#endif
}

// fast integer built-in functions
static inline __int mad24(__int x, __int y, __int z)	{	return x * y + z;	}
static inline __uint mad24(__uint x, __uint y, __uint z)	{	return x * y + z;	}
static inline __int mul24(__int x, __int y)	{	return x * y;	}
static inline __uint mul24(__uint x, __uint y)	{	return x * y;	}

VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__int, mad24)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__uint, mad24)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__int, mul24)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__uint, mul24)

#define DEFINE(F) \
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__long, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__ulong, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__int, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__uint, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__short, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__ushort, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__char, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3_T(__uchar, F)

DEFINE(clamp)
DEFINE(mad_sat)
DEFINE(mad_hi)
#undef DEFINE

VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__int, popcount)

#define DEFINE(F) \
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__long, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__ulong, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__int, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__uint, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__short, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__ushort, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__char, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1_T(__uchar, F)

DEFINE(clz)
#undef DEFINE

#define DEFINE(F) \
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__long, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__ulong, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__int, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__uint, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__short, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__ushort, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__char, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2_T(__uchar, F)

DEFINE(max)
DEFINE(min)
DEFINE(add_sat)
DEFINE(sub_sat)
DEFINE(hadd)
DEFINE(rhadd)
DEFINE(mul_hi)
DEFINE(rotate)
#undef DEFINE

#define DEFINE(F) \
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__long, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__ulong, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__int, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__uint, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__short, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__ushort, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__char, F)\
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2S_T(__uchar, F)

DEFINE(max)
DEFINE(min)
#undef DEFINE

#endif
