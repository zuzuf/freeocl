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
#include "type.h"
#include "native_type.h"
#include "pointer_type.h"

namespace FreeOCL
{
	Type::Type(const bool b_const, const AddressSpace address_space) : b_const(b_const), address_space(address_space)
	{
	}

	smartptr<Type> Type::computeResultingType(const smartptr<Type> &t0, const smartptr<Type> &t1)
	{
		const NativeType *n0 = t0.as<NativeType>();
		const NativeType *n1 = t1.as<NativeType>();
		if (n0 && n1)
		{
			if (n0->isScalar() && n1->isScalar())
			{
				if (n0->isFloat() || n1->isFloat())
					return NativeType::t_double;
				return NativeType::t_long;
			}
			if (n0->isVector())
				return t0;
			if (n1->isVector())
				return t1;
			// Normally this should not happen
			return NativeType::t_void;
		}
		// Pointer arithmetics
		if (t0.as<PointerType>() && n1)
			return t0;
		if (t1.as<PointerType>() && n0)
			return t1;
		// If you get there, someone is doing something ugly
		return NativeType::t_void;
	}
}
