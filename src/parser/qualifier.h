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
#ifndef __FREEOCL_PARSER_QUALIFIER_H__
#define __FREEOCL_PARSER_QUALIFIER_H__

#include <deque>
#include "node.h"
#include "type.h"

namespace FreeOCL
{
	class qualifier : public node
	{
	public:
		enum {
			KERNEL = 1,
			INLINE
		};
	public:
		qualifier(const int flags = 0);
		virtual ~qualifier();

		virtual void write(std::ostream &out) const;

		template<int F>
		inline bool is_set() const	{	return (flags & F) == F;	}

		template<int F>
		inline void set()	{	flags |= F;	}

		template<int F>
		inline void unset() {	flags &= ~F;	}

		inline int get_flags() const	{	return flags;	}

		virtual bool has_references_to(const std::string &function_name) const;
	private:
		int flags;
	};
}

#endif
