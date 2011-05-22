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
#include "pointer_type.h"
#include "native_type.h"

namespace FreeOCL
{
	smartptr<Type> PointerType::t_p_const_char(new PointerType(new NativeType(NativeType::CHAR, true, Type::CONSTANT), true, Type::CONSTANT));

	bool PointerType::operator==(const Type &type) const
	{
		const PointerType *pType = dynamic_cast<const PointerType*>(&type);
		return pType && pType->base_type && base_type && *(pType->base_type) == *base_type && isConst() == pType->isConst();
	}

	void PointerType::write(std::ostream &out) const
	{
		out << getName() << ' ';
	}

	std::string PointerType::getName() const
	{
		if (!base_type)
			return isConst() ? "* const" : "*";

		if (isConst())
			return (' ' + base_type->getName()) + "* const";
		return (' ' + base_type->getName()) + '*';
	}

	bool PointerType::isCompatibleWith(const PointerType &type) const
	{
		if (!base_type || !type.base_type)
			return false;
		return type.base_type->getAddressSpace() == getAddressSpace();
	}

	smartptr<Type> PointerType::clone(const bool b_const, const AddressSpace address_space) const
	{
		return new PointerType(base_type, b_const, address_space);
	}

	void PointerType::setRootType(const smartptr<Type> &root_type)
	{
		PointerType *ptr = base_type.as<PointerType>();
		if (ptr)
			ptr->setRootType(root_type);
		else
			base_type = root_type;
	}
}
