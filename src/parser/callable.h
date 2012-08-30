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
#ifndef __FREEOCL_PARSER_CALLABLE_H__
#define __FREEOCL_PARSER_CALLABLE_H__

#include "type.h"
#include <deque>

namespace FreeOCL
{
	class callable : public node
	{
	public:
		virtual smartptr<type> get_return_type(const std::deque<smartptr<type> > &arg_types) const = 0;
		virtual const std::string &get_name() const = 0;
		virtual size_t get_num_params() const = 0;
		virtual bool check_num_params(const size_t n) const;
		virtual bool is_defined() const;
        virtual std::deque<smartptr<type> > get_arg_types(const std::deque<smartptr<type> > &param_types) const = 0;
	};
}

#endif
