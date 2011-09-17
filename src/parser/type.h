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
	class type : public node
	{
	public:
		enum address_space
		{
			GLOBAL,
			LOCAL,
			PRIVATE,
			CONSTANT,
		};

	public:
		type(const bool b_const, const address_space addr_space);

		virtual bool operator==(const type &t) const = 0;
		bool operator!=(const type &t) const
		{	return !(*this == t);	}
		virtual std::string get_name() const = 0;

		bool is_const() const	{	return b_const;	}
		address_space get_address_space() const	{	return addr_space;	}

		virtual smartptr<type> clone(const bool b_const, const address_space addr_space) const = 0;

	private:
		const bool b_const;
		const address_space addr_space;

	public:
		static smartptr<type> compute_resulting_type(const smartptr<type> &t0, const smartptr<type> &t1);
	};
}

#endif
