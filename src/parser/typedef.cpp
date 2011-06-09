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
	Typedef::Typedef(const std::string &name, const smartptr<Type> &type)
		: Type(type->isConst(), type->getAddressSpace()),
		  name(name),
		  type(type)
	{
	}

	void Typedef::write(std::ostream& out) const
	{
		out << name << ' ';
	}

	bool Typedef::operator==(const Type &type) const
	{
		const Typedef *def = dynamic_cast<const Typedef*>(&type);
		if (def)
			return *this == *(def->type);
		return *(this->type) == type;
	}

	std::string Typedef::getName() const
	{
		return name;
	}

	smartptr<Type> Typedef::clone(const bool b_const, const AddressSpace address_space) const
	{
		return new Typedef(name, type->clone(b_const, address_space));
	}

	void Typedecl::write(std::ostream& out) const
	{
		out << "typedef ";
		smartptr<StructType> s = type.as<StructType>();
		if (s)
			s->define(out);
		else
			out << *type;
		out << ' ';
	}
}
