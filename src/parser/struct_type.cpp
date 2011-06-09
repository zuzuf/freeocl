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
	bool StructType::operator==(const Type &type) const
	{
		const StructType *pType = dynamic_cast<const StructType*>(&type);
		if (pType == NULL)
			return false;
		if (root)
			return *root == type;
		if (pType->root)
			return *this == *(pType->root);
		return getAggregateType() == pType->getAggregateType()
				&& name == pType->name
				&& members.size() == pType->members.size()
				&& std::equal(members.begin(), members.end(), pType->members.begin());
	}

	void StructType::define(std::ostream &out) const
	{
		if (root)
		{
			root->define(out);
			return;
		}
		switch(getAggregateType())
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

		for(std::vector<std::pair<std::string, smartptr<Type> > >::const_iterator it = members.begin(), end = members.end()
			; it != end
			; ++it)
			out << "    " << it->second->getName() << ' ' << it->first << ';' << std::endl;

		out << '}';
	}

	void StructType::write(std::ostream &out) const
	{
		switch(getAddressSpace())
		{
		case PRIVATE:	break;
		case GLOBAL:	out << "__global";	break;
		case LOCAL:		out << "__local";	break;
		case CONSTANT:	out << "__constant";	break;
		}

		out << getName() << ' ' << std::endl;
	}

	std::string StructType::getName() const
	{
		if (root)
			return root->name;
		return name;
	}

	bool StructType::hasMember(const std::string &name) const
	{
		if (root)
			return root->hasMember(name);
		for(std::vector<std::pair<std::string, smartptr<Type> > >::const_iterator it = members.begin(), end = members.end()
			; it != end
			; ++it)
			if (it->first == name)
				return true;
		return false;
	}

	smartptr<Type> StructType::getTypeOfMember(const std::string &name) const
	{
		if (root)
			return root->getTypeOfMember(name);
		for(std::vector<std::pair<std::string, smartptr<Type> > >::const_iterator it = members.begin(), end = members.end()
			; it != end
			; ++it)
			if (it->first == name)
				return it->second;
		return (Type*)NULL;
	}

	smartptr<Type> StructType::clone(const bool b_const, const AddressSpace address_space) const
	{
		return root ? new StructType(root, b_const, address_space) : new StructType(this, b_const, address_space);
	}

	StructType &StructType::operator<<(const std::pair<std::string, smartptr<Type> > &member)
	{
		if (root)
		{
			*root << member;
			return *this;
		}
		members.push_back(member);
		return *this;
	}

	StructType::AggregateType StructType::getAggregateType() const
	{
		if (root)
			return root->getAggregateType();
		return STRUCT;
	}
}
