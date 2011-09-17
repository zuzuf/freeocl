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
	class ternary : public expression
	{
	public:
		ternary(const smartptr<expression> &exp1, const smartptr<expression> &exp2, const smartptr<expression> &exp3);
		virtual ~ternary();

		virtual void write(std::ostream &out) const;

		virtual smartptr<type> get_type() const;

		const smartptr<expression> &get_exp1() const	{	return exp1;	}
		const smartptr<expression> &get_exp2() const	{	return exp2;	}
		const smartptr<expression> &get_exp3() const	{	return exp3;	}
	private:
		const smartptr<expression> exp1;
		const smartptr<expression> exp2;
		const smartptr<expression> exp3;
		smartptr<type> p_type;
	};
}

#endif
