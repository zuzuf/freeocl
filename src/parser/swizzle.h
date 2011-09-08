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
#ifndef __FREEOCL_PARSER_SWIZZLE_H__
#define __FREEOCL_PARSER_SWIZZLE_H__

#include "expression.h"

namespace FreeOCL
{
	class Swizzle : public Expression
	{
	public:
		Swizzle(const smartptr<Expression> &base, const std::string &components);
		void write(std::ostream& out) const;
		smartptr<Type> getType() const;

	private:
		static void parseComponents(const std::string &components, int values[]);

	public:
		static int getNumberOfComponents(const std::string &components);
		static bool validateComponents(const std::string &components, int dim);
	private:
		smartptr<Expression> base;
		const std::string components;
	};
}

#endif
