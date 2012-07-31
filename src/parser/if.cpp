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
#include "if.h"

namespace FreeOCL
{
	_if::_if(const smartptr<expression> &test, const smartptr<node> &if_true, const smartptr<node> &if_false)
		: test(test),
		  if_true(if_true),
		  if_false(if_false)
	{
	}

	void _if::write(std::ostream& out) const
	{
		out << "if (" << *test << ')' << std::endl
			<< *if_true << std::endl;
		if (if_false)
			out << "else" << std::endl
				<< *if_false << std::endl;
	}

	bool _if::has_references_to(const std::string &function_name) const
	{
		return (test->has_references_to(function_name)
				|| if_true->has_references_to(function_name)
				|| (if_false && if_false->has_references_to(function_name)));
	}

    const char *_if::get_node_type() const
    {
        return "_if";
    }
}
