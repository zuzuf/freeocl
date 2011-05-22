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
#ifndef __FREEOCL_PARSER_TYPE_H__
#define __FREEOCL_PARSER_TYPE_H__

#include "node.h"

namespace FreeOCL
{
	class Type : public Node
	{
	public:
		enum AddressSpace
		{
			GLOBAL,
			LOCAL,
			PRIVATE,
			CONSTANT,
		};

	public:
		Type(const bool b_const, const AddressSpace address_space);

		virtual smartptr<Type> getType() const;

		virtual bool operator==(const Type &type) const = 0;
		bool operator!=(const Type &type) const
		{	return !(*this == type);	}
		virtual std::string getName() const = 0;

		bool isConst() const	{	return b_const;	}
		AddressSpace getAddressSpace() const	{	return address_space;	}

		virtual smartptr<Type> clone(const bool b_const, const AddressSpace address_space) const = 0;

	private:
		const bool b_const;
		const AddressSpace address_space;
	};
}

#endif
