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
#include "pointer_type.h"
#include "native_type.h"

namespace FreeOCL
{
	smartptr<type> pointer_type::t_p_const_char(new pointer_type(new native_type(native_type::CHAR, true, type::CONSTANT), true, type::CONSTANT));

	bool pointer_type::operator==(const type &t) const
	{
		const pointer_type *p_type = dynamic_cast<const pointer_type*>(&t);
		return p_type && p_type->base_type && base_type && *(p_type->base_type) == *base_type && is_const() == p_type->is_const();
	}

	void pointer_type::write(std::ostream &out) const
	{
		out << get_name() << ' ';
	}

	std::string pointer_type::get_name() const
	{
		if (!base_type)
			return is_const() ? "* const" : "*";

		if (is_const())
			return (' ' + base_type->get_name()) + "* const";
		return (' ' + base_type->get_name()) + '*';
	}

	bool pointer_type::is_compatible_with(const pointer_type &type) const
	{
		if (!base_type || !type.base_type)
			return false;
		return type.base_type->get_address_space() == get_address_space();
	}

	smartptr<type> pointer_type::clone() const
	{
		const pointer_type *ptr = base_type.as<pointer_type>();
		if (ptr)
			return new pointer_type(ptr->clone(), is_const(), get_address_space());
		return new pointer_type(base_type, is_const(), get_address_space());
	}

	smartptr<type> pointer_type::clone(const bool b_const, const address_space addr_space) const
	{
		return new pointer_type(base_type, b_const, addr_space);
	}

	void pointer_type::set_root_type(const smartptr<type> &root_type)
	{
		pointer_type *ptr = base_type.as<pointer_type>();
		if (ptr)
			ptr->set_root_type(root_type);
		else
			base_type = root_type;
	}
}
