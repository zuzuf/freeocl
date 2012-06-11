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
#include "index.h"
#include "pointer_type.h"

namespace FreeOCL
{
	index::index(const smartptr<expression> &ptr, const smartptr<expression> &idx)
		: ptr(ptr),
		idx(idx)
	{

	}

	index::~index()
	{

	}

	smartptr<type> index::get_type() const
	{
		const smartptr<type> &vtype = ptr->get_type();
		const pointer_type *p_type = vtype.as<pointer_type>();
		if (!p_type)		// This should never happen
			return (type*)NULL;
		return p_type->get_base_type();
	}

	void index::write(std::ostream& out) const
	{
		out << *ptr << '[' << *idx << ']';
	}

	uint32_t index::eval_as_uint() const
	{
		throw "operator [] cannot be statically evaluated";
		return 0;
	}

	bool index::has_references_to(const std::string &function_name) const
	{
		return ptr->has_references_to(function_name) || idx->has_references_to(function_name);
	}
}
