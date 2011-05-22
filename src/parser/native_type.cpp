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
	smartptr<Type> NativeType::t_void(new NativeType(NativeType::VOID, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_bool(new NativeType(NativeType::BOOL, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_half(new NativeType(NativeType::HALF, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_size_t(new NativeType(NativeType::SIZE_T, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_char(new NativeType(NativeType::CHAR, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_short(new NativeType(NativeType::SHORT, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_int(new NativeType(NativeType::INT, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_long(new NativeType(NativeType::LONG, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uchar(new NativeType(NativeType::UCHAR, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ushort(new NativeType(NativeType::USHORT, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uint(new NativeType(NativeType::UINT, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ulong(new NativeType(NativeType::ULONG, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_float(new NativeType(NativeType::FLOAT, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_double(new NativeType(NativeType::DOUBLE, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_char2(new NativeType(NativeType::CHAR2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_short2(new NativeType(NativeType::SHORT2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_int2(new NativeType(NativeType::INT2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_long2(new NativeType(NativeType::LONG2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uchar2(new NativeType(NativeType::UCHAR2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ushort2(new NativeType(NativeType::USHORT2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uint2(new NativeType(NativeType::UINT2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ulong2(new NativeType(NativeType::ULONG2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_float2(new NativeType(NativeType::FLOAT2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_double2(new NativeType(NativeType::DOUBLE2, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_char3(new NativeType(NativeType::CHAR3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_short3(new NativeType(NativeType::SHORT3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_int3(new NativeType(NativeType::INT3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_long3(new NativeType(NativeType::LONG3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uchar3(new NativeType(NativeType::UCHAR3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ushort3(new NativeType(NativeType::USHORT3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uint3(new NativeType(NativeType::UINT3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ulong3(new NativeType(NativeType::ULONG3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_float3(new NativeType(NativeType::FLOAT3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_double3(new NativeType(NativeType::DOUBLE3, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_char4(new NativeType(NativeType::CHAR4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_short4(new NativeType(NativeType::SHORT4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_int4(new NativeType(NativeType::INT4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_long4(new NativeType(NativeType::LONG4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uchar4(new NativeType(NativeType::UCHAR4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ushort4(new NativeType(NativeType::USHORT4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uint4(new NativeType(NativeType::UINT4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ulong4(new NativeType(NativeType::ULONG4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_float4(new NativeType(NativeType::FLOAT4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_double4(new NativeType(NativeType::DOUBLE4, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_char8(new NativeType(NativeType::CHAR8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_short8(new NativeType(NativeType::SHORT8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_int8(new NativeType(NativeType::INT8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_long8(new NativeType(NativeType::LONG8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uchar8(new NativeType(NativeType::UCHAR8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ushort8(new NativeType(NativeType::USHORT8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uint8(new NativeType(NativeType::UINT8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ulong8(new NativeType(NativeType::ULONG8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_float8(new NativeType(NativeType::FLOAT8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_double8(new NativeType(NativeType::DOUBLE8, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_char16(new NativeType(NativeType::CHAR16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_short16(new NativeType(NativeType::SHORT16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_int16(new NativeType(NativeType::INT16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_long16(new NativeType(NativeType::LONG16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uchar16(new NativeType(NativeType::UCHAR16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ushort16(new NativeType(NativeType::USHORT16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_uint16(new NativeType(NativeType::UINT16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_ulong16(new NativeType(NativeType::ULONG16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_float16(new NativeType(NativeType::FLOAT16, true, Type::CONSTANT));
	smartptr<Type> NativeType::t_double16(new NativeType(NativeType::DOUBLE16, true, Type::CONSTANT));

	bool NativeType::operator==(const Type &type) const
	{
		const NativeType *pType = dynamic_cast<const NativeType*>(&type);
		return pType != NULL && pType->type_id == type_id && isConst() == pType->isConst();
	}

	std::string NativeType::getName() const
	{
		static const char *type_name[] = {
			"void", "bool", "half", "size_t",
			"char", "short", "int", "long", "uchar", "ushort", "uint", "ulong", "float", "double",
			"char2", "short2", "int2", "long2", "uchar2", "ushort2", "uint2", "ulong2", "float2", "double2",
			"char3", "short3", "int3", "long3", "uchar3", "ushort3", "uint3", "ulong3", "float3", "double3",
			"char4", "short4", "int4", "long4", "uchar4", "ushort4", "uint4", "ulong4", "float4", "double4",
			"char8", "short8", "int8", "long8", "uchar8", "ushort8", "uint8", "ulong8", "float8", "double8",
			"char16", "short16", "int16", "long16", "uchar16", "ushort16", "uint16", "ulong16", "float16", "double16",
		};
		std::string prefix;
		switch(getAddressSpace())
		{
		case PRIVATE:	break;
		case GLOBAL:	prefix = "__global ";	break;
		case LOCAL:		prefix = "__local ";	break;
		case CONSTANT:	prefix = "__constant ";	break;
		}
		if (isConst())
			return prefix + std::string("const ") + type_name[type_id];
		return prefix + type_name[type_id];
	}

	void NativeType::write(std::ostream &out) const
	{
		out << getName() << ' ';
	}

	int NativeType::getDim() const
	{
		static int type_dim[] = {
			0, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
			4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
			16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		};
		return type_dim[type_id];
	}

	smartptr<Type> NativeType::clone(const bool b_const, const AddressSpace address_space) const
	{
		return new NativeType(type_id, b_const, address_space);
	}
}
