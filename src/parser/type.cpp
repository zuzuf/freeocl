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
	type::type(const bool b_const, const address_space addr_space) : b_const(b_const), addr_space(addr_space)
	{
	}

	smartptr<type> type::compute_resulting_type(const smartptr<type> &t0, const smartptr<type> &t1)
	{
		const native_type *n0 = t0.as<native_type>();
		const native_type *n1 = t1.as<native_type>();
		if (n0 && n1)
		{
			if (n0->is_scalar() && n1->is_scalar())
			{
				if (n0->is_double() || n1->is_double())
					return native_type::t_double;
				if (n0->is_float() || n1->is_float())
					return native_type::t_float;
				if (n0->is_ulong() || n1->is_ulong())
					return native_type::t_ulong;
				if (n0->is_long() || n1->is_long())
					return native_type::t_long;
				return native_type::t_int;
			}
			if (n0->is_vector())
				return t0;
			if (n1->is_vector())
				return t1;
			// Normally this should not happen
			return native_type::t_void;
		}
		// Pointer arithmetics
		if (t0.as<pointer_type>() && n1)
			return t0;
		if (t1.as<pointer_type>() && n0)
			return t1;
		// If you get there, someone is doing something ugly
		return native_type::t_void;
	}

	std::string type::suffix() const
	{
		return std::string();
	}

	std::string type::prefix() const
	{
		return get_cxx_name();
	}

	bool type::has_references_to(const std::string &function_name) const
	{
		return false;
	}

	std::string type::get_cxx_name() const
	{
		return get_name();
	}
}
