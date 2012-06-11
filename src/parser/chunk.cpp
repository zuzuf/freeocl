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
#include "chunk.h"
#include "expression.h"

namespace FreeOCL
{
	namespace
	{
		template<class T>
		inline std::deque<T> &operator<<(std::deque<T> &out, const T &v)
		{
			out.push_back(v);
			return out;
		}
	}

	chunk::chunk()
	{}

	chunk::chunk(const smartptr<node> &n0)
	{
		childs << n0;
	}

	chunk::chunk(const smartptr<node> &n0, const smartptr<node> &n1)
	{
		childs << n0 << n1;
	}

	chunk::chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2)
	{
		childs << n0 << n1 << n2;
	}

	chunk::chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3)
	{
		childs << n0 << n1 << n2 << n3;
	}

	chunk::chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3, const smartptr<node> &n4)
	{
		childs << n0 << n1 << n2 << n3 << n4;
	}

	chunk::chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3, const smartptr<node> &n4, const smartptr<node> &n5)
	{
		childs << n0 << n1 << n2 << n3 << n4 << n5;
	}

	chunk::chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3, const smartptr<node> &n4, const smartptr<node> &n5, const smartptr<node> &n6)
	{
		childs << n0 << n1 << n2 << n3 << n4 << n5 << n6;
	}

	chunk::~chunk()
	{
	}

	void chunk::write(std::ostream &out) const
	{
		for(std::deque<smartptr<node> >::const_iterator it = childs.begin(), end = childs.end() ; it != end ; ++it)
			out << **it;
	}

	std::deque<smartptr<type> > chunk::get_as_types() const
	{
		std::deque<smartptr<type> > types;
		for(std::deque<smartptr<node> >::const_iterator it = childs.begin(), end = childs.end() ; it != end ; ++it)
		{
			const expression *exp = it->as<expression>();
			if (!exp)
			{
				types.push_back(smartptr<type>());
				continue;
			}
			types.push_back(exp->get_type());
		}

		return types;
	}

	bool chunk::has_references_to(const std::string &function_name) const
	{
		for(std::deque<smartptr<node> >::const_iterator it = childs.begin(), end = childs.end() ; it != end ; ++it)
			if ((*it)->has_references_to(function_name))
				return true;
		return false;
	}
}
