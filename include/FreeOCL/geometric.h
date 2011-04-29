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

inline float4 cross(const float4 &p0, const float4 &p1)
{
	return (float4)(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
					p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
					p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0],
					0.0f);
}
inline float3 cross(const float3 &p0, const float3 &p1)
{
	return (float3)(p0.v[1] * p1.v[2] - p0.v[2] * p1.v[1],
					p0.v[2] * p1.v[0] - p0.v[0] * p1.v[2],
					p0.v[0] * p1.v[1] - p0.v[1] * p1.v[0]);
}

inline float dot(const float2 &p0, const float2 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1];	}

inline float dot(const float3 &p0, const float3 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2];	}

inline float dot(const float4 &p0, const float4 &p1)
{	return p0.v[0] * p1.v[0] + p0.v[1] * p1.v[1] + p0.v[2] * p1.v[2] + p0.v[3] * p1.v[3];	}

inline float dot(const float8 &p0, const float8 &p1)
{
	return p0.v[0] * p1.v[0]
			+ p0.v[1] * p1.v[1]
			+ p0.v[2] * p1.v[2]
			+ p0.v[3] * p1.v[3]
			+ p0.v[4] * p1.v[4]
			+ p0.v[5] * p1.v[5]
			+ p0.v[6] * p1.v[6]
			+ p0.v[7] * p1.v[7];
}

inline float dot(const float16 &p0, const float16 &p1)
{
	return p0.v[0] * p1.v[0]
			+ p0.v[1] * p1.v[1]
			+ p0.v[2] * p1.v[2]
			+ p0.v[3] * p1.v[3]
			+ p0.v[4] * p1.v[4]
			+ p0.v[5] * p1.v[5]
			+ p0.v[6] * p1.v[6]
			+ p0.v[7] * p1.v[7]
			+ p0.v[8] * p1.v[8]
			+ p0.v[9] * p1.v[9]
			+ p0.v[10] * p1.v[10]
			+ p0.v[11] * p1.v[11]
			+ p0.v[12] * p1.v[12]
			+ p0.v[13] * p1.v[13]
			+ p0.v[14] * p1.v[14]
			+ p0.v[15] * p1.v[15];
}

inline float length(const float2 &p)	{	return sqrt(dot(p,p));	}
inline float length(const float3 &p)	{	return sqrt(dot(p,p));	}
inline float length(const float4 &p)	{	return sqrt(dot(p,p));	}
inline float length(const float8 &p)	{	return sqrt(dot(p,p));	}
inline float length(const float16 &p)	{	return sqrt(dot(p,p));	}

inline float distance(const float2 &p0, const float2 &p1)	{	return length(p0 - p1);	}
inline float distance(const float3 &p0, const float3 &p1)	{	return length(p0 - p1);	}
inline float distance(const float4 &p0, const float4 &p1)	{	return length(p0 - p1);	}
inline float distance(const float8 &p0, const float8 &p1)	{	return length(p0 - p1);	}
inline float distance(const float16 &p0, const float16 &p1)	{	return length(p0 - p1);	}

inline float2 normalize(const float2 &p)	{	return (1.0f / length(p)) * p;	}
inline float3 normalize(const float3 &p)	{	return (1.0f / length(p)) * p;	}
inline float4 normalize(const float4 &p)	{	return (1.0f / length(p)) * p;	}
inline float8 normalize(const float8 &p)	{	return (1.0f / length(p)) * p;	}
inline float16 normalize(const float16 &p)	{	return (1.0f / length(p)) * p;	}

inline float fast_length(const float2 &p)	{	return half_sqrt(dot(p,p));	}
inline float fast_length(const float3 &p)	{	return half_sqrt(dot(p,p));	}
inline float fast_length(const float4 &p)	{	return half_sqrt(dot(p,p));	}
inline float fast_length(const float8 &p)	{	return half_sqrt(dot(p,p));	}
inline float fast_length(const float16 &p)	{	return half_sqrt(dot(p,p));	}

inline float fast_distance(const float2 &p0, const float2 &p1)	{	return fast_length(p0 - p1);	}
inline float fast_distance(const float3 &p0, const float3 &p1)	{	return fast_length(p0 - p1);	}
inline float fast_distance(const float4 &p0, const float4 &p1)	{	return fast_length(p0 - p1);	}
inline float fast_distance(const float8 &p0, const float8 &p1)	{	return fast_length(p0 - p1);	}
inline float fast_distance(const float16 &p0, const float16 &p1)	{	return fast_length(p0 - p1);	}

inline float2 fast_normalize(const float2 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline float3 fast_normalize(const float3 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline float4 fast_normalize(const float4 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline float8 fast_normalize(const float8 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
inline float16 fast_normalize(const float16 &p)	{	return half_rsqrt(dot(p,p)) * p;	}
#endif
