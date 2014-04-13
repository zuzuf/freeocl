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
#ifndef __FREEOCL_PARSER_STRUCT_TYPE_H__
#define __FREEOCL_PARSER_STRUCT_TYPE_H__

#include "type.h"
#include <vector>

namespace FreeOCL
{
	class struct_type : public type
	{
		friend class union_type;
	protected:
		enum aggregate_type
		{
			STRUCT,
			UNION
		};
	private:
        struct_type(const smartptr<struct_type> &orig, const bool b_const, const address_space addr_space) : type(b_const, addr_space), root(orig), b_defined(false)	{}
	public:
        struct_type(const std::string &name) : type(false, PRIVATE), name(name), b_defined(false)	{}
		virtual bool operator==(const type &t) const;
		virtual void write(std::ostream &out) const;
		virtual void define(std::ostream &out) const;
		virtual std::string get_name() const;

		bool has_member(const std::string &name) const;
		size_t members_count() const;
		smartptr<type> get_type_of_member(const std::string &name) const;
		virtual smartptr<type> clone(const bool b_const, const address_space addr_space) const;

		struct_type &operator<<(const std::pair<std::string, smartptr<type> > &member);

        virtual const char *get_node_type() const;
    protected:
		virtual aggregate_type get_aggregate_type() const;

	protected:
		const std::string name;
		std::vector<std::pair<std::string, smartptr<type> > > members;
		smartptr<struct_type> root;
        mutable bool b_defined;
	};
}

#endif
