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
#ifndef __FREEOCL_PARSER_VAR_H__
#define __FREEOCL_PARSER_VAR_H__

#include "expression.h"

namespace FreeOCL
{
	class var : public expression
	{
	public:
		var(const std::string &name, const smartptr<type> &p_type);
		virtual ~var();
		virtual smartptr<type> get_type() const;
		virtual void write(std::ostream& out) const;

		const std::string &get_name() const	{	return name;	}

		virtual uint32_t eval_as_uint() const;
	private:
		const std::string name;
		const smartptr<type> p_type;
	};
}

#endif
