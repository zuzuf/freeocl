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
#ifndef __FREEOCL_PARSER_UNION_TYPE_H__
#define __FREEOCL_PARSER_UNION_TYPE_H__

#include "struct_type.h"

namespace FreeOCL
{
	class union_type : public struct_type
	{
	public:
		union_type(const std::string &name) : struct_type(name)	{}

        virtual const char *get_node_type() const;
    private:
		virtual aggregate_type get_aggregate_type() const;
	};
}

#endif
