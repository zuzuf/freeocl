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

/* cl_channel_order */
#define CLK_R			0x10B0
#define CLK_A			0x10B1
#define CLK_RG			0x10B2
#define CLK_RA			0x10B3
#define CLK_RGB			0x10B4
#define CLK_RGBA		0x10B5
#define CLK_BGRA		0x10B6
#define CLK_ARGB		0x10B7
#define CLK_INTENSITY	0x10B8
#define CLK_LUMINANCE	0x10B9
#define CLK_Rx			0x10BA
#define CLK_RGx			0x10BB
#define CLK_RGBx		0x10BC

/* cl_channel_type */
#define CLK_SNORM_INT8			0x10D0
#define CLK_SNORM_INT16			0x10D1
#define CLK_UNORM_INT8			0x10D2
#define CLK_UNORM_INT16			0x10D3
#define CLK_UNORM_SHORT_565		0x10D4
#define CLK_UNORM_SHORT_555		0x10D5
#define CLK_UNORM_INT_101010	0x10D6
#define CLK_SIGNED_INT8			0x10D7
#define CLK_SIGNED_INT16		0x10D8
#define CLK_SIGNED_INT32		0x10D9
#define CLK_UNSIGNED_INT8		0x10DA
#define CLK_UNSIGNED_INT16		0x10DB
#define CLK_UNSIGNED_INT32		0x10DC
#define CLK_HALF_FLOAT			0x10DD
#define CLK_FLOAT				0x10DE

struct image2d_t
{
	uint channel_order;
	uint channel_data_type;
	size_t width, height;
	size_t row_pitch;
	void *data;
};

struct image3d_t
{
	uint channel_order;
	uint channel_data_type;
	size_t width, height, depth;
	size_t row_pitch, slice_pitch;
	void *data;
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

inline int get_image_width(const image2d_t &image)	{	return image.width;	}
inline int get_image_width(const image3d_t &image)	{	return image.width;	}
inline int get_image_height(const image2d_t &image)	{	return image.height;	}
inline int get_image_height(const image3d_t &image)	{	return image.height;	}
inline int get_image_depth(const image3d_t &image)	{	return image.depth;	}

inline int get_image_channel_data_type(const image2d_t &image)	{	return image.channel_data_type;	}
inline int get_image_channel_data_type(const image3d_t &image)	{	return image.channel_data_type;	}

inline int get_image_channel_order(const image2d_t &image)	{	return image.channel_order;	}
inline int get_image_channel_order(const image3d_t &image)	{	return image.channel_order;	}

inline const int2 &get_image_dim(const image2d_t &image)
{
	return int2::make(image.width, image.height);
}
inline const int4 &get_image_dim(const image3d_t &image)
{
	return int4::make(image.width, image.height, image.depth, 0);
}

#endif
