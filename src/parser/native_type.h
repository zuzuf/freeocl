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
#ifndef __FREEOCL_PARSER_NATIVE_TYPE_H__
#define __FREEOCL_PARSER_NATIVE_TYPE_H__

#include "type.h"

namespace FreeOCL
{
	class native_type : public type
	{
	public:
		enum type_id {
			VOID, BOOL, HALF, SIZE_T, SAMPLER_T, EVENT_T,
			IMAGE1D_T, IMAGE1D_BUFFER_T, IMAGE1D_ARRAY_T, IMAGE2D_T, IMAGE2D_ARRAY_T, IMAGE3D_T,
			CHAR, SHORT, INT, LONG, UCHAR, USHORT, UINT, ULONG, FLOAT, DOUBLE,
			CHAR2, SHORT2, INT2, LONG2, UCHAR2, USHORT2, UINT2, ULONG2, FLOAT2, DOUBLE2,
			CHAR3, SHORT3, INT3, LONG3, UCHAR3, USHORT3, UINT3, ULONG3, FLOAT3, DOUBLE3,
			CHAR4, SHORT4, INT4, LONG4, UCHAR4, USHORT4, UINT4, ULONG4, FLOAT4, DOUBLE4,
			CHAR8, SHORT8, INT8, LONG8, UCHAR8, USHORT8, UINT8, ULONG8, FLOAT8, DOUBLE8,
			CHAR16, SHORT16, INT16, LONG16, UCHAR16, USHORT16, UINT16, ULONG16, FLOAT16, DOUBLE16,
			HALF2, HALF3, HALF4, HALF8, HALF16
		};
	public:
		native_type(const type_id id, const bool b_const, const address_space addr_space) : type(b_const, addr_space), id(id)	{}
		virtual bool operator==(const type &type) const;
		virtual void write(std::ostream &out) const;
		virtual std::string get_name() const;
		virtual std::string get_cxx_name() const;
		virtual smartptr<type> clone(const bool b_const, const address_space addr_space) const;
        virtual const char *get_node_type() const;

		type_id get_type_id() const	{	return id;	}
		int get_dim() const;
		bool is_vector() const	{	return get_dim() > 1;	}
		bool is_scalar() const	{	return get_dim() == 1;	}
        bool is_special() const;
        bool is_integer() const;
		bool is_floatting() const;
		bool is_half() const;
		bool is_float() const;
		bool is_double() const;
		bool is_long() const;
		bool is_ulong() const;
		bool is_uint() const;
		bool is_signed() const;
		type_id get_scalar_type() const;
	private:
		const type_id id;

	public:
		static smartptr<type> t_event_t;
		static smartptr<type> t_sampler_t;
		static smartptr<type> t_image1d_t;
		static smartptr<type> t_image1d_buffer_t;
		static smartptr<type> t_image1d_array_t;
		static smartptr<type> t_image2d_t;
		static smartptr<type> t_image2d_array_t;
		static smartptr<type> t_image3d_t;
		static smartptr<type> t_void;
		static smartptr<type> t_bool;
		static smartptr<type> t_half;
		static smartptr<type> t_half2;
		static smartptr<type> t_half3;
		static smartptr<type> t_half4;
		static smartptr<type> t_half8;
		static smartptr<type> t_half16;
		static smartptr<type> t_size_t;
		static smartptr<type> t_char;
		static smartptr<type> t_short;
		static smartptr<type> t_int;
		static smartptr<type> t_long;
		static smartptr<type> t_uchar;
		static smartptr<type> t_ushort;
		static smartptr<type> t_uint;
		static smartptr<type> t_ulong;
		static smartptr<type> t_float;
		static smartptr<type> t_double;
		static smartptr<type> t_char2;
		static smartptr<type> t_short2;
		static smartptr<type> t_int2;
		static smartptr<type> t_long2;
		static smartptr<type> t_uchar2;
		static smartptr<type> t_ushort2;
		static smartptr<type> t_uint2;
		static smartptr<type> t_ulong2;
		static smartptr<type> t_float2;
		static smartptr<type> t_double2;
		static smartptr<type> t_char3;
		static smartptr<type> t_short3;
		static smartptr<type> t_int3;
		static smartptr<type> t_long3;
		static smartptr<type> t_uchar3;
		static smartptr<type> t_ushort3;
		static smartptr<type> t_uint3;
		static smartptr<type> t_ulong3;
		static smartptr<type> t_float3;
		static smartptr<type> t_double3;
		static smartptr<type> t_char4;
		static smartptr<type> t_short4;
		static smartptr<type> t_int4;
		static smartptr<type> t_long4;
		static smartptr<type> t_uchar4;
		static smartptr<type> t_ushort4;
		static smartptr<type> t_uint4;
		static smartptr<type> t_ulong4;
		static smartptr<type> t_float4;
		static smartptr<type> t_double4;
		static smartptr<type> t_char8;
		static smartptr<type> t_short8;
		static smartptr<type> t_int8;
		static smartptr<type> t_long8;
		static smartptr<type> t_uchar8;
		static smartptr<type> t_ushort8;
		static smartptr<type> t_uint8;
		static smartptr<type> t_ulong8;
		static smartptr<type> t_float8;
		static smartptr<type> t_double8;
		static smartptr<type> t_char16;
		static smartptr<type> t_short16;
		static smartptr<type> t_int16;
		static smartptr<type> t_long16;
		static smartptr<type> t_uchar16;
		static smartptr<type> t_ushort16;
		static smartptr<type> t_uint16;
		static smartptr<type> t_ulong16;
		static smartptr<type> t_float16;
		static smartptr<type> t_double16;
		static smartptr<type> get_int_for_dim(int dim);
		static smartptr<type> get_long_for_dim(int dim);
		static smartptr<type> get_short_for_dim(int dim);

		static size_t get_dim_for(int id);
		static smartptr<type> make_vector_type(int base, int dim);
	};
}

#endif
