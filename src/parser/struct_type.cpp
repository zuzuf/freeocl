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
#include "struct_type.h"
#include <algorithm>

namespace FreeOCL
{
	bool struct_type::operator==(const type &type) const
	{
		const struct_type *p_type = dynamic_cast<const struct_type*>(&type);
		if (p_type == NULL)
			return false;
		if (root)
			return *root == type;
		if (p_type->root)
			return *this == *(p_type->root);
		return get_aggregate_type() == p_type->get_aggregate_type()
				&& name == p_type->name
				&& members.size() == p_type->members.size()
				&& std::equal(members.begin(), members.end(), p_type->members.begin());
	}

	void struct_type::define(std::ostream &out) const
	{
		if (root)
		{
			root->define(out);
			return;
		}
		switch(get_aggregate_type())
		{
		case STRUCT:
			out << "struct ";
			break;
		case UNION:
			out << "union ";
			break;
		}

		out << name << std::endl
			<< '{' << std::endl;

		for(std::vector<std::pair<std::string, smartptr<type> > >::const_iterator it = members.begin(), end = members.end()
			; it != end
			; ++it)
		{
			out << "    ";
			if (it->second.as<struct_type>())
			{
				it->second.as<struct_type>()->define(out);
				out << ' ';
			}
			out << it->second->prefix() << ' ' << it->first << it->second->suffix() << ';' << std::endl;
		}

		out << '}';
	}

	void struct_type::write(std::ostream &out) const
	{
		define(out);
	}

	std::string struct_type::get_name() const
	{
		if (root)
			return root->name;
		return name;
	}

	bool struct_type::has_member(const std::string &name) const
	{
		if (root)
			return root->has_member(name);
		for(std::vector<std::pair<std::string, smartptr<type> > >::const_iterator it = members.begin(), end = members.end()
			; it != end
			; ++it)
			if (it->first == name)
				return true;
		return false;
	}

	smartptr<type> struct_type::get_type_of_member(const std::string &name) const
	{
		if (root)
			return root->get_type_of_member(name);
		for(std::vector<std::pair<std::string, smartptr<type> > >::const_iterator it = members.begin(), end = members.end()
			; it != end
			; ++it)
			if (it->first == name)
				return it->second;
		return (type*)NULL;
	}

	smartptr<type> struct_type::clone(const bool b_const, const address_space addr_space) const
	{
		smartptr<struct_type> ret = root ? new struct_type(root, b_const, addr_space) : new struct_type(this, b_const, addr_space);
		ret->members = members;
		return ret;
	}

	struct_type &struct_type::operator<<(const std::pair<std::string, smartptr<type> > &member)
	{
		if (root)
		{
			*root << member;
			return *this;
		}
		members.push_back(member);
		return *this;
	}

	struct_type::aggregate_type struct_type::get_aggregate_type() const
	{
		if (root)
			return root->get_aggregate_type();
		return STRUCT;
	}

	size_t struct_type::members_count() const
	{
		return members.size();
	}
}
