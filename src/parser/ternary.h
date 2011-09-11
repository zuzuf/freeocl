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
#ifndef __FREEOCL_PARSER_TERNARY_H__
#define __FREEOCL_PARSER_TERNARY_H__

#include "expression.h"

namespace FreeOCL
{
	class Ternary : public Expression
	{
	public:
		Ternary(const smartptr<Expression> &exp1, const smartptr<Expression> &exp2, const smartptr<Expression> &exp3);
		virtual ~Ternary();

		virtual void write(std::ostream &out) const;

		virtual smartptr<Type> getType() const;

		const smartptr<Expression> &getExp1() const	{	return exp1;	}
		const smartptr<Expression> &getExp2() const	{	return exp2;	}
		const smartptr<Expression> &getExp3() const	{	return exp3;	}
	private:
		const smartptr<Expression> exp1;
		const smartptr<Expression> exp2;
		const smartptr<Expression> exp3;
		smartptr<Type> type;
	};
}

#endif
