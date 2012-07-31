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
#ifndef __FREEOCL_PARSER_TYPEDEF_H__
#define __FREEOCL_PARSER_TYPEDEF_H__

#include "type.h"

namespace FreeOCL
{
	class type_def : public type
	{
	public:
		type_def(const std::string &name, const smartptr<type> &p_type);

		virtual void write(std::ostream& out) const;

		virtual bool operator==(const type &p_type) const;
		virtual std::string get_name() const;
		virtual smartptr<type> clone(const bool b_const, const address_space addr_space) const;
        virtual const char *get_node_type() const;

		inline const smartptr<type> &get_type() const
		{
			return p_type;
		}

	protected:
		const std::string name;
		const smartptr<type> p_type;
	};

	class type_decl : public type_def
	{
	public:
		inline type_decl(const smartptr<type> &type)
			: type_def(std::string(), type.as<type_def>() ? type.as<type_def>()->get_type() : type)
		{}
		virtual void write(std::ostream& out) const;
        virtual const char *get_node_type() const;
    };
}

#endif
