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
	Cast::Cast(const smartptr<Type> &type, const smartptr<Expression> &exp)
		: exp(exp), type(type)
	{
	}

	void Cast::write(std::ostream &out) const
	{
		smartptr<NativeType> native = type.as<NativeType>();
		if (native && native->isVector())
		{
			out << *type << "::make(";
			std::string post;
			smartptr<Expression> cur = exp;
			while(cur.as<Binary>() && cur.as<Binary>()->getOp() == ',')
			{
				smartptr<Binary> bin = cur.as<Binary>();
				std::stringstream buf;
				buf << bin->getRight();
				post = ',' + buf.str() + post;
				cur = bin->getLeft();
			}
			out << *cur << post << ')';
		}
		else
			out << '(' << *type << ")(" << *exp << ')';
	}

	smartptr<Type> Cast::getType() const
	{
		return type;
	}
}
