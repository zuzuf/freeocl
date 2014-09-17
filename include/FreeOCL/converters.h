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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_CONVERTERS_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_CONVERTERS_H__

#include <fenv.h>

template<class T> struct __type_range;
template<> struct __type_range<__char>		{	enum { __min = -128, __max = 127 };	};
template<> struct __type_range<__uchar>		{	enum { __min = 0, __max = 255 };	};
template<> struct __type_range<__short>		{	enum { __min = -32768, __max = 32767 };	};
template<> struct __type_range<__ushort>	{	enum { __min = 0, __max = 65536 };	};
template<> struct __type_range<__int>		{	enum { __min = -0x10000000, __max = 0x7FFFFFFF };	};
template<> struct __type_range<__uint>		{	enum { __min = 0, __max = 0xFFFFFFFFU };	};
template<> struct __type_range<__long>		{	enum { __min = -0x1000000000000000L, __max = 0x7FFFFFFFFFFFFFFFL };	};
template<> struct __type_range<__ulong>		{	enum { __min = 0, __max = 0xFFFFFFFFFFFFFFFFLU };	};

// Built-in type conversion functions
#define CONVERTER(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X(const Scalar &v)	{	return (__##X)v;	}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value, __##X>::type &convert_##X(const Scalar &v)	{	return v;	}

#define CONVERTER_SAT(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_sat(const Scalar &v)\
{\
	return (__##X)(v > __type_range<__##X>::__max\
			   ? __type_range<__##X>::__max\
			   : v < __type_range<__##X>::__min\
				 ? __type_range<__##X>::__min\
				 : v);\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_sat(const Scalar &v)	{	return v;	}

#define CONVERTER_RTZ(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_rtz(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_TOWARDZERO);\
	__##X result = __##X(v);\
	fesetround(rmode);\
	return result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_rtz(const Scalar &v)	{	return v;	}

#define CONVERTER_SAT_RTZ(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_sat_rtz(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_TOWARDZERO);\
	__##X result = convert_##X##_sat(v);\
	fesetround(rmode);\
	return result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_sat_rtz(const Scalar &v)	{	return v;	}

#define CONVERTER_RTP(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_rtp(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_UPWARD);\
	__##X result = rint(v);\
	fesetround(rmode);\
	return (__##X)result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_rtp(const Scalar &v)	{	return v;	}

#define CONVERTER_SAT_RTP(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_sat_rtp(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_UPWARD);\
	__##X result = convert_##X##_sat(rint(v));\
	fesetround(rmode);\
	return result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_sat_rtp(const Scalar &v)	{	return v;	}

#define CONVERTER_RTN(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_rtn(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_DOWNWARD);\
	__##X result = rint(v);\
	fesetround(rmode);\
	return (__##X)result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_rtn(const Scalar &v)	{	return v;	}

#define CONVERTER_SAT_RTN(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_sat_rtn(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_DOWNWARD);\
	__##X result = convert_##X##_sat(rint(v));\
	fesetround(rmode);\
	return result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_sat_rtn(const Scalar &v)	{	return v;	}

#define CONVERTER_RTE(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_rte(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_TONEAREST);\
	__##X result = rint(v);\
	fesetround(rmode);\
	return (__##X)result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_rte(const Scalar &v)	{	return v;	}

#define CONVERTER_SAT_RTE(X)\
template<class Scalar>\
static inline typename __if< !__match<typename __scalar<Scalar>::type, __##X>::value, __##X>::type convert_##X##_sat_rte(const Scalar &v)\
{\
	int rmode = fegetround();\
	fesetround(FE_TONEAREST);\
	__##X result = convert_##X##_sat(v);\
	fesetround(rmode);\
	return result;\
}\
template<class Scalar>\
static inline const typename __if< __match<Scalar, __##X>::value , __##X>::type &convert_##X##_sat_rte(const Scalar &v)	{	return v;	}

CONVERTER(char)
CONVERTER(uchar)
CONVERTER(short)
CONVERTER(ushort)
CONVERTER(int)
CONVERTER(uint)
CONVERTER(long)
CONVERTER(ulong)
CONVERTER(float)
CONVERTER(double)

CONVERTER_SAT(char)
CONVERTER_SAT(uchar)
CONVERTER_SAT(short)
CONVERTER_SAT(ushort)
CONVERTER_SAT(int)
CONVERTER_SAT(uint)
CONVERTER_SAT(long)
CONVERTER_SAT(ulong)

CONVERTER_RTZ(char)
CONVERTER_RTZ(uchar)
CONVERTER_RTZ(short)
CONVERTER_RTZ(ushort)
CONVERTER_RTZ(int)
CONVERTER_RTZ(uint)
CONVERTER_RTZ(long)
CONVERTER_RTZ(ulong)
CONVERTER_RTZ(float)
CONVERTER_RTZ(double)

CONVERTER_RTP(char)
CONVERTER_RTP(uchar)
CONVERTER_RTP(short)
CONVERTER_RTP(ushort)
CONVERTER_RTP(int)
CONVERTER_RTP(uint)
CONVERTER_RTP(long)
CONVERTER_RTP(ulong)
CONVERTER_RTP(float)
CONVERTER_RTP(double)

CONVERTER_RTN(char)
CONVERTER_RTN(uchar)
CONVERTER_RTN(short)
CONVERTER_RTN(ushort)
CONVERTER_RTN(int)
CONVERTER_RTN(uint)
CONVERTER_RTN(long)
CONVERTER_RTN(ulong)
CONVERTER_RTN(float)
CONVERTER_RTN(double)

CONVERTER_RTE(char)
CONVERTER_RTE(uchar)
CONVERTER_RTE(short)
CONVERTER_RTE(ushort)
CONVERTER_RTE(int)
CONVERTER_RTE(uint)
CONVERTER_RTE(long)
CONVERTER_RTE(ulong)
CONVERTER_RTE(float)
CONVERTER_RTE(double)

CONVERTER_SAT_RTZ(char)
CONVERTER_SAT_RTZ(uchar)
CONVERTER_SAT_RTZ(short)
CONVERTER_SAT_RTZ(ushort)
CONVERTER_SAT_RTZ(int)
CONVERTER_SAT_RTZ(uint)
CONVERTER_SAT_RTZ(long)
CONVERTER_SAT_RTZ(ulong)
CONVERTER_SAT_RTZ(float)
CONVERTER_SAT_RTZ(double)

CONVERTER_SAT_RTP(char)
CONVERTER_SAT_RTP(uchar)
CONVERTER_SAT_RTP(short)
CONVERTER_SAT_RTP(ushort)
CONVERTER_SAT_RTP(int)
CONVERTER_SAT_RTP(uint)
CONVERTER_SAT_RTP(long)
CONVERTER_SAT_RTP(ulong)
CONVERTER_SAT_RTP(float)
CONVERTER_SAT_RTP(double)

CONVERTER_SAT_RTN(char)
CONVERTER_SAT_RTN(uchar)
CONVERTER_SAT_RTN(short)
CONVERTER_SAT_RTN(ushort)
CONVERTER_SAT_RTN(int)
CONVERTER_SAT_RTN(uint)
CONVERTER_SAT_RTN(long)
CONVERTER_SAT_RTN(ulong)
CONVERTER_SAT_RTN(float)
CONVERTER_SAT_RTN(double)

CONVERTER_SAT_RTE(char)
CONVERTER_SAT_RTE(uchar)
CONVERTER_SAT_RTE(short)
CONVERTER_SAT_RTE(ushort)
CONVERTER_SAT_RTE(int)
CONVERTER_SAT_RTE(uint)
CONVERTER_SAT_RTE(long)
CONVERTER_SAT_RTE(ulong)
CONVERTER_SAT_RTE(float)
CONVERTER_SAT_RTE(double)

#undef CONVERTER
#undef CONVERTER_SAT
#undef CONVERTER_RTZ
#undef CONVERTER_RTP
#undef CONVERTER_RTN
#undef CONVERTER_RTE
#undef CONVERTER_SAT_RTZ
#undef CONVERTER_SAT_RTP
#undef CONVERTER_SAT_RTN
#undef CONVERTER_SAT_RTE

#define CONVERTER(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = (__##X)v.v[i];\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N(const V &v)	{	return v;	}
#define CONVERTER_SAT(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_sat(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_sat(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_sat(const V &v)	{	return v;	}

#define CONVERTER_RTZ(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_rtz(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_rtz(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_rtz(const V &v)	{	return v;	}

#define CONVERTER_SAT_RTZ(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_sat_rtz(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_sat_rtz(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_sat_rtz(const V &v)	{	return v;	}

#define CONVERTER_RTP(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_rtp(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_rtp(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_rtp(const V &v)	{	return v;	}

#define CONVERTER_SAT_RTP(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_sat_rtp(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_sat_rtp(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_sat_rtp(const V &v)	{	return v;	}

#define CONVERTER_RTN(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_rtn(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_rtn(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_rtn(const V &v)	{	return v;	}

#define CONVERTER_SAT_RTN(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_sat_rtn(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_sat_rtn(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_sat_rtn(const V &v)	{	return v;	}

#define CONVERTER_RTE(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_rte(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_rte(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_rte(const V &v)	{	return v;	}

#define CONVERTER_SAT_RTE(X, N)\
template<class V>\
	static inline typename __if<__vector<V>::components == N && !__match<__##X##N, V>::value, __##X##N>::type convert_##X##N##_sat_rte(const V &v)\
{\
	__##X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_sat_rte(v.v[i]);\
	return ret;\
}\
template<class V>\
	static inline const typename __if<__match<__##X##N, V>::value, __##X##N>::type &convert_##X##N##_sat_rte(const V &v)	{	return v;	}

#define CONVERTER_VECTORS(X)\
	CONVERTER(X, 2)\
	CONVERTER(X, 3)\
	CONVERTER(X, 4)\
	CONVERTER(X, 8)\
	CONVERTER(X, 16)
#define CONVERTER_VECTORS_SAT(X)\
	CONVERTER_SAT(X, 2)\
	CONVERTER_SAT(X, 3)\
	CONVERTER_SAT(X, 4)\
	CONVERTER_SAT(X, 8)\
	CONVERTER_SAT(X, 16)

#define CONVERTER_VECTORS_RTZ(X)\
	CONVERTER_RTZ(X, 2)\
	CONVERTER_RTZ(X, 3)\
	CONVERTER_RTZ(X, 4)\
	CONVERTER_RTZ(X, 8)\
	CONVERTER_RTZ(X, 16)

#define CONVERTER_VECTORS_RTP(X)\
	CONVERTER_RTP(X, 2)\
	CONVERTER_RTP(X, 3)\
	CONVERTER_RTP(X, 4)\
	CONVERTER_RTP(X, 8)\
	CONVERTER_RTP(X, 16)

#define CONVERTER_VECTORS_RTN(X)\
	CONVERTER_RTN(X, 2)\
	CONVERTER_RTN(X, 3)\
	CONVERTER_RTN(X, 4)\
	CONVERTER_RTN(X, 8)\
	CONVERTER_RTN(X, 16)

#define CONVERTER_VECTORS_RTE(X)\
	CONVERTER_RTE(X, 2)\
	CONVERTER_RTE(X, 3)\
	CONVERTER_RTE(X, 4)\
	CONVERTER_RTE(X, 8)\
	CONVERTER_RTE(X, 16)

#define CONVERTER_VECTORS_SAT_RTZ(X)\
	CONVERTER_SAT_RTZ(X, 2)\
	CONVERTER_SAT_RTZ(X, 3)\
	CONVERTER_SAT_RTZ(X, 4)\
	CONVERTER_SAT_RTZ(X, 8)\
	CONVERTER_SAT_RTZ(X, 16)

#define CONVERTER_VECTORS_SAT_RTP(X)\
	CONVERTER_SAT_RTP(X, 2)\
	CONVERTER_SAT_RTP(X, 3)\
	CONVERTER_SAT_RTP(X, 4)\
	CONVERTER_SAT_RTP(X, 8)\
	CONVERTER_SAT_RTP(X, 16)

#define CONVERTER_VECTORS_SAT_RTN(X)\
	CONVERTER_SAT_RTN(X, 2)\
	CONVERTER_SAT_RTN(X, 3)\
	CONVERTER_SAT_RTN(X, 4)\
	CONVERTER_SAT_RTN(X, 8)\
	CONVERTER_SAT_RTN(X, 16)

#define CONVERTER_VECTORS_SAT_RTE(X)\
	CONVERTER_SAT_RTE(X, 2)\
	CONVERTER_SAT_RTE(X, 3)\
	CONVERTER_SAT_RTE(X, 4)\
	CONVERTER_SAT_RTE(X, 8)\
	CONVERTER_SAT_RTE(X, 16)

CONVERTER_VECTORS(char)
CONVERTER_VECTORS(uchar)
CONVERTER_VECTORS(short)
CONVERTER_VECTORS(ushort)
CONVERTER_VECTORS(int)
CONVERTER_VECTORS(uint)
CONVERTER_VECTORS(long)
CONVERTER_VECTORS(ulong)
CONVERTER_VECTORS(float)
CONVERTER_VECTORS(double)

CONVERTER_VECTORS_SAT(char)
CONVERTER_VECTORS_SAT(uchar)
CONVERTER_VECTORS_SAT(short)
CONVERTER_VECTORS_SAT(ushort)
CONVERTER_VECTORS_SAT(int)
CONVERTER_VECTORS_SAT(uint)
CONVERTER_VECTORS_SAT(long)
CONVERTER_VECTORS_SAT(ulong)

CONVERTER_VECTORS_RTZ(char)
CONVERTER_VECTORS_RTZ(uchar)
CONVERTER_VECTORS_RTZ(short)
CONVERTER_VECTORS_RTZ(ushort)
CONVERTER_VECTORS_RTZ(int)
CONVERTER_VECTORS_RTZ(uint)
CONVERTER_VECTORS_RTZ(long)
CONVERTER_VECTORS_RTZ(ulong)
CONVERTER_VECTORS_RTZ(float)
CONVERTER_VECTORS_RTZ(double)

CONVERTER_VECTORS_RTP(char)
CONVERTER_VECTORS_RTP(uchar)
CONVERTER_VECTORS_RTP(short)
CONVERTER_VECTORS_RTP(ushort)
CONVERTER_VECTORS_RTP(int)
CONVERTER_VECTORS_RTP(uint)
CONVERTER_VECTORS_RTP(long)
CONVERTER_VECTORS_RTP(ulong)
CONVERTER_VECTORS_RTP(float)
CONVERTER_VECTORS_RTP(double)

CONVERTER_VECTORS_RTN(char)
CONVERTER_VECTORS_RTN(uchar)
CONVERTER_VECTORS_RTN(short)
CONVERTER_VECTORS_RTN(ushort)
CONVERTER_VECTORS_RTN(int)
CONVERTER_VECTORS_RTN(uint)
CONVERTER_VECTORS_RTN(long)
CONVERTER_VECTORS_RTN(ulong)
CONVERTER_VECTORS_RTN(float)
CONVERTER_VECTORS_RTN(double)

CONVERTER_VECTORS_RTE(char)
CONVERTER_VECTORS_RTE(uchar)
CONVERTER_VECTORS_RTE(short)
CONVERTER_VECTORS_RTE(ushort)
CONVERTER_VECTORS_RTE(int)
CONVERTER_VECTORS_RTE(uint)
CONVERTER_VECTORS_RTE(long)
CONVERTER_VECTORS_RTE(ulong)
CONVERTER_VECTORS_RTE(float)
CONVERTER_VECTORS_RTE(double)

CONVERTER_VECTORS_SAT_RTZ(char)
CONVERTER_VECTORS_SAT_RTZ(uchar)
CONVERTER_VECTORS_SAT_RTZ(short)
CONVERTER_VECTORS_SAT_RTZ(ushort)
CONVERTER_VECTORS_SAT_RTZ(int)
CONVERTER_VECTORS_SAT_RTZ(uint)
CONVERTER_VECTORS_SAT_RTZ(long)
CONVERTER_VECTORS_SAT_RTZ(ulong)
CONVERTER_VECTORS_SAT_RTZ(float)
CONVERTER_VECTORS_SAT_RTZ(double)

CONVERTER_VECTORS_SAT_RTP(char)
CONVERTER_VECTORS_SAT_RTP(uchar)
CONVERTER_VECTORS_SAT_RTP(short)
CONVERTER_VECTORS_SAT_RTP(ushort)
CONVERTER_VECTORS_SAT_RTP(int)
CONVERTER_VECTORS_SAT_RTP(uint)
CONVERTER_VECTORS_SAT_RTP(long)
CONVERTER_VECTORS_SAT_RTP(ulong)
CONVERTER_VECTORS_SAT_RTP(float)
CONVERTER_VECTORS_SAT_RTP(double)

CONVERTER_VECTORS_SAT_RTN(char)
CONVERTER_VECTORS_SAT_RTN(uchar)
CONVERTER_VECTORS_SAT_RTN(short)
CONVERTER_VECTORS_SAT_RTN(ushort)
CONVERTER_VECTORS_SAT_RTN(int)
CONVERTER_VECTORS_SAT_RTN(uint)
CONVERTER_VECTORS_SAT_RTN(long)
CONVERTER_VECTORS_SAT_RTN(ulong)
CONVERTER_VECTORS_SAT_RTN(float)
CONVERTER_VECTORS_SAT_RTN(double)

CONVERTER_VECTORS_SAT_RTE(char)
CONVERTER_VECTORS_SAT_RTE(uchar)
CONVERTER_VECTORS_SAT_RTE(short)
CONVERTER_VECTORS_SAT_RTE(ushort)
CONVERTER_VECTORS_SAT_RTE(int)
CONVERTER_VECTORS_SAT_RTE(uint)
CONVERTER_VECTORS_SAT_RTE(long)
CONVERTER_VECTORS_SAT_RTE(ulong)
CONVERTER_VECTORS_SAT_RTE(float)
CONVERTER_VECTORS_SAT_RTE(double)

#undef CONVERTER_VECTORS
#undef CONVERTER
#undef CONVERTER_VECTORS_SAT
#undef CONVERTER_SAT
#undef CONVERTER_VECTORS_RTZ
#undef CONVERTER_RTZ
#undef CONVERTER_VECTORS_RTP
#undef CONVERTER_RTP
#undef CONVERTER_VECTORS_RTN
#undef CONVERTER_RTN
#undef CONVERTER_VECTORS_RTE
#undef CONVERTER_RTE
#undef CONVERTER_VECTORS_SAT_RTZ
#undef CONVERTER_SAT_RTZ
#undef CONVERTER_VECTORS_SAT_RTP
#undef CONVERTER_SAT_RTP
#undef CONVERTER_VECTORS_SAT_RTN
#undef CONVERTER_SAT_RTN
#undef CONVERTER_VECTORS_SAT_RTE
#undef CONVERTER_SAT_RTE

#define AS_TYPE(Out)\
template<typename In>	static inline const __##Out &as_##Out(const In &t)	{	return reinterpret_cast<const __##Out&>(t);	}\
template<typename In>	static inline __##Out &as_##Out(In &t)	{	return reinterpret_cast<__##Out&>(t);	}

#define AS_BASE_TYPE(Out)\
AS_TYPE(Out)\
AS_TYPE(Out##2)\
AS_TYPE(Out##3)\
AS_TYPE(Out##4)\
AS_TYPE(Out##8)\
AS_TYPE(Out##16)

AS_BASE_TYPE(char)
AS_BASE_TYPE(short)
AS_BASE_TYPE(int)
AS_BASE_TYPE(long)
AS_BASE_TYPE(uchar)
AS_BASE_TYPE(ushort)
AS_BASE_TYPE(uint)
AS_BASE_TYPE(ulong)
AS_BASE_TYPE(float)
AS_BASE_TYPE(double)
AS_TYPE(size_t)

#undef AS_TYPE
#undef AS_BASE_TYPE

#endif
