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
#ifndef __FREEOCL_PARSER_POINTER_TYPE_H__
#define __FREEOCL_PARSER_POINTER_TYPE_H__

#include "type.h"

namespace FreeOCL
{
	class PointerType : public Type
	{
	public:
		PointerType(const smartptr<Type> &base_type, const bool b_const, const AddressSpace address_space) : Type(b_const, address_space), base_type(base_type)	{}
		virtual bool operator==(const Type &type) const;
		virtual void write(std::ostream &out) const;
		virtual std::string getName() const;

		const smartptr<Type> &getBaseType() const	{	return base_type;	}
		bool isCompatibleWith(const PointerType &type) const;

		virtual smartptr<Type> clone(const bool b_const, const AddressSpace address_space) const;
		smartptr<Type> clone() const;

		void setRootType(const smartptr<Type> &root_type);
	private:
		smartptr<Type> base_type;

	public:
		static smartptr<Type> t_p_const_char;
	};
}

#endif
