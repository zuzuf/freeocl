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

namespace FreeOCL
{
	function::function(const smartptr<type> &return_type, const std::string &name, const smartptr<chunk> &arguments, const smartptr<chunk> &body)
		: name(name),
		return_type(return_type),
		arguments(arguments),
		body(body)
	{
		this->arguments->pop_front();
		this->arguments->pop_back();
		this->body->pop_front();
		this->body->pop_back();
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
		out << *return_type << ' ' << name << '(';
		for(size_t i = 0 ; i < arguments->size() ; ++i)
		{
			if (i > 0)
				out << ',';
			out << *(*arguments)[i];
		}
		out << ')' << std::endl
			<< '{' << std::endl
			<< *body
			<< '}' << std::endl;
	}

	smartptr<type> function::get_return_type(const std::deque<smartptr<type> > &/*arg_types*/) const
	{
		return return_type;
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
		return (name == function_name) || body->has_references_to(function_name);
	}
}
