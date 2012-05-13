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
#include "member.h"
#include "pointer_type.h"
#include "struct_type.h"
#include "typedef.h"

namespace FreeOCL
{
	member::member(const smartptr<expression> &base, const std::string &member_name)
		: base(base), member_name(member_name)
	{
	}

	void member::write(std::ostream& out) const
	{
		smartptr<type> p_type = base->get_type();
		if (p_type.as<type_def>())	p_type = p_type.as<type_def>()->get_type();
		out << *base;
		if (p_type.as<pointer_type>())
			out << "->";
		else
			out << '.';
		out << member_name;
	}

	smartptr<type> member::get_type() const
	{
		smartptr<type> p_type = base->get_type();
		if (p_type.as<type_def>())	p_type = p_type.as<type_def>()->get_type();

		const smartptr<pointer_type> ptr = p_type.as<pointer_type>();
		smartptr<struct_type> s_type;
		if (ptr)
		{
			p_type = ptr->get_base_type();
			if (p_type.as<type_def>())	p_type = p_type.as<type_def>()->get_type();
			s_type = p_type.as<struct_type>();
		}
		else
			s_type = p_type.as<struct_type>();
		return s_type->get_type_of_member(member_name);
	}

	uint32_t member::eval_as_uint() const
	{
		throw "user defined types cannot appear in statically evaluated expressions";
		return 0;
	}
}
