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
#include "function.h"
#include "chunk.h"
#include "type.h"
#include "overloaded_builtin.h"

namespace FreeOCL
{
    function::function(const smartptr<type> &return_type,
                       const std::string &name,
                       const smartptr<chunk> &arguments,
                       const smartptr<chunk> &body,
                       std::deque<smartptr<type> > &arg_types)
		: name(name),
		return_type(return_type),
		arguments(arguments),
		body(body)
	{
        // This is required to respect overloaded_builtin::all_types_weak_match semantics
        arg_types.push_front((type*)NULL);
        this->arg_types.swap(arg_types);
		this->arguments->pop_front();
		this->arguments->pop_back();
		if (body)
		{
			this->body->pop_front();
			this->body->pop_back();
		}
		if (this->arguments->size() == 1)
			this->arguments = this->arguments->front();
		else
			this->arguments = new chunk();
	}

	function::~function()
	{
	}

	void function::write(std::ostream &out) const
	{
		out << "static inline " << *return_type << ' ' << name << '(';
		for(size_t i = 0 ; i < arguments->size() ; ++i)
		{
			if (i > 0)
				out << ',';
			out << *(*arguments)[i];
		}
		out << ')';
		if (body)
			out << std::endl
				<< '{' << std::endl
				<< *body
				<< '}' << std::endl;
		else
			out << ';' << std::endl;
	}

    smartptr<type> function::get_return_type(const std::deque<smartptr<type> > &arg_types) const
	{
        if (arg_types.size() != arg_types.size())
            return (type*)NULL;
        if (!overloaded_builtin::all_types_weak_match(arg_types, this->arg_types))
            return (type*)NULL;
		return return_type;
	}

	bool function::is_defined() const
	{
		return body;
	}

	const std::string &function::get_name() const
	{
		return name;
	}

	size_t function::get_num_params() const
	{
		return arguments->size();
	}

	bool function::has_references_to(const std::string &function_name) const
	{
		return (name == function_name) || (body && body->has_references_to(function_name));
	}

    const char *function::get_node_type() const
    {
        return "function";
    }

    std::deque<smartptr<type> > function::get_arg_types(const std::deque<smartptr<type> > &/*param_types*/) const
    {
        std::deque<smartptr<type> > arg_types(this->arg_types);
        arg_types.pop_front();
        return arg_types;
    }

	bool function::operator==(const function &f) const
	{
		if (f.arg_types.size() != arg_types.size()
				|| *return_type != *f.return_type)
			return false;
		for(size_t i = 1 ; i < arg_types.size() ; ++i)
			if (*arg_types[i] != *f.arg_types[i])
				return false;
		return true;
	}
}
