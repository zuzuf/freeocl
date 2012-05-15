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
#ifndef __FREEOCL_PARSER_MEMBER_H__
#define __FREEOCL_PARSER_MEMBER_H__

#include "expression.h"

namespace FreeOCL
{
	class member : public expression
	{
	public:
		member(const smartptr<expression> &base, const std::string &member_name);
		void write(std::ostream& out) const;
		smartptr<type> get_type() const;

		virtual uint32_t eval_as_uint() const;

		virtual bool has_references_to(const std::string &function_name) const;
	private:
		smartptr<expression> base;
		const std::string member_name;
	};
}

#endif
