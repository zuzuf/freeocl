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
#include "overloaded_function.h"
#include <stdexcept>

namespace FreeOCL
{
	overloaded_function::overloaded_function(const smartptr<callable> &f0, const smartptr<callable> &f1)
	{
		fn.push_back(f0);
		fn.push_back(f1);
	}

	overloaded_function::~overloaded_function()
	{

	}

	smartptr<type> overloaded_function::get_return_type(const std::deque<smartptr<type> > &arg_types) const
	{
		smartptr<type> ret;
		for(size_t i = 0 ; i < fn.size() ; ++i)
		{
			const smartptr<type> &p_type = fn[i]->get_return_type(arg_types);
			if (p_type)
			{
				if (ret)
					throw std::runtime_error("reference to function is ambiguous");
				ret = p_type;
			}
		}
		return ret;
	}

    std::deque<smartptr<type> > overloaded_function::get_arg_types(const std::deque<smartptr<type> > &param_types) const
    {
        for(size_t i = 0 ; i < fn.size() ; ++i)
        {
            const std::deque<smartptr<type> > &arg_types = fn[i]->get_arg_types(param_types);
            if (!arg_types.empty())
                return arg_types;
        }
        return std::deque<smartptr<type> >();
    }

	const smartptr<callable> &overloaded_function::get_function(const std::deque<smartptr<type> > &arg_types) const
	{
		for(size_t i = 0 ; i < fn.size() ; ++i)
		{
			const smartptr<type> &p_type = fn[i]->get_return_type(arg_types);
			if (p_type)
				return fn[i];
		}
		static smartptr<callable> null;
		return null;
	}

	const std::string &overloaded_function::get_name() const
	{
		return fn.front()->get_name();
	}

	size_t overloaded_function::get_num_params() const
	{
		size_t n = 0;
		for(size_t i = 0 ; i < fn.size() ; ++i)
			n = std::max(n, fn[i]->get_num_params());
		return n;
	}

	void overloaded_function::write(std::ostream& out) const
	{
	}

	bool overloaded_function::has_references_to(const std::string &function_name) const
	{
		// NB: we should not get there
		return false;
	}

	void overloaded_function::add_function(const smartptr<callable> &f)
	{
		fn.push_back(f);
	}

    const char *overloaded_function::get_node_type() const
    {
        return "overloaded_function";
    }
}
