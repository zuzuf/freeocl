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
	ternary::ternary(const smartptr<expression> &exp1, const smartptr<expression> &exp2, const smartptr<expression> &exp3)
		: exp1(exp1),
		exp2(exp2),
		exp3(exp3)
	{
		const smartptr<type> t0 = exp2->get_type();
		const smartptr<type> t1 = exp3->get_type();
		p_type = type::compute_resulting_type(t0, t1);
	}

	ternary::~ternary()
	{
	}

	void ternary::write(std::ostream &out) const
	{
		out << '(' <<  *exp1 << '?' << *exp2 << ':' << *exp3 << ')';
	}

	smartptr<type> ternary::get_type() const
	{
		return p_type;
	}
}
