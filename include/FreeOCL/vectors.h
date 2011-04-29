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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_VECTORS_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_VECTORS_H__
// Built-in vector types

// A small template to get base type and components of a vector type
template<typename V>	struct __vector;

// this template exists only if its first parameter is true
// it returns T in its type member
template<bool, class T>	struct _If;
template<class T>	struct _If<true, T>	{	typedef T	type;	};

// this template is a wrapper to avoid adding endless my_type::value everywhere
template<class B, class T>	struct If : public _If<B::value, T>	{};

// This predicate is true for vector types
template<class T>	struct IsAVector	{	enum { value = false };	};
template<typename V>	struct IsAVector<__vector<V> >	{	enum { value = true };	};

// The operator not for template values
template<class B>	struct Not	{	enum { value = !B::value };	};

#define DEFINE_VECTOR_TYPE(X, N)\
struct X##N\
{\
	X v[N];\
	X##N() {}\
	explicit X##N(X p0) {	for(size_t i = 0 ; i < N ; ++i)	v[i] = p0;	}\
	explicit X##N(X p0, X p1) {	v[0] = p0, v[1] = p1;	}\
	explicit X##N(X p0, X p1, X p2) {	v[0] = p0, v[1] = p1, v[2] = p2;	}\
	explicit X##N(X p0, X p1, X p2, X p3) {	v[0] = p0, v[1] = p1, v[2] = p2, v[3] = p3;	}\
	explicit X##N(X p0, X p1, X p2, X p3, X p4, X p5, X p6, X p7) {	v[0] = p0, v[1] = p1, v[2] = p2, v[3] = p3, v[4] = p4, v[5] = p5, v[6] = p6, v[7] = p7;	}\
	explicit X##N(X p0, X p1, X p2, X p3, X p4, X p5, X p6, X p7, X p8, X p9, X pA, X pB, X pC, X pD, X pE, X pF) {	v[0] = p0, v[1] = p1, v[2] = p2, v[3] = p3, v[4] = p4, v[5] = p5, v[6] = p6, v[7] = p7, v[8] = p8, v[9] = p9, v[10] = pA, v[11] = pB, v[12] = pC, v[13] = pD, v[14] = pE, v[15] = pF;	}\
};\
template<>	struct __vector<X##N>\
{\
	typedef X	base_type;\
	enum { components = N };\
	typedef X##N	type;\
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

// vector op vector
#define IMPLEMENT_BINARY_OP(op)\
template<class V>\
inline typename __vector<V>::type operator op (const V &lhs, const V &rhs)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = lhs.v[i] op rhs.v[i];\
	return ret;\
}

IMPLEMENT_BINARY_OP(+)
IMPLEMENT_BINARY_OP(-)
IMPLEMENT_BINARY_OP(*)
IMPLEMENT_BINARY_OP(/)
IMPLEMENT_BINARY_OP(%)
IMPLEMENT_BINARY_OP(<<)
IMPLEMENT_BINARY_OP(>>)
IMPLEMENT_BINARY_OP(|)
IMPLEMENT_BINARY_OP(&)
IMPLEMENT_BINARY_OP(^)

#undef IMPLEMENT_BINARY_OP

// scalar op vector
#define IMPLEMENT_BINARY_OP(op)\
template<class V>\
inline typename __vector<V>::type operator op (const typename __vector<V>::base_type &lhs, const V &rhs)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = lhs op rhs.v[i];\
	return ret;\
}

IMPLEMENT_BINARY_OP(+)
IMPLEMENT_BINARY_OP(-)
IMPLEMENT_BINARY_OP(*)
IMPLEMENT_BINARY_OP(/)
IMPLEMENT_BINARY_OP(%)
IMPLEMENT_BINARY_OP(<<)
IMPLEMENT_BINARY_OP(>>)
IMPLEMENT_BINARY_OP(|)
IMPLEMENT_BINARY_OP(&)
IMPLEMENT_BINARY_OP(^)

#undef IMPLEMENT_BINARY_OP

// vector op scalar
#define IMPLEMENT_BINARY_OP(op)\
template<class V>\
inline typename __vector<V>::type operator op (const V &lhs, const typename __vector<V>::base_type &rhs)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = lhs.v[i] op rhs;\
	return ret;\
}

IMPLEMENT_BINARY_OP(+)
IMPLEMENT_BINARY_OP(-)
IMPLEMENT_BINARY_OP(*)
IMPLEMENT_BINARY_OP(/)
IMPLEMENT_BINARY_OP(%)
IMPLEMENT_BINARY_OP(<<)
IMPLEMENT_BINARY_OP(>>)
IMPLEMENT_BINARY_OP(|)
IMPLEMENT_BINARY_OP(&)
IMPLEMENT_BINARY_OP(^)

#undef IMPLEMENT_BINARY_OP

// vector op= vector
#define IMPLEMENT_ASSIGN_OP(op)\
template<class V>\
inline typename __vector<V>::type &operator op (V &lhs, const V &rhs)\
{\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		lhs.v[i] op rhs.v[i];\
	return lhs;\
}

IMPLEMENT_ASSIGN_OP(+=)
IMPLEMENT_ASSIGN_OP(-=)
IMPLEMENT_ASSIGN_OP(*=)
IMPLEMENT_ASSIGN_OP(/=)
IMPLEMENT_ASSIGN_OP(%=)

#undef IMPLEMENT_ASSIGN_OP

#endif
