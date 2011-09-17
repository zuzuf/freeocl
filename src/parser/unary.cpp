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
#include "unary.h"
#include "parser.h"
#include "native_type.h"
#include "pointer_type.h"

namespace FreeOCL
{
	unary::unary(int op, const smartptr<expression> &exp, const bool b_postfix)
		: exp(exp),
		op(op),
		b_postfix(b_postfix)
	{
		const smartptr<type> t0 = exp->get_type();
		switch(op)
		{
		case parser::SIZEOF:
			p_type = native_type::t_size_t;
			break;
		case parser::INC_OP:
		case parser::DEC_OP:
		case '!':
		case '~':
		case '-':
		case '+':
			p_type = t0;
			break;
		case '*':
			p_type = t0.as<pointer_type>()->get_base_type();
			break;
		case '&':
			p_type = new pointer_type(t0, true, type::PRIVATE);
			break;
		}
	}

	unary::~unary()
	{
	}

	void unary::write(std::ostream &out) const
	{
		if (op == parser::SIZEOF)
		{
			out << "sizeof(" << *exp << ')';
			return;
		}
		out << '(';
		if (b_postfix)
			out << *exp;

		switch(op)
		{
		case parser::INC_OP:	out << "++";	break;
		case parser::DEC_OP:	out << "--";	break;
		case '!':
		case '~':
		case '-':
		case '+':
		case '*':
		case '&':
			out << (char)op;
			break;
		}

		if (!b_postfix)
			out << *exp;
		out << ')';
	}

	smartptr<type> unary::get_type() const
	{
		return p_type;
	}
}
