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

namespace FreeOCL
{
	member::member(const smartptr<expression> &base, const std::string &member_name)
		: base(base), member_name(member_name)
	{
	}

	void member::write(std::ostream& out) const
	{
		out << *base;
		if (base->get_type().as<pointer_type>())
			out << "->";
		else
			out << '.';
		out << member_name;
	}

	smartptr<type> member::get_type() const
	{
		const smartptr<pointer_type> ptr = base->get_type().as<pointer_type>();
		const smartptr<struct_type> type = ptr ? ptr->get_base_type() : base->get_type().as<struct_type>();
		return type->get_type_of_member(member_name);
	}
}
