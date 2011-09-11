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
#include "ternary.h"
#include "parser.h"
#include "native_type.h"

namespace FreeOCL
{
	Ternary::Ternary(const smartptr<Expression> &exp1, const smartptr<Expression> &exp2, const smartptr<Expression> &exp3)
		: exp1(exp1),
		exp2(exp2),
		exp3(exp3)
	{
		const smartptr<Type> t0 = exp2->getType();
		const smartptr<Type> t1 = exp3->getType();
		type = Type::computeResultingType(t0, t1);
	}

	Ternary::~Ternary()
	{
	}

	void Ternary::write(std::ostream &out) const
	{
		out << '(' <<  *exp1 << '?' << *exp2 << ':' << *exp3 << ')';
	}

	smartptr<Type> Ternary::getType() const
	{
		return type;
	}
}
