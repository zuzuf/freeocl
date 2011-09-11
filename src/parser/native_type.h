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
	class NativeType : public Type
	{
	public:
		enum TypeID {
			VOID, BOOL, HALF, SIZE_T,
			CHAR, SHORT, INT, LONG, UCHAR, USHORT, UINT, ULONG, FLOAT, DOUBLE,
			CHAR2, SHORT2, INT2, LONG2, UCHAR2, USHORT2, UINT2, ULONG2, FLOAT2, DOUBLE2,
			CHAR3, SHORT3, INT3, LONG3, UCHAR3, USHORT3, UINT3, ULONG3, FLOAT3, DOUBLE3,
			CHAR4, SHORT4, INT4, LONG4, UCHAR4, USHORT4, UINT4, ULONG4, FLOAT4, DOUBLE4,
			CHAR8, SHORT8, INT8, LONG8, UCHAR8, USHORT8, UINT8, ULONG8, FLOAT8, DOUBLE8,
			CHAR16, SHORT16, INT16, LONG16, UCHAR16, USHORT16, UINT16, ULONG16, FLOAT16, DOUBLE16,
			HALF2, HALF3, HALF4, HALF8, HALF16
		};
	public:
		NativeType(const TypeID type_id, const bool b_const, const AddressSpace address_space) : Type(b_const, address_space), type_id(type_id)	{}
		virtual bool operator==(const Type &type) const;
		virtual void write(std::ostream &out) const;
		virtual std::string getName() const;
		virtual smartptr<Type> clone(const bool b_const, const AddressSpace address_space) const;

		TypeID getTypeID() const	{	return type_id;	}
		int getDim() const;
		bool isVector() const	{	return getDim() > 1;	}
		bool isScalar() const	{	return getDim() == 1;	}
		bool isInteger() const;
		bool isFloat() const;
		bool isSigned() const;
		TypeID getScalarType() const;
	private:
		const TypeID type_id;

	public:
		static smartptr<Type> t_void;
		static smartptr<Type> t_bool;
		static smartptr<Type> t_half;
		static smartptr<Type> t_half2;
		static smartptr<Type> t_half3;
		static smartptr<Type> t_half4;
		static smartptr<Type> t_half8;
		static smartptr<Type> t_half16;
		static smartptr<Type> t_size_t;
		static smartptr<Type> t_char;
		static smartptr<Type> t_short;
		static smartptr<Type> t_int;
		static smartptr<Type> t_long;
		static smartptr<Type> t_uchar;
		static smartptr<Type> t_ushort;
		static smartptr<Type> t_uint;
		static smartptr<Type> t_ulong;
		static smartptr<Type> t_float;
		static smartptr<Type> t_double;
		static smartptr<Type> t_char2;
		static smartptr<Type> t_short2;
		static smartptr<Type> t_int2;
		static smartptr<Type> t_long2;
		static smartptr<Type> t_uchar2;
		static smartptr<Type> t_ushort2;
		static smartptr<Type> t_uint2;
		static smartptr<Type> t_ulong2;
		static smartptr<Type> t_float2;
		static smartptr<Type> t_double2;
		static smartptr<Type> t_char3;
		static smartptr<Type> t_short3;
		static smartptr<Type> t_int3;
		static smartptr<Type> t_long3;
		static smartptr<Type> t_uchar3;
		static smartptr<Type> t_ushort3;
		static smartptr<Type> t_uint3;
		static smartptr<Type> t_ulong3;
		static smartptr<Type> t_float3;
		static smartptr<Type> t_double3;
		static smartptr<Type> t_char4;
		static smartptr<Type> t_short4;
		static smartptr<Type> t_int4;
		static smartptr<Type> t_long4;
		static smartptr<Type> t_uchar4;
		static smartptr<Type> t_ushort4;
		static smartptr<Type> t_uint4;
		static smartptr<Type> t_ulong4;
		static smartptr<Type> t_float4;
		static smartptr<Type> t_double4;
		static smartptr<Type> t_char8;
		static smartptr<Type> t_short8;
		static smartptr<Type> t_int8;
		static smartptr<Type> t_long8;
		static smartptr<Type> t_uchar8;
		static smartptr<Type> t_ushort8;
		static smartptr<Type> t_uint8;
		static smartptr<Type> t_ulong8;
		static smartptr<Type> t_float8;
		static smartptr<Type> t_double8;
		static smartptr<Type> t_char16;
		static smartptr<Type> t_short16;
		static smartptr<Type> t_int16;
		static smartptr<Type> t_long16;
		static smartptr<Type> t_uchar16;
		static smartptr<Type> t_ushort16;
		static smartptr<Type> t_uint16;
		static smartptr<Type> t_ulong16;
		static smartptr<Type> t_float16;
		static smartptr<Type> t_double16;
		static smartptr<Type> getIntForDim(int dim);

		static int getDimFor(int type_id);
		static smartptr<Type> makeVectorType(int base, int dim);
	};
}

#endif
