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
#ifndef __FREEOCL_PARSER_ENUM_TYPE_H__
#define __FREEOCL_PARSER_ENUM_TYPE_H__

#include "native_type.h"
#include <vector>

namespace FreeOCL
{
	class enum_type : public native_type
	{
	public:
		enum_type(const std::string &name, const std::vector<smartptr<node> > &values, const bool b_const, const address_space addr_space);

		virtual void write(std::ostream &out) const;
		virtual bool operator==(const type &t) const;
		virtual std::string get_name() const;

		virtual smartptr<type> clone(const bool b_const, const address_space addr_space) const;

        virtual const char *get_node_type() const;
    private:
		const std::string name;
		const std::vector<smartptr<node> > v_values;
	};
}

#endif
