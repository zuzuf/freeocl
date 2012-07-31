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
#include "enum_type.h"

namespace FreeOCL
{
	enum_type::enum_type(const std::string &name, const std::vector<smartptr<node> > &values, const bool b_const, const address_space addr_space)
		: native_type(native_type::INT, b_const, addr_space),
		  name(name),
		  v_values(values)
	{

	}

	void enum_type::write(std::ostream &out) const
	{
		out << "enum " << name << ' ';
		if (!v_values.empty())
		{
			out << '{';
			for(size_t i = 0 ; i < v_values.size() ; ++i)
			{
				out << *v_values[i];
				if (i + 1 < v_values.size())
					out << ',';
			}
			out << '}';
		}
	}

	bool enum_type::operator==(const type &t) const
	{
		const enum_type *p_type = dynamic_cast<const enum_type*>(&t);
		if (p_type)
		{
			if (name.empty())
				return false;
			return p_type->name == name && is_const() == p_type->is_const();
		}
		return native_type::operator ==(t);
	}

	std::string enum_type::get_name() const
	{
		return name;
	}

	smartptr<type> enum_type::clone(const bool b_const, const address_space addr_space) const
	{
		return new enum_type(name, v_values, b_const, addr_space);
	}

    const char *enum_type::get_node_type() const
    {
        return "enum_type";
    }
}
