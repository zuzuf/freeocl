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
#include "native_type.h"

namespace FreeOCL
{
	smartptr<type> native_type::t_image2d_t(new native_type(native_type::IMAGE2D_T, true, type::CONSTANT));
	smartptr<type> native_type::t_image3d_t(new native_type(native_type::IMAGE3D_T, true, type::CONSTANT));
	smartptr<type> native_type::t_sampler_t(new native_type(native_type::SAMPLER_T, true, type::CONSTANT));
	smartptr<type> native_type::t_event_t(new native_type(native_type::EVENT_T, true, type::CONSTANT));
	smartptr<type> native_type::t_void(new native_type(native_type::VOID, true, type::CONSTANT));
	smartptr<type> native_type::t_bool(new native_type(native_type::BOOL, true, type::CONSTANT));
	smartptr<type> native_type::t_half(new native_type(native_type::HALF, true, type::CONSTANT));
	smartptr<type> native_type::t_half2(new native_type(native_type::HALF2, true, type::CONSTANT));
	smartptr<type> native_type::t_half3(new native_type(native_type::HALF3, true, type::CONSTANT));
	smartptr<type> native_type::t_half4(new native_type(native_type::HALF4, true, type::CONSTANT));
	smartptr<type> native_type::t_half8(new native_type(native_type::HALF8, true, type::CONSTANT));
	smartptr<type> native_type::t_half16(new native_type(native_type::HALF16, true, type::CONSTANT));
	smartptr<type> native_type::t_size_t(new native_type(native_type::SIZE_T, true, type::CONSTANT));
	smartptr<type> native_type::t_char(new native_type(native_type::CHAR, true, type::CONSTANT));
	smartptr<type> native_type::t_short(new native_type(native_type::SHORT, true, type::CONSTANT));
	smartptr<type> native_type::t_int(new native_type(native_type::INT, true, type::CONSTANT));
	smartptr<type> native_type::t_long(new native_type(native_type::LONG, true, type::CONSTANT));
	smartptr<type> native_type::t_uchar(new native_type(native_type::UCHAR, true, type::CONSTANT));
	smartptr<type> native_type::t_ushort(new native_type(native_type::USHORT, true, type::CONSTANT));
	smartptr<type> native_type::t_uint(new native_type(native_type::UINT, true, type::CONSTANT));
	smartptr<type> native_type::t_ulong(new native_type(native_type::ULONG, true, type::CONSTANT));
	smartptr<type> native_type::t_float(new native_type(native_type::FLOAT, true, type::CONSTANT));
	smartptr<type> native_type::t_double(new native_type(native_type::DOUBLE, true, type::CONSTANT));
	smartptr<type> native_type::t_char2(new native_type(native_type::CHAR2, true, type::CONSTANT));
	smartptr<type> native_type::t_short2(new native_type(native_type::SHORT2, true, type::CONSTANT));
	smartptr<type> native_type::t_int2(new native_type(native_type::INT2, true, type::CONSTANT));
	smartptr<type> native_type::t_long2(new native_type(native_type::LONG2, true, type::CONSTANT));
	smartptr<type> native_type::t_uchar2(new native_type(native_type::UCHAR2, true, type::CONSTANT));
	smartptr<type> native_type::t_ushort2(new native_type(native_type::USHORT2, true, type::CONSTANT));
	smartptr<type> native_type::t_uint2(new native_type(native_type::UINT2, true, type::CONSTANT));
	smartptr<type> native_type::t_ulong2(new native_type(native_type::ULONG2, true, type::CONSTANT));
	smartptr<type> native_type::t_float2(new native_type(native_type::FLOAT2, true, type::CONSTANT));
	smartptr<type> native_type::t_double2(new native_type(native_type::DOUBLE2, true, type::CONSTANT));
	smartptr<type> native_type::t_char3(new native_type(native_type::CHAR3, true, type::CONSTANT));
	smartptr<type> native_type::t_short3(new native_type(native_type::SHORT3, true, type::CONSTANT));
	smartptr<type> native_type::t_int3(new native_type(native_type::INT3, true, type::CONSTANT));
	smartptr<type> native_type::t_long3(new native_type(native_type::LONG3, true, type::CONSTANT));
	smartptr<type> native_type::t_uchar3(new native_type(native_type::UCHAR3, true, type::CONSTANT));
	smartptr<type> native_type::t_ushort3(new native_type(native_type::USHORT3, true, type::CONSTANT));
	smartptr<type> native_type::t_uint3(new native_type(native_type::UINT3, true, type::CONSTANT));
	smartptr<type> native_type::t_ulong3(new native_type(native_type::ULONG3, true, type::CONSTANT));
	smartptr<type> native_type::t_float3(new native_type(native_type::FLOAT3, true, type::CONSTANT));
	smartptr<type> native_type::t_double3(new native_type(native_type::DOUBLE3, true, type::CONSTANT));
	smartptr<type> native_type::t_char4(new native_type(native_type::CHAR4, true, type::CONSTANT));
	smartptr<type> native_type::t_short4(new native_type(native_type::SHORT4, true, type::CONSTANT));
	smartptr<type> native_type::t_int4(new native_type(native_type::INT4, true, type::CONSTANT));
	smartptr<type> native_type::t_long4(new native_type(native_type::LONG4, true, type::CONSTANT));
	smartptr<type> native_type::t_uchar4(new native_type(native_type::UCHAR4, true, type::CONSTANT));
	smartptr<type> native_type::t_ushort4(new native_type(native_type::USHORT4, true, type::CONSTANT));
	smartptr<type> native_type::t_uint4(new native_type(native_type::UINT4, true, type::CONSTANT));
	smartptr<type> native_type::t_ulong4(new native_type(native_type::ULONG4, true, type::CONSTANT));
	smartptr<type> native_type::t_float4(new native_type(native_type::FLOAT4, true, type::CONSTANT));
	smartptr<type> native_type::t_double4(new native_type(native_type::DOUBLE4, true, type::CONSTANT));
	smartptr<type> native_type::t_char8(new native_type(native_type::CHAR8, true, type::CONSTANT));
	smartptr<type> native_type::t_short8(new native_type(native_type::SHORT8, true, type::CONSTANT));
	smartptr<type> native_type::t_int8(new native_type(native_type::INT8, true, type::CONSTANT));
	smartptr<type> native_type::t_long8(new native_type(native_type::LONG8, true, type::CONSTANT));
	smartptr<type> native_type::t_uchar8(new native_type(native_type::UCHAR8, true, type::CONSTANT));
	smartptr<type> native_type::t_ushort8(new native_type(native_type::USHORT8, true, type::CONSTANT));
	smartptr<type> native_type::t_uint8(new native_type(native_type::UINT8, true, type::CONSTANT));
	smartptr<type> native_type::t_ulong8(new native_type(native_type::ULONG8, true, type::CONSTANT));
	smartptr<type> native_type::t_float8(new native_type(native_type::FLOAT8, true, type::CONSTANT));
	smartptr<type> native_type::t_double8(new native_type(native_type::DOUBLE8, true, type::CONSTANT));
	smartptr<type> native_type::t_char16(new native_type(native_type::CHAR16, true, type::CONSTANT));
	smartptr<type> native_type::t_short16(new native_type(native_type::SHORT16, true, type::CONSTANT));
	smartptr<type> native_type::t_int16(new native_type(native_type::INT16, true, type::CONSTANT));
	smartptr<type> native_type::t_long16(new native_type(native_type::LONG16, true, type::CONSTANT));
	smartptr<type> native_type::t_uchar16(new native_type(native_type::UCHAR16, true, type::CONSTANT));
	smartptr<type> native_type::t_ushort16(new native_type(native_type::USHORT16, true, type::CONSTANT));
	smartptr<type> native_type::t_uint16(new native_type(native_type::UINT16, true, type::CONSTANT));
	smartptr<type> native_type::t_ulong16(new native_type(native_type::ULONG16, true, type::CONSTANT));
	smartptr<type> native_type::t_float16(new native_type(native_type::FLOAT16, true, type::CONSTANT));
	smartptr<type> native_type::t_double16(new native_type(native_type::DOUBLE16, true, type::CONSTANT));

	bool native_type::operator==(const type &type) const
	{
		const native_type *p_type = dynamic_cast<const native_type*>(&type);
		return p_type != NULL && p_type->id == id && is_const() == p_type->is_const();
	}

	std::string native_type::get_name() const
	{
		static const char *type_name[] = {
			"void", "bool", "half", "size_t", "sampler_t", "image2d_t", "image3d_t", "event_t",
			"char", "short", "int", "long", "uchar", "ushort", "uint", "ulong", "float", "double",
			"char2", "short2", "int2", "long2", "uchar2", "ushort2", "uint2", "ulong2", "float2", "double2",
			"char3", "short3", "int3", "long3", "uchar3", "ushort3", "uint3", "ulong3", "float3", "double3",
			"char4", "short4", "int4", "long4", "uchar4", "ushort4", "uint4", "ulong4", "float4", "double4",
			"char8", "short8", "int8", "long8", "uchar8", "ushort8", "uint8", "ulong8", "float8", "double8",
			"char16", "short16", "int16", "long16", "uchar16", "ushort16", "uint16", "ulong16", "float16", "double16",
			"half2", "half3", "half4", "half8", "half16"
		};
		std::string prefix;
		switch(get_address_space())
		{
		case PRIVATE:	break;
		case GLOBAL:	prefix = "__global ";	break;
		case LOCAL:		prefix = "__local ";	break;
		case CONSTANT:	prefix = "__constant ";	break;
		}
		if (is_const() && get_address_space() != CONSTANT)
			return prefix + std::string("const ") + type_name[id];
		return prefix + type_name[id];
	}

	void native_type::write(std::ostream &out) const
	{
		out << get_name() << ' ';
	}

	int native_type::get_dim() const
	{
		return get_dim_for(id);
	}

	size_t native_type::get_dim_for(int id)
	{
		static size_t type_dim[] = {
			0, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
			2, 3, 4, 8, 16
		};
		return type_dim[id];
	}

	smartptr<type> native_type::clone(const bool b_const, const address_space addr_space) const
	{
		return new native_type(id, b_const, addr_space);
	}

	bool native_type::is_integer() const
	{
		switch(id)
		{
		case BOOL:
		case SIZE_T:
		case CHAR:
		case SHORT:
		case INT:
		case LONG:
		case UCHAR:
		case USHORT:
		case UINT:
		case ULONG:
		case CHAR2:
		case SHORT2:
		case INT2:
		case LONG2:
		case UCHAR2:
		case USHORT2:
		case UINT2:
		case ULONG2:
		case CHAR3:
		case SHORT3:
		case INT3:
		case LONG3:
		case UCHAR3:
		case USHORT3:
		case UINT3:
		case ULONG3:
		case CHAR4:
		case SHORT4:
		case INT4:
		case LONG4:
		case UCHAR4:
		case USHORT4:
		case UINT4:
		case ULONG4:
		case CHAR8:
		case SHORT8:
		case INT8:
		case LONG8:
		case UCHAR8:
		case USHORT8:
		case UINT8:
		case ULONG8:
		case CHAR16:
		case SHORT16:
		case INT16:
		case LONG16:
		case UCHAR16:
		case USHORT16:
		case UINT16:
		case ULONG16:
			return true;
		default:
			return false;
		}
	}

	bool native_type::is_floatting() const
	{
		switch(id)
		{
		case HALF:
		case FLOAT:
		case DOUBLE:
		case HALF2:
		case FLOAT2:
		case DOUBLE2:
		case HALF3:
		case FLOAT3:
		case DOUBLE3:
		case HALF4:
		case FLOAT4:
		case DOUBLE4:
		case HALF8:
		case FLOAT8:
		case DOUBLE8:
		case HALF16:
		case FLOAT16:
		case DOUBLE16:
			return true;
		default:
			return false;
		}
	}

	bool native_type::is_float() const
	{
		switch(id)
		{
		case FLOAT:
		case FLOAT2:
		case FLOAT3:
		case FLOAT4:
		case FLOAT8:
		case FLOAT16:
			return true;
		default:
			return false;
		}
	}

	bool native_type::is_half() const
	{
		switch(id)
		{
		case HALF:
		case HALF2:
		case HALF3:
		case HALF4:
		case HALF8:
		case HALF16:
			return true;
		default:
			return false;
		}
	}

	bool native_type::is_double() const
	{
		switch(id)
		{
		case DOUBLE:
		case DOUBLE2:
		case DOUBLE3:
		case DOUBLE4:
		case DOUBLE8:
		case DOUBLE16:
			return true;
		default:
			return false;
		}
	}

	bool native_type::is_long() const
	{
		switch(id)
		{
		case LONG:
		case LONG2:
		case LONG3:
		case LONG4:
		case LONG8:
		case LONG16:
			return true;
		default:
			return false;
		}
	}

	bool native_type::is_ulong() const
	{
		switch(id)
		{
		case ULONG:
		case ULONG2:
		case ULONG3:
		case ULONG4:
		case ULONG8:
		case ULONG16:
			return true;
		default:
			return false;
		}
	}

	bool native_type::is_signed() const
	{
		switch(id)
		{
		case HALF:
		case CHAR:
		case SHORT:
		case INT:
		case LONG:
		case FLOAT:
		case DOUBLE:
		case CHAR2:
		case SHORT2:
		case INT2:
		case LONG2:
		case FLOAT2:
		case DOUBLE2:
		case CHAR3:
		case SHORT3:
		case INT3:
		case LONG3:
		case FLOAT3:
		case DOUBLE3:
		case CHAR4:
		case SHORT4:
		case INT4:
		case LONG4:
		case FLOAT4:
		case DOUBLE4:
		case CHAR8:
		case SHORT8:
		case INT8:
		case LONG8:
		case FLOAT8:
		case DOUBLE8:
		case CHAR16:
		case SHORT16:
		case INT16:
		case LONG16:
		case FLOAT16:
		case DOUBLE16:
		case HALF2:
		case HALF3:
		case HALF4:
		case HALF8:
		case HALF16:
			return true;
		default:
			return false;
		}
	}

	native_type::type_id native_type::get_scalar_type() const
	{
		switch(id)
		{
		case CHAR:
		case CHAR2:
		case CHAR3:
		case CHAR4:
		case CHAR8:
		case CHAR16:
			return CHAR;
		case SHORT:
		case SHORT2:
		case SHORT3:
		case SHORT4:
		case SHORT8:
		case SHORT16:
			return SHORT;
		case INT:
		case INT2:
		case INT3:
		case INT4:
		case INT8:
		case INT16:
			return INT;
		case LONG:
		case LONG2:
		case LONG3:
		case LONG4:
		case LONG8:
		case LONG16:
			return LONG;
		case UCHAR:
		case UCHAR2:
		case UCHAR3:
		case UCHAR4:
		case UCHAR8:
		case UCHAR16:
			return UCHAR;
		case USHORT:
		case USHORT2:
		case USHORT3:
		case USHORT4:
		case USHORT8:
		case USHORT16:
			return USHORT;
		case UINT:
		case UINT2:
		case UINT3:
		case UINT4:
		case UINT8:
		case UINT16:
			return UINT;
		case ULONG:
		case ULONG2:
		case ULONG3:
		case ULONG4:
		case ULONG8:
		case ULONG16:
			return ULONG;
		case FLOAT:
		case FLOAT2:
		case FLOAT3:
		case FLOAT4:
		case FLOAT8:
		case FLOAT16:
			return FLOAT;
		case DOUBLE:
		case DOUBLE2:
		case DOUBLE3:
		case DOUBLE4:
		case DOUBLE8:
		case DOUBLE16:
			return DOUBLE;
		case HALF:
		case HALF2:
		case HALF3:
		case HALF4:
		case HALF8:
		case HALF16:
			return HALF;
		default:
			return id;
		}
	}

	smartptr<type> native_type::get_int_for_dim(int dim)
	{
		switch(dim)
		{
		case 1:
			return t_int;
		case 2:
			return t_int2;
		case 3:
			return t_int3;
		case 4:
			return t_int4;
		case 8:
			return t_int8;
		case 16:
			return t_int16;
		default:
			return t_int;
		}
	}

	smartptr<type> native_type::make_vector_type(int base, int dim)
	{
#define IMPL(t)\
		switch(dim)\
		{\
		case 1:		return t;\
		case 2:		return t##2;\
		case 3:		return t##3;\
		case 4:		return t##4;\
		case 8:		return t##8;\
		case 16:	return t##16;\
		default:\
			return t;\
		}

		switch(base)
		{
		case CHAR:		IMPL(t_char);
		case SHORT:		IMPL(t_short);
		case INT:		IMPL(t_int);
		case LONG:		IMPL(t_long);
		case UCHAR:		IMPL(t_uchar);
		case USHORT:	IMPL(t_ushort);
		case UINT:		IMPL(t_uint);
		case ULONG:		IMPL(t_ulong);
		case HALF:		IMPL(t_half);
		case FLOAT:		IMPL(t_float);
		case DOUBLE:	IMPL(t_double);
		default:
			return (type*)NULL;
		}
#undef IMPL
	}
}
