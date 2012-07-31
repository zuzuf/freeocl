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
#ifndef __FREEOCL_PARSER_WHILE_H__
#define __FREEOCL_PARSER_WHILE_H__

#include "expression.h"

namespace FreeOCL
{
	class _while : public node
	{
	public:
		_while(const smartptr<expression> &test, const smartptr<node> &stmt);

		virtual void write(std::ostream& out) const;

		virtual bool has_references_to(const std::string &function_name) const;

        virtual const char *get_node_type() const;
    private:
		const smartptr<expression> test;
		const smartptr<node> stmt;
	};
}

#endif
