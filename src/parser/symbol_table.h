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
#ifndef __FREEOCL_PARSER_SYMBOL_TABLE_H__
#define __FREEOCL_PARSER_SYMBOL_TABLE_H__

#include "node.h"
#include <utils/map.h>
#include <utils/set.h>
#include <deque>
#include <string>

namespace FreeOCL
{
	class symbol_table
	{
		friend class parser;
	public:
		symbol_table();
		~symbol_table();

		void insert(const std::string &name, const smartptr<node> &symbol);
		template<class T>
		smartptr<T> get(const std::string &name) const
		{
			FreeOCL::map<std::string, std::deque<smartptr<node> > >::const_iterator it = table.find(name);
			if (it == table.end() || it->second.empty())
				return (T*)NULL;
			return it->second.back();
		}

		void push();
		void pop();

	private:
		FreeOCL::map<std::string, std::deque<smartptr<node> > >	table;
		std::deque<FreeOCL::set<std::string> >	scope_stack;
	};
}

#endif
