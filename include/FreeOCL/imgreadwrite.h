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
#ifndef __FREEOCL_OPENCL_C_PREINCLUDE_IMGREADWRITE_H__
#define __FREEOCL_OPENCL_C_PREINCLUDE_IMGREADWRITE_H__

typedef uint sampler_t;

#define CLK_NORMALIZED_COORDS_FALSE		0x00000000U
#define CLK_NORMALIZED_COORDS_TRUE		0x01000000U

#define CLK_ADDRESS_NONE				0x00000000U
#define CLK_ADDRESS_CLAMP_TO_EDGE		0x00000001U
#define CLK_ADDRESS_REPEAT				0x00000002U
#define CLK_ADDRESS_CLAMP				0x00000003U
#define CLK_ADDRESS_MIRRORED_REPEAT		0x00000004U

#define CLK_FILTER_NEAREST				0x00000000U
#define CLK_FILTER_LINEAR				0x00010000U

struct image2d_t
{

};

struct image3d_t
{

};

// Built-in image read/write functions
inline float4 read_imagef(const image2d_t &image, sampler_t sampler, const int2 &coord)	{}
inline float4 read_imagef(const image2d_t &image, sampler_t sampler, const float2 &coord)	{}

inline int4 read_imagei(const image2d_t &image, sampler_t sampler, const int2 &coord)	{}
inline int4 read_imagei(const image2d_t &image, sampler_t sampler, const float2 &coord)	{}

inline uint4 read_imageui(const image2d_t &image, sampler_t sampler, const int2 &coord)	{}
inline uint4 read_imageui(const image2d_t &image, sampler_t sampler, const float2 &coord)	{}

inline void write_imagef(image2d_t &image, const int2 &coord, const float4 &color)	{}
inline void write_imagei(image2d_t &image, const int2 &coord, const int4 &color)	{}
inline void write_imageui(image2d_t &image, const int2 &coord, const uint4 &color)	{}

inline float4 read_imagef(const image3d_t &image, sampler_t sampler, const int4 &coord)	{}
inline float4 read_imagef(const image3d_t &image, sampler_t sampler, const float4 &coord)	{}

inline int4 read_imagei(const image3d_t &image, sampler_t sampler, const int4 &coord)	{}
inline int4 read_imagei(const image3d_t &image, sampler_t sampler, const float4 &coord)	{}

inline uint4 read_imageui(const image3d_t &image, sampler_t sampler, const int4 &coord)	{}
inline uint4 read_imageui(const image3d_t &image, sampler_t sampler, const float4 &coord)	{}

inline void write_imagef(image3d_t &image, const int4 &coord, const float4 &color)	{}
inline void write_imagei(image3d_t &image, const int4 &coord, const int4 &color)	{}
inline void write_imageui(image3d_t &image, const int4 &coord, const uint4 &color)	{}

inline int get_image_width(const image2d_t &image)	{}
inline int get_image_width(const image3d_t &image)	{}
inline int get_image_height(const image2d_t &image)	{}
inline int get_image_height(const image3d_t &image)	{}
inline int get_image_depth(const image3d_t &image)	{}

inline int get_image_channel_data_type(const image2d_t &image)	{}
inline int get_image_channel_data_type(const image3d_t &image)	{}

inline int get_image_channel_order(const image2d_t &image)	{}
inline int get_image_channel_order(const image3d_t &image)	{}

inline const int2 &get_image_dim(const image2d_t &image)	{}
inline const int4 &get_image_dim(const image3d_t &image)	{}

#endif
