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
#ifndef __FREEOCL_PARSER_NODE_H__
#define __FREEOCL_PARSER_NODE_H__

#include <ostream>
#include "../utils/smartptr.h"

namespace FreeOCL
{
	class Type;

	class Node : public ref_count
	{
	public:
		virtual ~Node()	{}

		virtual void write(std::ostream& out) const = 0;

		virtual smartptr<Type> getType() const = 0;
	};

	inline std::ostream &operator<<(std::ostream &out, const Node &n)
	{
		n.write(out);
		return out;
	}
}

#endif
