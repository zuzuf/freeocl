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

template<class T> struct __type_range;
template<> struct __type_range<char>		{	enum { __min = -128, __max = 127 };	};
template<> struct __type_range<uchar>		{	enum { __min = 0, __max = 255 };	};
template<> struct __type_range<short>		{	enum { __min = -32768, __max = 32767 };	};
template<> struct __type_range<ushort>		{	enum { __min = 0, __max = 65536 };	};
template<> struct __type_range<int>			{	enum { __min = -0x10000000, __max = 0x7FFFFFFF };	};
template<> struct __type_range<uint>		{	enum { __min = 0, __max = 0xFFFFFFFFU };	};
template<> struct __type_range<long>		{	enum { __min = -0x1000000000000000L, __max = 0x7FFFFFFFFFFFFFFFL };	};
template<> struct __type_range<ulong>		{	enum { __min = 0, __max = 0xFFFFFFFFFFFFFFFFLU };	};

// Built-in type conversion functions
#define CONVERTER(X)\
template<class Scalar>\
inline typename __right<typename __scalar<Scalar>::type, X>::type convert_##X(const Scalar &v)	{	return (X)v;	}

#define CONVERTER_SAT(X)\
template<class Scalar>\
inline typename __right<typename __scalar<Scalar>::type, X>::type convert_##X##_sat(const Scalar &v)\
{\
	return (X)(v > __type_range<X>::__max\
			   ? __type_range<X>::__max\
			   : v < __type_range<X>::__min\
				 ? __type_range<X>::__min\
				 : v);\
}

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

#undef CONVERTER
#undef CONVERTER_SAT

#define CONVERTER(X, N)\
template<class V>\
	inline typename __if<__vector<V>::components == N, X##N>::type convert_##X##N(const V &v)\
{\
	X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = (X)v.v[i];\
	return ret;\
}
#define CONVERTER_SAT(X, N)\
template<class V>\
	inline typename __if<__vector<V>::components == N, X##N>::type convert_##X##N##_sat(const V &v)\
{\
	X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = convert_##X##_sat(v.v[i]);\
	return ret;\
}

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

#undef CONVERTER_VECTORS
#undef CONVERTER
#undef CONVERTER_VECTORS_SAT
#undef CONVERTER_SAT

#endif
