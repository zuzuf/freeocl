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
#include "native_type.h"
#include "pointer_type.h"

namespace FreeOCL
{
	template<>	smartptr<type> value<std::string>::get_type() const
	{	return pointer_type::t_p_const_char;	}

	template<>	smartptr<type> value<double>::get_type() const
	{	return native_type::t_double;	}

	template<>	smartptr<type> value<float>::get_type() const
	{	return native_type::t_float;	}

	template<>	smartptr<type> value<char>::get_type() const
	{	return native_type::t_char;	}

	template<>	smartptr<type> value<uint64_t>::get_type() const
	{	return native_type::t_ulong;	}

	template<>	smartptr<type> value<int64_t>::get_type() const
	{	return native_type::t_long;	}

	template<>	smartptr<type> value<int>::get_type() const
	{	return native_type::t_int;	}

	template<>	smartptr<type> value<bool>::get_type() const
	{	return native_type::t_bool;	}
}
