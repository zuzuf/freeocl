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
	class StructType : public Type
	{
		friend class UnionType;
	protected:
		enum AggregateType
		{
			STRUCT,
			UNION
		};
	private:
		StructType(const smartptr<StructType> &orig, const bool b_const, const AddressSpace address_space) : Type(b_const, address_space), root(orig)	{}
	public:
		StructType(const std::string &name) : Type(false, PRIVATE), name(name)	{}
		virtual bool operator==(const Type &type) const;
		virtual void write(std::ostream &out) const;
		virtual void define(std::ostream &out) const;
		virtual std::string getName() const;

		bool hasMember(const std::string &name) const;
		smartptr<Type> getTypeOfMember(const std::string &name) const;
		virtual smartptr<Type> clone(const bool b_const, const AddressSpace address_space) const;

		StructType &operator<<(const std::pair<std::string, smartptr<Type> > &member);

	protected:
		virtual AggregateType getAggregateType() const;

	protected:
		const std::string name;
		std::vector<std::pair<std::string, smartptr<Type> > > members;
		smartptr<StructType> root;
	};
}

#endif
