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
#include "cast.h"
#include "native_type.h"
#include "binary.h"
#include <sstream>
#include <iostream>

namespace FreeOCL
{
	cast::cast(const smartptr<type> &p_type, const smartptr<expression> &exp)
		: exp(exp), p_type(p_type)
	{
	}

	void cast::write(std::ostream &out) const
	{
		smartptr<native_type> native = p_type.as<native_type>();
		if (native && native->is_vector())
		{
			out << *p_type << "::make(";
			std::string post;
			smartptr<expression> cur = exp;
			while(cur.as<binary>() && cur.as<binary>()->get_op() == ',')
			{
				smartptr<binary> bin = cur.as<binary>();
				std::stringstream buf;
				buf << *(bin->get_right());
				post = ',' + buf.str() + post;
				cur = bin->get_left();
			}
			out << *cur << post << ')';
		}
		else
			out << '(' << *p_type << ")(" << *exp << ')';
	}

	bool cast::validate() const
	{
		smartptr<native_type> native = p_type.as<native_type>();
		if (native && native->is_vector())
		{
			const int literal_dim = native->get_dim();
			int acc_dim = 0;
			smartptr<expression> cur = exp;
			while(cur.as<binary>() && cur.as<binary>()->get_op() == ',')
			{
				smartptr<binary> bin = cur.as<binary>();
				smartptr<native_type> t = bin->get_right()->get_type().as<native_type>();
				if (!t)
					return false;
				acc_dim += t->get_dim();
				cur = bin->get_left();
			}
			smartptr<expression> exp = cur.as<expression>();
			if (!exp)
				return false;
			smartptr<native_type> t = exp->get_type().as<native_type>();
			if (!t)
				return false;
			acc_dim += t->get_dim();
			return literal_dim == acc_dim || acc_dim == 1;
		}
		else
			return true;
	}

	smartptr<type> cast::get_type() const
	{
		return p_type;
	}
}
