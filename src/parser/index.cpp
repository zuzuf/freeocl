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
#include "array_type.h"

namespace FreeOCL
{
	index::index(const smartptr<expression> &ptr, const smartptr<expression> &idx)
		: ptr(ptr),
		idx(idx),
		b_check_bounds(false)
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
		if (!b_check_bounds)
			out << *ptr << '[' << *idx << ']';
		else
		{
			const smartptr<type> &_type = ptr->get_type();
			const array_type *atype = _type.as<array_type>();
			if (!atype)
				out << *ptr << '[' << *idx << ']';
			else
				out << "__check_lookup_bounds<" << atype->get_size() << ">(" << *ptr << ',' << *idx << ',' << '"' << ref << '"' << ')';
		}
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

	void index::enable_boundary_check(bool b_check_bounds)
	{
		this->b_check_bounds = b_check_bounds;
	}

	void index::set_ref_string(const std::string &ref)
	{
		this->ref = ref;
	}

    const char *index::get_node_type() const
    {
        return "index";
    }
}
