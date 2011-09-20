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
#include "symbol_table.h"
#include "overloaded_builtin.h"

namespace FreeOCL
{
	symbol_table::symbol_table()
	{
		push();
	}

	symbol_table::~symbol_table()
	{
	}

	void symbol_table::insert(const std::string &name, const smartptr<node> &symbol)
	{
		if (!scope_stack.back().count(name))
		{
			scope_stack.back().insert(name);
			table[name].push_back(symbol);
		}
		else
		{
			if (symbol.as<overloaded_builtin>() && table[name].back().as<overloaded_builtin>())
				table[name].back().as<overloaded_builtin>()->merge(symbol);
			else
				table[name].back() = symbol;
		}
	}

	void symbol_table::push()
	{
		scope_stack.push_back(FreeOCL::set<std::string>());
	}

	void symbol_table::pop()
	{
		for(FreeOCL::set<std::string>::const_iterator it = scope_stack.back().begin(), end = scope_stack.back().end()
			; it != end
			; ++it)
			table[*it].pop_back();
		scope_stack.pop_back();
	}
}
