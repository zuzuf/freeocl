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
#include "value.h"

namespace FreeOCL
{
	template<>	smartptr<Type> Value<std::string>::getType() const
	{	return PointerType::t_p_const_char;	}

	template<>	smartptr<Type> Value<double>::getType() const
	{	return NativeType::t_double;	}

	template<>	smartptr<Type> Value<float>::getType() const
	{	return NativeType::t_float;	}

	template<>	smartptr<Type> Value<char>::getType() const
	{	return NativeType::t_char;	}

	template<>	smartptr<Type> Value<uint64_t>::getType() const
	{	return NativeType::t_ulong;	}

	template<>	smartptr<Type> Value<int64_t>::getType() const
	{	return NativeType::t_long;	}

	template<>	smartptr<Type> Value<int>::getType() const
	{	return NativeType::t_int;	}

	template<>	smartptr<Type> Value<bool>::getType() const
	{	return NativeType::t_bool;	}
}
