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
#ifndef __FREEOCL_PARSER_FUNCTION_H__
#define __FREEOCL_PARSER_FUNCTION_H__

#include <deque>
#include "node.h"
#include "chunk.h"
#include "type.h"
#include "callable.h"

namespace FreeOCL
{
	class function : public callable
	{
	public:
        function(const smartptr<type> &return_type, const std::string &name, const smartptr<chunk> &arguments, const smartptr<chunk> &body, std::deque<smartptr<type> > &arg_types);
		virtual ~function();

		virtual bool is_defined() const;

		bool operator==(const function &f) const;
		inline bool operator!=(const function &f) const	{	return !(*this == f);	}

		virtual void write(std::ostream &out) const;

		virtual smartptr<type> get_return_type(const std::deque<smartptr<type> > &arg_types) const;
		virtual const std::string &get_name() const;
		virtual size_t get_num_params() const;
        virtual std::deque<smartptr<type> > get_arg_types(const std::deque<smartptr<type> > &param_types) const;

		smartptr<chunk> get_arguments() const	{	return arguments;	}
		smartptr<chunk> get_body() const	{	return body;	}

		virtual bool has_references_to(const std::string &function_name) const;

        virtual const char *get_node_type() const;
    private:
		const std::string name;
		smartptr<type>	return_type;
		smartptr<chunk>	arguments;
		smartptr<chunk>	body;
        std::deque<smartptr<type> > arg_types;
	};
}

#endif
