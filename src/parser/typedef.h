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
	class Typedef : public Type
	{
	public:
		Typedef(const std::string &name, const smartptr<Type> &type);

		virtual void write(std::ostream& out) const;

		virtual bool operator==(const Type &type) const;
		virtual std::string getName() const;
		virtual smartptr<Type> clone(const bool b_const, const AddressSpace address_space) const;

		inline const smartptr<Type> &getType() const
		{
			return type;
		}

	protected:
		const std::string name;
		const smartptr<Type> type;
	};

	class Typedecl : public Typedef
	{
	public:
		inline Typedecl(const smartptr<Type> &type)
			: Typedef(std::string(), type)
		{}
		virtual void write(std::ostream& out) const;
	};
}

#endif
