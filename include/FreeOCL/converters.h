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

// Built-in type conversion functions
#define CONVERTER(X)\
template<class Scalar>\
inline typename __right<typename __scalar<Scalar>::type, X>::type convert_##X(const Scalar &v)	{	return (X)v;	}

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

#undef CONVERTER

template<bool, class T> struct __if;
template<class T> struct __if<true, T>	{	typedef T type;	};

#define CONVERTER(X, N)\
template<class V>\
	inline typename __if<__vector<V>::components == N, X##N>::type convert_##X##N(const V &v)\
{\
	X##N ret;\
	for(size_t i = 0 ; i < N ; ++i)\
		ret.v[i] = (X)v.v[i];\
	return ret;\
}

#define CONVERTER_VECTORS(X)\
	CONVERTER(X, 2)\
	CONVERTER(X, 3)\
	CONVERTER(X, 4)\
	CONVERTER(X, 8)\
	CONVERTER(X, 16)

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

#undef CONVERTER_VECTORS
#undef CONVERTER

#endif
