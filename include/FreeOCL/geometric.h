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
inline float4 cross(const float4 &p0, const float4 &p1)
{
	return float4::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0],
						0.0f);
}
inline float3 cross(const float3 &p0, const float3 &p1)
{
	return float3::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0]);
}

inline float dot(const float &p0, const float &p1)
{	return p0 * p1;	}
inline float dot(const float2 &p0, const float2 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1];	}

inline float dot(const float3 &p0, const float3 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2];	}
//inline float dot(const float4 &p0, const float4 &p1)
//{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2] + p0.v[3] * p1.v[3];	}
inline float dot(const float4 &p0, const float4 &p1)
{
	const float4 &tmp = p0 * p1;
	return tmp.v[0] + tmp.v[1] + tmp.v[2] + tmp.v[3];
}

inline float length(const float &p)		{	return sqrt(dot(p,p));	}
inline float length(const float2 &p)	{	return sqrt(dot(p,p));	}
inline float length(const float3 &p)	{	return sqrt(dot(p,p));	}
inline float length(const float4 &p)	{	return sqrt(dot(p,p));	}

inline float distance(const float  &p0, const float  &p1)	{	return length(p0 - p1);	}
inline float distance(const float2 &p0, const float2 &p1)	{	return length(p0 - p1);	}
inline float distance(const float3 &p0, const float3 &p1)	{	return length(p0 - p1);	}
inline float distance(const float4 &p0, const float4 &p1)	{	return length(p0 - p1);	}

inline float  normalize(const float  &p)	{	return (1.0f / length(p)) * p;	}
inline float2 normalize(const float2 &p)	{	return (1.0f / length(p)) * p;	}
inline float3 normalize(const float3 &p)	{	return (1.0f / length(p)) * p;	}
#if defined __FAST_RELAXED_MATH__ && defined __SSE4_1__
inline float4 normalize(const float4 &v)
{
	const __m128 _v = reinterpret_cast<const __m128&>(v);
	const __m128 s = _mm_mul_ps(_v, _mm_rsqrt_ps(_mm_dp_ps(_v,_v,0xFF)));
	return reinterpret_cast<const float4&>(s);
}
#else
inline float4 normalize(const float4 &p)	{	return (1.0f / length(p)) * p;	}
#endif

inline float fast_length(const float  &p)	{	return half_sqrt(dot(p,p));	}
inline float fast_length(const float2 &p)	{	return half_sqrt(dot(p,p));	}
inline float fast_length(const float3 &p)	{	return half_sqrt(dot(p,p));	}
inline float fast_length(const float4 &p)	{	return half_sqrt(dot(p,p));	}

inline float fast_distance(const float  &p0, const float  &p1)	{	return fast_length(p0 - p1);	}
inline float fast_distance(const float2 &p0, const float2 &p1)	{	return fast_length(p0 - p1);	}
inline float fast_distance(const float3 &p0, const float3 &p1)	{	return fast_length(p0 - p1);	}
inline float fast_distance(const float4 &p0, const float4 &p1)	{	return fast_length(p0 - p1);	}

inline float  fast_normalize(const float  &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline float2 fast_normalize(const float2 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline float3 fast_normalize(const float3 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
#ifdef __SSE4_1__
inline float4 fast_normalize(const float4 &p)
{
	const __m128 v = reinterpret_cast<const __m128&>(p);
	const __m128 s = _mm_mul_ps(v, _mm_rsqrt_ps(_mm_dp_ps(v,v,0xFF)));
	return reinterpret_cast<const float4&>(s);
}
#else
inline float4 fast_normalize(const float4 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
#endif
// doubles
inline double4 cross(const double4 &p0, const double4 &p1)
{
	return double4::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0],
						0.0f);
}
inline double3 cross(const double3 &p0, const double3 &p1)
{
	return double3::make(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
						p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
						p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0]);
}

inline double dot(const double &p0, const double &p1)
{	return p0 * p1;	}

inline double dot(const double2 &p0, const double2 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1];	}

inline double dot(const double3 &p0, const double3 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2];	}

inline double dot(const double4 &p0, const double4 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2] + p0.v[3] * p1.v[3];	}

inline double length(const double &p)	{	return sqrt(dot(p,p));	}
inline double length(const double2 &p)	{	return sqrt(dot(p,p));	}
inline double length(const double3 &p)	{	return sqrt(dot(p,p));	}
inline double length(const double4 &p)	{	return sqrt(dot(p,p));	}

inline double distance(const double &p0, const double &p1)		{	return length(p0 - p1);	}
inline double distance(const double2 &p0, const double2 &p1)	{	return length(p0 - p1);	}
inline double distance(const double3 &p0, const double3 &p1)	{	return length(p0 - p1);	}
inline double distance(const double4 &p0, const double4 &p1)	{	return length(p0 - p1);	}

inline double normalize(const double &p)	{	return (1.0 / length(p)) * p;	}
inline double2 normalize(const double2 &p)	{	return (1.0 / length(p)) * p;	}
inline double3 normalize(const double3 &p)	{	return (1.0 / length(p)) * p;	}
inline double4 normalize(const double4 &p)	{	return (1.0 / length(p)) * p;	}
#endif
