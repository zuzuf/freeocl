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
#include "typedef.h"
#include "struct_type.h"

namespace FreeOCL
{
	type_def::type_def(const std::string &name, const smartptr<type> &p_type)
		: type(p_type->is_const(), p_type->get_address_space()),
		  name(name),
		  p_type(p_type)
	{
	}

	void type_def::write(std::ostream& out) const
	{
		out << name << ' ';
	}

	bool type_def::operator==(const type &p_type) const
	{
		const type_def *def = dynamic_cast<const type_def*>(&p_type);
		if (def)
			return *this == *(def->p_type);
		return *(this->p_type) == p_type;
	}

	std::string type_def::get_name() const
	{
		return name;
	}

	smartptr<type> type_def::clone(const bool b_const, const address_space addr_space) const
	{
		return new type_def(name, p_type->clone(b_const, addr_space));
	}

	void type_decl::write(std::ostream& out) const
	{
		out << "typedef ";
		const struct_type *s = p_type.as<struct_type>();
		if (s)
			s->define(out);
		else
			out << *p_type;
		out << ' ';
	}
}
