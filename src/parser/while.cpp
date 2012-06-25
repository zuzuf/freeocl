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
#include "while.h"

namespace FreeOCL
{
	_while::_while(const smartptr<expression> &test, const smartptr<node> &stmt)
		: test(test),
		  stmt(stmt)
	{
	}

	void _while::write(std::ostream& out) const
	{
		out << "while(" << *test << ')' << std::endl
			<< *stmt << std::endl;
	}

	bool _while::has_references_to(const std::string &function_name) const
	{
		return test->has_references_to(function_name) || stmt->has_references_to(function_name);
	}
}
