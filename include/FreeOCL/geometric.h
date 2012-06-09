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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_GEOMETRIC_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_GEOMETRIC_H__
// Built-in geometric functions
// floats
inline __float4 cross(const __float4 &p0, const __float4 &p1)
{
	return __float4::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						  p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						  p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0],
						  0.0f);
}
inline __float3 cross(const __float3 &p0, const __float3 &p1)
{
	return __float3::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						  p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						  p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0]);
}

inline __float dot(const __float &p0, const __float &p1)
{	return p0 * p1;	}
inline __float dot(const __float2 &p0, const __float2 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1];	}

inline __float dot(const __float3 &p0, const __float3 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2];	}
//inline __float dot(const __float4 &p0, const __float4 &p1)
//{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2] + p0.v[3] * p1.v[3];	}
inline __float dot(const __float4 &p0, const __float4 &p1)
{
	const __float4 &tmp = p0 * p1;
	return tmp.v[0] + tmp.v[1] + tmp.v[2] + tmp.v[3];
}

inline __float length(const __float &p)		{	return sqrt(dot(p,p));	}
inline __float length(const __float2 &p)	{	return sqrt(dot(p,p));	}
inline __float length(const __float3 &p)	{	return sqrt(dot(p,p));	}
inline __float length(const __float4 &p)	{	return sqrt(dot(p,p));	}

inline __float distance(const __float  &p0, const __float  &p1)	{	return length(p0 - p1);	}
inline __float distance(const __float2 &p0, const __float2 &p1)	{	return length(p0 - p1);	}
inline __float distance(const __float3 &p0, const __float3 &p1)	{	return length(p0 - p1);	}
inline __float distance(const __float4 &p0, const __float4 &p1)	{	return length(p0 - p1);	}

inline __float  normalize(const __float  &p)	{	return (1.0f / length(p)) * p;	}
inline __float2 normalize(const __float2 &p)	{	return (1.0f / length(p)) * p;	}
inline __float3 normalize(const __float3 &p)	{	return (1.0f / length(p)) * p;	}
#if defined __FAST_RELAXED_MATH__ && defined __SSE4_1__
inline __float4 normalize(const __float4 &v)
{
	const __m128 _v = reinterpret_cast<const __m128&>(v);
	const __m128 s = _mm_mul_ps(_v, _mm_rsqrt_ps(_mm_dp_ps(_v,_v,0xFF)));
	return reinterpret_cast<const __float4&>(s);
}
#else
inline __float4 normalize(const __float4 &p)	{	return (1.0f / length(p)) * p;	}
#endif

inline __float fast_length(const __float  &p)	{	return half_sqrt(dot(p,p));	}
inline __float fast_length(const __float2 &p)	{	return half_sqrt(dot(p,p));	}
inline __float fast_length(const __float3 &p)	{	return half_sqrt(dot(p,p));	}
inline __float fast_length(const __float4 &p)	{	return half_sqrt(dot(p,p));	}

inline __float fast_distance(const __float  &p0, const __float  &p1)	{	return fast_length(p0 - p1);	}
inline __float fast_distance(const __float2 &p0, const __float2 &p1)	{	return fast_length(p0 - p1);	}
inline __float fast_distance(const __float3 &p0, const __float3 &p1)	{	return fast_length(p0 - p1);	}
inline __float fast_distance(const __float4 &p0, const __float4 &p1)	{	return fast_length(p0 - p1);	}

inline __float  fast_normalize(const __float  &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline __float2 fast_normalize(const __float2 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline __float3 fast_normalize(const __float3 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
#ifdef __SSE4_1__
inline __float4 fast_normalize(const __float4 &p)
{
	const __m128 v = reinterpret_cast<const __m128&>(p);
	const __m128 s = _mm_mul_ps(v, _mm_rsqrt_ps(_mm_dp_ps(v,v,0xFF)));
	return reinterpret_cast<const __float4&>(s);
}
#else
inline __float4 fast_normalize(const __float4 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
#endif
// doubles
inline __double4 cross(const __double4 &p0, const __double4 &p1)
{
	return __double4::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						   p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						   p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0],
						   0.0f);
}
inline __double3 cross(const __double3 &p0, const __double3 &p1)
{
	return __double3::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						   p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						   p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0]);
}

inline __double dot(const __double &p0, const __double &p1)
{	return p0 * p1;	}

inline __double dot(const __double2 &p0, const __double2 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1];	}

inline __double dot(const __double3 &p0, const __double3 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2];	}

inline __double dot(const __double4 &p0, const __double4 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2] + p0.v[3] * p1.v[3];	}

inline __double length(const __double &p)	{	return sqrt(dot(p,p));	}
inline __double length(const __double2 &p)	{	return sqrt(dot(p,p));	}
inline __double length(const __double3 &p)	{	return sqrt(dot(p,p));	}
inline __double length(const __double4 &p)	{	return sqrt(dot(p,p));	}

inline __double distance(const __double &p0, const __double &p1)		{	return length(p0 - p1);	}
inline __double distance(const __double2 &p0, const __double2 &p1)	{	return length(p0 - p1);	}
inline __double distance(const __double3 &p0, const __double3 &p1)	{	return length(p0 - p1);	}
inline __double distance(const __double4 &p0, const __double4 &p1)	{	return length(p0 - p1);	}

inline __double normalize(const __double &p)	{	return (1.0 / length(p)) * p;	}
inline __double2 normalize(const __double2 &p)	{	return (1.0 / length(p)) * p;	}
inline __double3 normalize(const __double3 &p)	{	return (1.0 / length(p)) * p;	}
inline __double4 normalize(const __double4 &p)	{	return (1.0 / length(p)) * p;	}
#endif
