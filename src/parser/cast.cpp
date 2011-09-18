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

	smartptr<type> cast::get_type() const
	{
		return p_type;
	}
}
