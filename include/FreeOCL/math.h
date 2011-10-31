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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_MATH_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_MATH_H__
#include "vectors.h"

#include <math.h>

// Built-in math functions
// for double
inline double acospi(double x)	{	return acos(x) / M_PI;	}
inline double asinpi(double x)	{	return asin(x) / M_PI;	}
inline double atanpi(double x)	{	return atan(x) / M_PI;	}
inline double atan2pi(double y, double x)	{	return atan2(y, x) / M_PI;	}
inline double cospi(double x)	{	return cos(M_PI * x);	}
inline double fract(double x, double *iptr)
{
	const double _x = floor(x);
	*iptr = _x;
	return fmin(x - _x, 0x1.fffffep-1f);
}
inline double mad(double a, double b, double c)	{	return a * b + c;	}
inline double maxmag(double x, double y)	{	return fmax(fabs(x), fabs(y));	}
inline double minmag(double x, double y)	{	return fmin(fabs(x), fabs(y));	}
inline double pown(double x, int n)
{
	double _m = 1.0;
	if (n < 0)
	{
		x = 1.0 / x;
		n = -n;
	}
	for( ; n > 0 ; n >>= 1)
	{
		if (n & 1)
			_m *= x;
	}
	return _m;
}
inline double powr(double x, double y)	{	return pow(x, y);	}
inline double rootn(double x, int y)	{	return pow(x, 1.0 / y);	}
inline double rsqrt(double x)	{	return 1.0 / sqrt(x);	}
inline double sincos(double x, double *cosval)
{
#ifdef _GNU_SOURCE
	double _sin;
	sincos(x, &_sin, cosval);
	return _sin;
#else
	*cosval = cos(x);
	return sin(x);
#endif
}
inline double sinpi(double x)	{	return sin(M_PI * x);	}
inline double tanpi(double x)	{	return tan(M_PI * x);	}

// for float
inline float acos(float x)	{	return acosf(x);	}
inline float acosh(float x)	{	return acoshf(x);	}
inline float acospi(float x)	{	return acos(x) / M_PI_F;	}
inline float asin(float x)	{	return asinf(x);	}
inline float asinh(float x)	{	return asinhf(x);	}
inline float asinpi(float x)	{	return asin(x) / M_PI_F;	}
inline float atan(float x)	{	return atanf(x);	}
inline float atan2(float y, float x)	{	return atan2f(y, x);	}
inline float atanh(float x)	{	return atanhf(x);	}
inline float atanpi(float x)	{	return atan(x) / M_PI_F;	}
inline float atan2pi(float y, float x)	{	return atan2(y, x) / M_PI_F;	}
inline float cbrt(float x)	{	return cbrtf(x);	}
inline float ceil(float x)	{	return ceilf(x);	}
inline float copysign(float x, float y)	{	return copysignf(x, y);	}
inline float cos(float x)	{	return cosf(x);	}
inline float cosh(float x)	{	return coshf(x);	}
inline float cospi(float x)	{	return cos(M_PI_F * x);	}
inline float erfc(float x)	{	return erfcf(x);	}
inline float erf(float x)	{	return erff(x);	}
inline float exp(float x)	{	return expf(x);	}
inline float exp2(float x)	{	return exp2f(x);	}
inline float exp10(float x)
{
#ifdef _GNU_SOURCE
	return exp10f(x);
#else
	return exp(x * 2.3025851f);
#endif
}
inline float expm1(float x)	{	return expm1f(x);	}
inline float fabs(float x)	{	return fabsf(x);	}
inline float fdim(float x, float y)	{	return fdimf(x, y);	}
inline float floor(float x)	{	return floorf(x);	}
inline float fma(float a, float b, float c)	{	return fmaf(a, b, c);	}
inline float fmax(float x, float y)	{	return fmaxf(x, y);	}
inline float fmin(float x, float y)	{	return fminf(x, y);	}
inline float fmod(float x, float y)	{	return fmodf(x, y);	}
inline float fract(float x, float *iptr)
{
	const float _x = floor(x);
	*iptr = _x;
	return fmin(x - _x, 0x1.fffffep-1f);
}
inline float frexp(float x, int *exp)	{	return frexpf(x, exp);	}
inline float hypot(float x, float y)	{	return hypotf(x, y);	}
inline float ldexp(float x, int n)	{	return ldexpf(x, n);	}
inline float lgamma(float x)	{	return lgammaf(x);	}
inline float lgamma_r(float x, int *signp)	{	return lgammaf_r(x, signp);	}
inline float log(float x)	{	return logf(x);	}
inline float log2(float x)	{	return log2f(x);	}
inline float log10(float x)	{	return log10f(x);	}
inline float log1p(float x)	{	return log1pf(x);	}
inline float logb(float x)	{	return logbf(x);	}
inline float mad(float a, float b, float c)	{	return a * b + c;	}
inline float maxmag(float x, float y)	{	return fmax(fabs(x), fabs(y));	}
inline float minmag(float x, float y)	{	return fmin(fabs(x), fabs(y));	}
inline float modf(float x, float *iptr)	{	return modff(x, iptr);	}
inline float nan(uint nancode)	{	return reinterpret_cast<const float&>(nancode |= 0x7F800000U);	}
inline float nextafter(float x, float y)	{	return nextafterf(x, y);	}
inline float pow(float x, float y)	{	return powf(x, y);	}
inline float pown(float x, int n)
{
	float _m = 1.0f;
	if (n < 0)
	{
		x = 1.0f / x;
		n = -n;
	}
	for( ; n > 0 ; n >>= 1)
	{
		if (n & 1)
			_m *= x;
	}
	return _m;
}
inline float powr(float x, float y)	{	return powf(x, y);	}
inline float remainder(float x, float y)	{	return remainderf(x, y);	}
inline float remquo(float x, float y, int *quo)	{	return remquof(x, y, quo);	}
inline float rint(float x)	{	return rintf(x);	}
inline float rootn(float x, int y)	{	return pow(x, 1.0f / y);	}
inline float round(float x)	{	return roundf(x);	}
inline float rsqrt(float x)	{	return 1.0f / sqrtf(x);	}
inline float sin(float x)	{	return sinf(x);	}
inline float sincos(float x, float *cosval)
{
#ifdef _GNU_SOURCE
	float _sin;
	sincosf(x, &_sin, cosval);
	return _sin;
#else
	*cosval = cos(x);
	return sin(x);
#endif
}
inline float sinh(float x)	{	return sinhf(x);	}
inline float sinpi(float x)	{	return sin(M_PI_F * x);	}
inline float sqrt(float x)	{	return sqrtf(x);	}
inline float tan(float x)	{	return tanf(x);	}
inline float tanh(float x)	{	return tanhf(x);	}
inline float tanpi(float x)	{	return tan(M_PI_F * x);	}
inline float tgamma(float x)	{	return tgammaf(x);	}
inline float trunc(float x)	{	return truncf(x);	}

// half_ versions
inline float half_cos(float x)	{	return cos(x);	}
inline float half_divide(float x, float y)	{	return x / y;	}
inline float half_exp(float x)	{	return exp(x);	}
inline float half_exp2(float x)	{	return exp2(x);	}
inline float half_exp10(float x)	{	return exp10(x);	}
inline float half_log(float x)	{	return log(x);	}
inline float half_log2(float x)	{	return log2(x);	}
inline float half_log10(float x)	{	return log10(x);	}
inline float half_powr(float x, float y)	{	return powr(x, y);	}
inline float half_recip(float x)	{	return 1.0f / x;	}
inline float half_rsqrt(float x)	{	return 1.0f / sqrt(x);	}
inline float half_sin(float x)	{	return sin(x);	}
inline float half_sqrt(float x)	{	return sqrt(x);	}
inline float half_tan(float x)	{	return tan(x);	}

// native_ versions
inline float native_cos(float x)	{	return cos(x);	}
inline float native_divide(float x, float y)	{	return x / y;	}
inline float native_exp(float x)	{	return exp(x);	}
inline float native_exp2(float x)	{	return exp2(x);	}
inline float native_exp10(float x)	{	return exp10(x);	}
inline float native_log(float x)	{	return log(x);	}
inline float native_log2(float x)	{	return log2(x);	}
inline float native_log10(float x)	{	return log10(x);	}
inline float native_powr(float x, float y)	{	return powr(x, y);	}
inline float native_recip(float x)	{	return 1.0f / x;	}
inline float native_rsqrt(float x)	{	return 1.0f / sqrt(x);	}
inline float native_sin(float x)	{	return sin(x);	}
inline float native_sqrt(float x)	{	return sqrt(x);	}
inline float native_tan(float x)	{	return tan(x);	}

// Vector versions
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(acos)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(acosh)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(acospi)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(asin)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(asinh)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(asinpi)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(atan)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(atan2)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(atanh)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(atanpi)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(atan2pi)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(cbrt)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(ceil)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(copysign)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(cos)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(cosh)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(cospi)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(erfc)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(erf)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(exp)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(exp2)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(exp10)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(expm1)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(fabs)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(fdim)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(floor)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3(fma)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(fmax)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(fmin)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(fmod)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2PB(fract)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2PI(frexp)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(hypot)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2I(ldexp)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(lgamma)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2PI(lgamma_r)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(log)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(log2)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(log10)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(log1p)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(logb)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION3(mad)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(maxmag)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(minmag)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2PB(modf)
inline float2 nan(uint2 nancode)	{	return float2::make(nan(nancode.v[0]), nan(nancode.v[1]));	}
inline float3 nan(uint3 nancode)	{	return float3::make(nan(nancode.v[0]), nan(nancode.v[1]), nan(nancode.v[2]));	}
inline float4 nan(uint4 nancode)	{	return float4::make(nan(nancode.v[0]), nan(nancode.v[1]), nan(nancode.v[2]), nan(nancode.v[3]));	}
inline float8 nan(uint8 nancode)	{	return float8::make(nan(nancode.v[0]), nan(nancode.v[1]), nan(nancode.v[2]), nan(nancode.v[3]),
															nan(nancode.v[4]), nan(nancode.v[5]), nan(nancode.v[6]), nan(nancode.v[7]));	}
inline float16 nan(uint16 nancode)	{	return float16::make(nan(nancode.v[0]), nan(nancode.v[1]), nan(nancode.v[2]), nan(nancode.v[3]),
															 nan(nancode.v[4]), nan(nancode.v[5]), nan(nancode.v[6]), nan(nancode.v[7]),
															 nan(nancode.v[8]), nan(nancode.v[9]), nan(nancode.v[10]), nan(nancode.v[11]),
															 nan(nancode.v[12]), nan(nancode.v[13]), nan(nancode.v[14]), nan(nancode.v[15]));	}
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(nextafter)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(pow)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2I(pown)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(powr)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(remainder)
template<class V> inline CHECK(V) remquo(const V &x, const V &y, const typename __vector_type<int, __vector<V>::components>::type &z)
{
	V ret;
	for(size_t i = 0 ; i < __vector<V>::components ; ++i)
		ret.v[i] = F(x.v[i], y.v[i], (int*)z + i);
	return ret;
}
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(rint)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2I(rootn)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(round)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(rsqrt)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(sin)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2PB(sincos)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(sinh)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(sinpi)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(sqrt)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(tan)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(tanh)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(tanpi)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(tgamma)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(trunc)

// half_ versions
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_cos)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(half_divide)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_exp)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_exp2)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_exp10)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_log)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_log2)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_log10)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(half_powr)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_recip)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_rsqrt)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_sin)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_sqrt)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(half_tan)

// native_ versions
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_cos)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(native_divide)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_exp)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_exp2)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_exp10)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_log)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_log2)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_log10)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION2(native_powr)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_recip)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_rsqrt)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_sin)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_sqrt)
VECTOR_IMPLEMENTATION_FROM_SCALAR_IMPLEMENTATION1(native_tan)

#endif
