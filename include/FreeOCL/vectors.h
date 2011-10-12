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
#include "opencl_c.h"

// Built-in vector types

// A small template to get base type and components of a vector type
template<typename V>	struct __vector;

template<typename B, int N>	struct __vector_type;

template<int i0, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int iA, int iB, int iC, int iD, int iE, int iF, int N> struct Get;
#define DEF(N)\
template<int i0, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10, int i11, int i12, int i13, int i14, int i15>\
struct Get<i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15,N>	{	enum { value = i##N };	}

template<int i0, int i1, int i2, int i3, int i4, int i5, int i6, int i7, int i8, int i9, int i10, int i11, int i12, int i13, int i14, int i15>
struct Get<i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15,-1>	{	enum { value = 0 };	};

DEF(0);
DEF(1);
DEF(2);
DEF(3);
DEF(4);
DEF(5);
DEF(6);
DEF(7);
DEF(8);
DEF(9);
DEF(10);
DEF(11);
DEF(12);
DEF(13);
DEF(14);
DEF(15);

#undef DEF

template<class V,
		 class W,
		 int i0, int i1,
		 int i2 = -1, int i3 = -1,
		 int i4 = -1, int i5 = -1, int i6 = -1, int i7 = -1, int i8 = -1,
		 int i9 = -1, int iA = -1, int iB = -1, int iC = -1, int iD = -1, int iE = -1, int iF = -1>
class __swizzle_wrapper
{
public:
	typedef W type;
#define _get(X)	Get<i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,iA,iB,iC,iD,iE,iF,X>::value
public:
	inline __swizzle_wrapper(V &ref) : ref(ref)	{}
	inline operator W() const
	{
		W v;
		if (i0 != -1)	v.v[0] = ref.v[i0];
		if (i1 != -1)	v.v[1] = ref.v[i1];
		if (i2 != -1)	v.v[2] = ref.v[i2];
		if (i3 != -1)	v.v[3] = ref.v[i3];
		if (i4 != -1)	v.v[4] = ref.v[i4];
		if (i5 != -1)	v.v[5] = ref.v[i5];
		if (i6 != -1)	v.v[6] = ref.v[i6];
		if (i7 != -1)	v.v[7] = ref.v[i7];
		if (i8 != -1)	v.v[8] = ref.v[i8];
		if (i9 != -1)	v.v[9] = ref.v[i9];
		if (iA != -1)	v.v[10] = ref.v[iA];
		if (iB != -1)	v.v[11] = ref.v[iB];
		if (iC != -1)	v.v[12] = ref.v[iC];
		if (iD != -1)	v.v[13] = ref.v[iD];
		if (iE != -1)	v.v[14] = ref.v[iE];
		if (iF != -1)	v.v[15] = ref.v[iF];
		return v;
	}
#define IMPLEMENT_OPERATOR(op)\
	inline __swizzle_wrapper &operator op(const W &v)\
	{\
		if (i0 != -1)	ref.v[i0] op v.v[0];\
		if (i1 != -1)	ref.v[i1] op v.v[1];\
		if (i2 != -1)	ref.v[i2] op v.v[2];\
		if (i3 != -1)	ref.v[i3] op v.v[3];\
		if (i4 != -1)	ref.v[i4] op v.v[4];\
		if (i5 != -1)	ref.v[i5] op v.v[5];\
		if (i6 != -1)	ref.v[i6] op v.v[6];\
		if (i7 != -1)	ref.v[i7] op v.v[7];\
		if (i8 != -1)	ref.v[i8] op v.v[8];\
		if (i9 != -1)	ref.v[i9] op v.v[9];\
		if (iA != -1)	ref.v[iA] op v.v[10];\
		if (iB != -1)	ref.v[iB] op v.v[11];\
		if (iC != -1)	ref.v[iC] op v.v[12];\
		if (iD != -1)	ref.v[iD] op v.v[13];\
		if (iE != -1)	ref.v[iE] op v.v[14];\
		if (iF != -1)	ref.v[iF] op v.v[15];\
		return *this;\
	}\
	inline __swizzle_wrapper &operator op(const typename __vector<W>::base_type &v)\
	{\
		if (i0 != -1)	ref.v[i0] op v;\
		if (i1 != -1)	ref.v[i1] op v;\
		if (i2 != -1)	ref.v[i2] op v;\
		if (i3 != -1)	ref.v[i3] op v;\
		if (i4 != -1)	ref.v[i4] op v;\
		if (i5 != -1)	ref.v[i5] op v;\
		if (i6 != -1)	ref.v[i6] op v;\
		if (i7 != -1)	ref.v[i7] op v;\
		if (i8 != -1)	ref.v[i8] op v;\
		if (i9 != -1)	ref.v[i9] op v;\
		if (iA != -1)	ref.v[iA] op v;\
		if (iB != -1)	ref.v[iB] op v;\
		if (iC != -1)	ref.v[iC] op v;\
		if (iD != -1)	ref.v[iD] op v;\
		if (iE != -1)	ref.v[iE] op v;\
		if (iF != -1)	ref.v[iF] op v;\
		return *this;\
	}
	IMPLEMENT_OPERATOR(=)
	IMPLEMENT_OPERATOR(+=)
	IMPLEMENT_OPERATOR(-=)
	IMPLEMENT_OPERATOR(*=)
	IMPLEMENT_OPERATOR(/=)
#undef IMPLEMENT_OPERATOR
	template<class X,
			 int j0, int j1,
			 int j2, int j3,
			 int j4, int j5, int j6, int j7,
			 int j8, int j9, int jA, int jB, int jC, int jD, int jE, int jF>
	inline __swizzle_wrapper<const V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)> swizzle() const
			 {	return __swizzle_wrapper<const V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)>(ref);	}
	template<class X,
			 int j0, int j1,
			 int j2, int j3,
			 int j4, int j5, int j6, int j7,
			 int j8, int j9, int jA, int jB, int jC, int jD, int jE, int jF>
	inline __swizzle_wrapper<V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)> swizzle()
			 {	return __swizzle_wrapper<V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)>(ref);	}
	template<int i>
	inline typename __vector<V>::base_type &get()	{	return ref.get<_get(i)>();	}
	template<int i>
	inline const typename __vector<V>::base_type &get() const	{	return ref.get<_get(i)>();	}

#undef _get
protected:
	V &ref;
};

template<class V,
		 class W,
		 int i0, int i1,
		 int i2 = -1, int i3 = -1,
		 int i4 = -1, int i5 = -1, int i6 = -1, int i7 = -1, int i8 = -1,
		 int i9 = -1, int iA = -1, int iB = -1, int iC = -1, int iD = -1, int iE = -1, int iF = -1>
class __swizzle_wrapper_int : public __swizzle_wrapper<V, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF>
{
public:
	typedef W type;
	typedef __swizzle_wrapper<V, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF> super;
#define _get(X)	Get<i0,i1,i2,i3,i4,i5,i6,i7,i8,i9,iA,iB,iC,iD,iE,iF,X>::value
public:
	inline __swizzle_wrapper_int(V &ref) : super(ref)	{}
#define IMPLEMENT_OPERATOR(op)\
	inline __swizzle_wrapper_int &operator op(const W &v)\
	{\
		if (i0 != -1)	super::ref.v[i0] op v.v[0];\
		if (i1 != -1)	super::ref.v[i1] op v.v[1];\
		if (i2 != -1)	super::ref.v[i2] op v.v[2];\
		if (i3 != -1)	super::ref.v[i3] op v.v[3];\
		if (i4 != -1)	super::ref.v[i4] op v.v[4];\
		if (i5 != -1)	super::ref.v[i5] op v.v[5];\
		if (i6 != -1)	super::ref.v[i6] op v.v[6];\
		if (i7 != -1)	super::ref.v[i7] op v.v[7];\
		if (i8 != -1)	super::ref.v[i8] op v.v[8];\
		if (i9 != -1)	super::ref.v[i9] op v.v[9];\
		if (iA != -1)	super::ref.v[iA] op v.v[10];\
		if (iB != -1)	super::ref.v[iB] op v.v[11];\
		if (iC != -1)	super::ref.v[iC] op v.v[12];\
		if (iD != -1)	super::ref.v[iD] op v.v[13];\
		if (iE != -1)	super::ref.v[iE] op v.v[14];\
		if (iF != -1)	super::ref.v[iF] op v.v[15];\
		return *this;\
	}\
	inline __swizzle_wrapper_int &operator op(const typename __vector<W>::base_type &v)\
	{\
		if (i0 != -1)	super::ref.v[i0] op v;\
		if (i1 != -1)	super::ref.v[i1] op v;\
		if (i2 != -1)	super::ref.v[i2] op v;\
		if (i3 != -1)	super::ref.v[i3] op v;\
		if (i4 != -1)	super::ref.v[i4] op v;\
		if (i5 != -1)	super::ref.v[i5] op v;\
		if (i6 != -1)	super::ref.v[i6] op v;\
		if (i7 != -1)	super::ref.v[i7] op v;\
		if (i8 != -1)	super::ref.v[i8] op v;\
		if (i9 != -1)	super::ref.v[i9] op v;\
		if (iA != -1)	super::ref.v[iA] op v;\
		if (iB != -1)	super::ref.v[iB] op v;\
		if (iC != -1)	super::ref.v[iC] op v;\
		if (iD != -1)	super::ref.v[iD] op v;\
		if (iE != -1)	super::ref.v[iE] op v;\
		if (iF != -1)	super::ref.v[iF] op v;\
		return *this;\
	}
	IMPLEMENT_OPERATOR(%=)
	IMPLEMENT_OPERATOR(^=)
	IMPLEMENT_OPERATOR(|=)
	IMPLEMENT_OPERATOR(&=)
	IMPLEMENT_OPERATOR(<<=)
	IMPLEMENT_OPERATOR(>>=)
#undef IMPLEMENT_OPERATOR
	template<class X,
			 int j0, int j1,
			 int j2, int j3,
			 int j4, int j5, int j6, int j7,
			 int j8, int j9, int jA, int jB, int jC, int jD, int jE, int jF>
	inline __swizzle_wrapper_int<const V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)> swizzle() const
			 {	return __swizzle_wrapper_int<const V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)>(super::ref);	}
	template<class X,
			 int j0, int j1,
			 int j2, int j3,
			 int j4, int j5, int j6, int j7,
			 int j8, int j9, int jA, int jB, int jC, int jD, int jE, int jF>
	inline __swizzle_wrapper_int<V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)> swizzle()
			 {	return __swizzle_wrapper_int<V, X, _get(j0), _get(j1), _get(j2), _get(j3), _get(j4), _get(j5), _get(j6), _get(j7), _get(j8), _get(j9), _get(jA), _get(jB), _get(jC), _get(jD), _get(jE), _get(jF)>(super::ref);	}

#undef _get
};

#define IMPLEMENT_OPERATOR(op, X, N)\
inline X##N &operator op(const X##N &w)\
{\
	for(size_t i = 0 ; i < N ; ++i)	v[i] op w.v[i];\
	return *this;\
}\
inline X##N &operator op(const X &w)\
{\
	for(size_t i = 0 ; i < N ; ++i)	v[i] op w;\
	return *this;\
}

#define DEFINE_VECTOR_MAKE_1(X, N)	static inline X##N make(X p0) {	X##N v;	for(size_t i = 0 ; i < N ; ++i)	v.v[i] = p0;	return v;	}
#define DEFINE_VECTOR_MAKE_2(X)	static inline X##2 make(X p0, X p1) {	X##2 v;	v.v[0] = p0, v.v[1] = p1;	return v;	}
#define DEFINE_VECTOR_MAKE_3(X)	static inline X##3 make(X p0, X p1, X p2) {	X##3 v;	v.v[0] = p0, v.v[1] = p1, v.v[2] = p2;	return v;	}
#define DEFINE_VECTOR_MAKE_4(X)	static inline X##4 make(X p0, X p1, X p2, X p3) {	X##4 v;	v.v[0] = p0, v.v[1] = p1, v.v[2] = p2, v.v[3] = p3; return v;	}
#define DEFINE_VECTOR_MAKE_8(X)	static inline X##8 make(X p0, X p1, X p2, X p3, X p4, X p5, X p6, X p7) {	X##8 v;	v.v[0] = p0, v.v[1] = p1, v.v[2] = p2, v.v[3] = p3, v.v[4] = p4, v.v[5] = p5, v.v[6] = p6, v.v[7] = p7;	return v;	}
#define DEFINE_VECTOR_MAKE_16(X)	static inline X##16 make(X p0, X p1, X p2, X p3, X p4, X p5, X p6, X p7, X p8, X p9, X pA, X pB, X pC, X pD, X pE, X pF) {	X##16 v;	v.v[0] = p0, v.v[1] = p1, v.v[2] = p2, v.v[3] = p3, v.v[4] = p4, v.v[5] = p5, v.v[6] = p6, v.v[7] = p7, v.v[8] = p8, v.v[9] = p9, v.v[10] = pA, v.v[11] = pB, v.v[12] = pC, v.v[13] = pD, v.v[14] = pE, v.v[15] = pF;	return v;	}

#define DEFINE_VECTOR_TYPE(X, N)\
struct X##N\
{\
	X v[N];\
	DEFINE_VECTOR_MAKE_1(X, N)\
	DEFINE_VECTOR_MAKE_##N(X)\
	template<class W,\
			 int i0, int i1,\
			 int i2, int i3,\
			 int i4, int i5, int i6, int i7,\
			 int i8, int i9, int iA, int iB, int iC, int iD, int iE, int iF>\
	inline __swizzle_wrapper<const X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF> swizzle() const\
			 {	return __swizzle_wrapper<const X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF>(*this);	}\
	template<class W,\
			 int i0, int i1,\
			 int i2, int i3,\
			 int i4, int i5, int i6, int i7,\
			 int i8, int i9, int iA, int iB, int iC, int iD, int iE, int iF>\
	inline __swizzle_wrapper<X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF> swizzle()\
			 {	return __swizzle_wrapper<X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF>(*this);	}\
	template<int i>\
	inline X &get()	{	return v[i];	}\
	template<int i>\
	inline const X &get() const	{	return v[i];	}\
	IMPLEMENT_OPERATOR(=,X,N)\
	IMPLEMENT_OPERATOR(+=,X,N)\
	IMPLEMENT_OPERATOR(-=,X,N)\
	IMPLEMENT_OPERATOR(*=,X,N)\
	IMPLEMENT_OPERATOR(/=,X,N)\
};\
template<>	struct __vector<X##N>\
{\
	typedef X	base_type;\
	enum { components = N };\
	typedef X##N	type;\
};\
template<>	struct __vector_type<X, N>\
{\
	typedef X##N	type;\
}

#define DEFINE_VECTOR_INT_TYPE(X, N)\
struct X##N\
{\
	X v[N];\
	DEFINE_VECTOR_MAKE_1(X, N)\
	DEFINE_VECTOR_MAKE_##N(X)\
	template<class W,\
			 int i0, int i1,\
			 int i2, int i3,\
			 int i4, int i5, int i6, int i7,\
			 int i8, int i9, int iA, int iB, int iC, int iD, int iE, int iF>\
	inline __swizzle_wrapper_int<const X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF> swizzle() const\
			 {	return __swizzle_wrapper_int<const X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF>(*this);	}\
	template<class W,\
			 int i0, int i1,\
			 int i2, int i3,\
			 int i4, int i5, int i6, int i7,\
			 int i8, int i9, int iA, int iB, int iC, int iD, int iE, int iF>\
	inline __swizzle_wrapper_int<X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF> swizzle()\
			 {	return __swizzle_wrapper_int<X##N, W, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF>(*this);	}\
	template<int i>\
	inline X &get()	{	return v[i];	}\
	template<int i>\
	inline const X &get() const	{	return v[i];	}\
	IMPLEMENT_OPERATOR(=,X,N)\
	IMPLEMENT_OPERATOR(+=,X,N)\
	IMPLEMENT_OPERATOR(-=,X,N)\
	IMPLEMENT_OPERATOR(*=,X,N)\
	IMPLEMENT_OPERATOR(/=,X,N)\
	IMPLEMENT_OPERATOR(%=,X,N)\
	IMPLEMENT_OPERATOR(^=,X,N)\
	IMPLEMENT_OPERATOR(|=,X,N)\
	IMPLEMENT_OPERATOR(&=,X,N)\
	IMPLEMENT_OPERATOR(<<=,X,N)\
	IMPLEMENT_OPERATOR(>>=,X,N)\
};\
template<>	struct __vector<X##N>\
{\
	typedef X	base_type;\
	enum { components = N };\
	typedef X##N	type;\
};\
template<>	struct __vector_type<X, N>\
{\
	typedef X##N	type;\
}

#define DEFINE_VECTORS(X)\
DEFINE_VECTOR_TYPE(X, 2);\
DEFINE_VECTOR_TYPE(X, 3);\
DEFINE_VECTOR_TYPE(X, 4);\
DEFINE_VECTOR_TYPE(X, 8);\
DEFINE_VECTOR_TYPE(X, 16)

#define DEFINE_VECTORS_INT(X)\
DEFINE_VECTOR_INT_TYPE(X, 2);\
DEFINE_VECTOR_INT_TYPE(X, 3);\
DEFINE_VECTOR_INT_TYPE(X, 4);\
DEFINE_VECTOR_INT_TYPE(X, 8);\
DEFINE_VECTOR_INT_TYPE(X, 16)

DEFINE_VECTORS_INT(char);
DEFINE_VECTORS_INT(short);
DEFINE_VECTORS_INT(int);
DEFINE_VECTORS_INT(long);
DEFINE_VECTORS_INT(uchar);
DEFINE_VECTORS_INT(ushort);
DEFINE_VECTORS_INT(uint);
DEFINE_VECTORS_INT(ulong);
DEFINE_VECTORS(float);
DEFINE_VECTORS(double);

#undef DEFINE_VECTORS
#undef DEFINE_VECTOR_TYPE
#undef DEFINE_VECTORS_INT
#undef DEFINE_VECTOR_INT_TYPE
#undef IMPLEMENT_OPERATOR

// vector op vector
#define IMPLEMENT_BINARY_OP(op)\
template<class V>\
inline typename __vector<V>::type operator op (const V &lhs, const V &rhs)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = lhs.v[i] op rhs.v[i];\
	return ret;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const typename W::type &lhs, const W &rhs)\
{\
	return lhs op (typename W::type)rhs;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const W &lhs, const typename W::type &rhs)\
{\
	return (typename W::type)lhs op rhs;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const W &lhs, const W &rhs)\
{\
	return (typename W::type)lhs op (typename W::type)rhs;\
}

#define IMPLEMENT_BINARY_OP_LOGICAL(op)\
template<class V>\
inline typename __vector_type<typename __sint_type_of_same_size<typename __vector<V>::base_type>::type, __vector<V>::components>::type operator op (const V &lhs, const V &rhs)\
{\
	typename __vector_type<typename __sint_type_of_same_size<typename __vector<V>::base_type>::type, __vector<V>::components>::type ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = -(lhs.v[i] op rhs.v[i]);\
	return ret;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const typename W::type &lhs, const W &rhs)\
{\
	return lhs op (typename W::type)rhs;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const W &lhs, const typename W::type &rhs)\
{\
	return (typename W::type)lhs op rhs;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const W &lhs, const W &rhs)\
{\
	return (typename W::type)lhs op (typename W::type)rhs;\
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
IMPLEMENT_BINARY_OP_LOGICAL(<)
IMPLEMENT_BINARY_OP_LOGICAL(>)
IMPLEMENT_BINARY_OP_LOGICAL(<=)
IMPLEMENT_BINARY_OP_LOGICAL(>=)
IMPLEMENT_BINARY_OP_LOGICAL(!=)
IMPLEMENT_BINARY_OP_LOGICAL(==)

#undef IMPLEMENT_BINARY_OP
#undef IMPLEMENT_BINARY_OP_LOGICAL

// scalar op vector
#define IMPLEMENT_BINARY_OP(op)\
template<class V>\
inline typename __vector<V>::type operator op (const typename __vector<V>::base_type &lhs, const V &rhs)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = lhs op rhs.v[i];\
	return ret;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const typename __vector<typename W::type>::base_type &lhs, const W &rhs)\
{\
	return lhs op (typename W::type)rhs;\
}
#define IMPLEMENT_BINARY_OP_LOGICAL(op)\
template<class V>\
inline typename __vector_type<typename __sint_type_of_same_size<typename __vector<V>::base_type>::type, __vector<V>::components>::type operator op (const typename __vector<V>::base_type &lhs, const V &rhs)\
{\
	typename __vector_type<typename __sint_type_of_same_size<typename __vector<V>::base_type>::type, __vector<V>::components>::type ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = -(lhs op rhs.v[i]);\
	return ret;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const typename __vector<typename W::type>::base_type &lhs, const W &rhs)\
{\
	return lhs op (typename W::type)rhs;\
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
IMPLEMENT_BINARY_OP_LOGICAL(<)
IMPLEMENT_BINARY_OP_LOGICAL(>)
IMPLEMENT_BINARY_OP_LOGICAL(<=)
IMPLEMENT_BINARY_OP_LOGICAL(>=)
IMPLEMENT_BINARY_OP_LOGICAL(!=)
IMPLEMENT_BINARY_OP_LOGICAL(==)

#undef IMPLEMENT_BINARY_OP
#undef IMPLEMENT_BINARY_OP_LOGICAL

// vector op scalar
#define IMPLEMENT_BINARY_OP(op)\
template<class V>\
inline typename __vector<V>::type operator op (const V &lhs, const typename __vector<V>::base_type &rhs)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = lhs.v[i] op rhs;\
	return ret;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const W &lhs, const typename __vector<typename W::type>::base_type &rhs)\
{\
	return (typename W::type)lhs op rhs;\
}
#define IMPLEMENT_BINARY_OP_LOGICAL(op)\
template<class V>\
inline typename __vector_type<typename __sint_type_of_same_size<typename __vector<V>::base_type>::type, __vector<V>::components>::type operator op (const V &lhs, const typename __vector<V>::base_type &rhs)\
{\
	typename __vector_type<typename __sint_type_of_same_size<typename __vector<V>::base_type>::type, __vector<V>::components>::type ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
	ret.v[i] = -(lhs.v[i] op rhs);\
	return ret;\
}\
template<class W>\
inline typename __vector<typename W::type>::type operator op (const W &lhs, const typename __vector<typename W::type>::base_type &rhs)\
{\
	return lhs op (typename W::type)rhs;\
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
IMPLEMENT_BINARY_OP_LOGICAL(<)
IMPLEMENT_BINARY_OP_LOGICAL(>)
IMPLEMENT_BINARY_OP_LOGICAL(<=)
IMPLEMENT_BINARY_OP_LOGICAL(>=)
IMPLEMENT_BINARY_OP_LOGICAL(!=)
IMPLEMENT_BINARY_OP_LOGICAL(==)

#undef IMPLEMENT_BINARY_OP
#undef IMPLEMENT_BINARY_OP_LOGICAL

// Unary + and -
#define IMPLEMENT_UNARY_OP(op)\
template<class V>\
inline typename __vector<V>::type operator op (const V &lhs)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = op lhs.v[i];\
	return ret;\
}

IMPLEMENT_UNARY_OP(-)
IMPLEMENT_UNARY_OP(+)

#undef IMPLEMENT_UNARY_OP

// Unary ++ and --
#define IMPLEMENT_UNARY_OP(op)\
template<class V>\
inline typename __vector<V>::type &operator op (V &lhs)\
{\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		op lhs.v[i];\
	return lhs;\
}\
template<class V>\
inline typename __vector<V>::type &operator op (V &lhs, int)\
{\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		lhs.v[i] op;\
	return lhs;\
}

IMPLEMENT_UNARY_OP(--)
IMPLEMENT_UNARY_OP(++)

#undef IMPLEMENT_UNARY_OP

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

// Tools to define vector functions using scalar ones
template<class V> struct __float_vector_predicate;
#define DEFINE(X) template<> struct __float_vector_predicate<X>	{	typedef X type;	}

DEFINE(float2);
DEFINE(float3);
DEFINE(float4);
DEFINE(float8);
DEFINE(float16);
DEFINE(double2);
DEFINE(double3);
DEFINE(double4);
DEFINE(double8);
DEFINE(double16);

#undef DEFINE

#define CHECK(X)	typename __float_vector_predicate<X>::type

#define VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(F)\
template<class V> inline CHECK(V) F(const V &x)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = F(x.v[i]);\
	return ret;\
}
#define VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(F)\
template<class V> inline CHECK(V) F(const V &x, const V &y)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = F(x.v[i], y.v[i]);\
	return ret;\
}
#define VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2I(F)\
template<class V> inline CHECK(V) F(const V &x, const typename __vector_type<int, __vector<V>::components>::type &y)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = F(x.v[i], y.v[i]);\
	return ret;\
}
#define VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2PI(F)\
template<class V> inline CHECK(V) F(const V &x, typename __vector_type<int, __vector<V>::components>::type *y)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = F(x.v[i], &(y->v[i]));\
	return ret;\
}
#define VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2PB(F)\
template<class V> inline CHECK(V) F(const V &x, V *y)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = F(x.v[i], &(y->v[i]));\
	return ret;\
}
#define VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3(F)\
template<class V> inline CHECK(V) F(const V &x, const V &y, const V &z)\
{\
	V ret;\
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)\
		ret.v[i] = F(x.v[i], y.v[i], z.v[i]);\
	return ret;\
}
#endif
