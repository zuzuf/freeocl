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
#include "declarator.h"
#include "pointer_type.h"

namespace FreeOCL
{
	declarator::declarator(const smartptr<type> &p_type, const smartptr<node> &n0, const smartptr<node> &n1)
		: p_type(p_type),
		  n0(n0),
		  n1(n1)
	{

	}

	declarator::~declarator()
	{

	}

	void declarator::write(std::ostream &out) const
	{
		out << *p_type << *n0 << *n1;
	}

	bool declarator::has_references_to(const std::string &function_name) const
	{
		return n0->has_references_to(function_name) || n1->has_references_to(function_name);
	}

	const char *declarator::get_node_type() const
	{
		return "declarator";
	}
}
