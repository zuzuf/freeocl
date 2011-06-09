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
	Member::Member(const smartptr<Expression> &base, const std::string &memberName)
		: base(base), memberName(memberName)
	{
	}

	void Member::write(std::ostream& out) const
	{
		out << *base;
		if (base->getType().as<PointerType>())
			out << "->";
		else
			out << '.';
		out << memberName;
	}

	smartptr<Type> Member::getType() const
	{
		const smartptr<PointerType> ptr = base->getType().as<PointerType>();
		const smartptr<StructType> type = ptr ? ptr->getBaseType() : base->getType().as<StructType>();
		return type->getTypeOfMember(memberName);
	}
}
