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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_H__

#include <cstddef>
#include <x86intrin.h>
#include <omp.h>
#include <math.h>

#define __OPENCL_VERSION__	110
#define CL_VERSION_1_0		100
#define CL_VERSION_1_1		110
// TODO: this should be passed to the compiler as a command line parameter
#define __ENDIAN_LITTLE__	1
#undef __IMAGE_SUPPORT__
//#define __FAST_RELAXED_MATH__

#define MAXFLOAT		0x1.fffffep127f
//#define HUGE_VALF
//#define INFINITY
//#define NAN

#define FLT_DIG			6
#define FLT_MANT_DIG	24
#define FLT_MAX_10_EXP	+38
#define FLT_MAX_EXP		+128
#define FLT_MIN_10_EXP	-37
#define FLT_MIN_EXP		-125
#define FLT_RADIX		2
#define FLT_MAX			0x1.fffffep127f
#define FLT_MIN			0x1.0p-126f
#define FLT_EPSILON		0x1.0p-23f

#define M_E_F			2.7182818284590452354f	/* e */
#define M_LOG2E_F		1.4426950408889634074f	/* log_2 e */
#define M_LOG10E_F		0.43429448190325182765f	/* log_10 e */
#define M_LN2_F			0.69314718055994530942f	/* log_e 2 */
#define M_LN10_F		2.30258509299404568402f	/* log_e 10 */
#define M_PI_F			3.14159265358979323846f	/* pi */
#define M_PI_2_F		1.57079632679489661923f	/* pi/2 */
#define M_PI_4_F		0.78539816339744830962f	/* pi/4 */
#define M_1_PI_F		0.31830988618379067154f	/* 1/pi */
#define M_2_PI_F		0.63661977236758134308f	/* 2/pi */
#define M_2_SQRTPI_F	1.12837916709551257390f	/* 2/sqrt(pi) */
#define M_SQRT2_F		1.41421356237309504880f	/* sqrt(2) */
#define M_SQRT1_2_F		0.70710678118654752440f	/* 1/sqrt(2) */

#define CHAR_BIT	8
#define CHAR_MAX	SCHAR_MAX
#define CHAR_MIN	SCHAR_MIN
#define INT_MAX		2147483647
#define INT_MIN		(-2147483647 – 1)
#define LONG_MAX	0x7fffffffffffffffL
#define LONG_MIN	(-0x7fffffffffffffffL – 1)
#define SCHAR_MAX	127
#define SCHAR_MIN	(-127 – 1)
#define SHRT_MAX	32767
#define SHRT_MIN	(-32767 – 1)
#define UCHAR_MAX	255
#define USHRT_MAX	65535
#define UINT_MAX	0xffffffff
#define ULONG_MAX	0xffffffffffffffffUL

#define __kernel	extern "C"

#define __global
#define __local
#define __constant
#define __private
#define global		__global
#define local		__local
#define constant	__constant
#define private		__private

#define __read_only	const
#define __write_only
#define __read_write
#define read_only	__read_only
#define write_only	__write_only
#define read_write	__read_write

#define FLOAT2	__m64
#define FLOAT4	__m128

// Built-in scalar types
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;
struct half
{
	ushort v;
};

// Built-in vector types
#define DEFINE_VECTOR_TYPE(X, N)\
struct X##N\
{\
	X v[N];\
}

#define DEFINE_VECTORS(X)\
DEFINE_VECTOR_TYPE(X, 2);\
DEFINE_VECTOR_TYPE(X, 3);\
DEFINE_VECTOR_TYPE(X, 4);\
DEFINE_VECTOR_TYPE(X, 8);\
DEFINE_VECTOR_TYPE(X, 16)

DEFINE_VECTORS(char);
DEFINE_VECTORS(short);
DEFINE_VECTORS(int);
DEFINE_VECTORS(long);
DEFINE_VECTORS(uchar);
DEFINE_VECTORS(ushort);
DEFINE_VECTORS(uint);
DEFINE_VECTORS(ulong);
DEFINE_VECTORS(float);
DEFINE_VECTORS(double);

#undef DEFINE_VECTORS
#undef DEFINE_VECTOR_TYPE

#include "workitem.h"
#include "math.h"
#include "integer.h"
#include "common.h"
#include "geometric.h"
#include "relational.h"
#include "vloadstore.h"
#include "sync.h"
#include "memfence.h"
#include "asyncmemop.h"
#include "atomic.h"
#include "vmisc.h"
#include "imgreadwrite.h"

#undef FLOAT2
#undef FLOAT4

#endif
