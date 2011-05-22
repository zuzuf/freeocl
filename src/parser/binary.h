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
#ifndef __FREEOCL_PARSER_BINARY_H__
#define __FREEOCL_PARSER_BINARY_H__

#include "expression.h"

namespace FreeOCL
{
	class Binary : public Expression
	{
	public:
		Binary(int op, const smartptr<Expression> &left, const smartptr<Expression> &right);
		virtual ~Binary();

		virtual void write(std::ostream &out) const;

		virtual smartptr<Type> getType() const;

		const smartptr<Expression> &getLeft() const		{	return left;	}
		const smartptr<Expression> &getRight() const	{	return right;	}
		int getOp() const	{	return op;	}
	private:
		const smartptr<Expression> left;
		const smartptr<Expression> right;
		smartptr<Type> type;
		const int op;
	};
}

#endif
