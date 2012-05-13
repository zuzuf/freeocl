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
#ifndef __FREEOCL_PARSER_VALUE_H__
#define __FREEOCL_PARSER_VALUE_H__

#include "expression.h"
#include <stdint.h>

namespace FreeOCL
{
	class generic_value : public expression
	{
	public:
		virtual uint32_t get_as_uint() const = 0;
		virtual uint32_t eval_as_uint() const;
	};

	template<class T>
	class value : public generic_value
	{
	public:
		value(const T &v) : v(v)	{}
		virtual ~value()	{}

		const T &get_value() const	{	return v;	}

		virtual uint32_t get_as_uint() const;

		virtual smartptr<type> get_type() const;

		virtual void write(std::ostream& out) const
		{
			out << v << ' ';
		}
	private:
		const T v;
	};
}

#endif
