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
	SymbolTable::SymbolTable()
	{
		push();
	}

	SymbolTable::~SymbolTable()
	{
	}

	void SymbolTable::insert(const std::string &name, const smartptr<Node> &symbol)
	{
		if (!scope_stack.back().count(name))
		{
			scope_stack.back().insert(name);
			table[name].push_back(symbol);
		}
		else
		{
			if (symbol.as<OverloadedBuiltin>() && table[name].back().as<OverloadedBuiltin>())
				table[name].back().as<OverloadedBuiltin>()->merge(symbol);
			else
				table[name].back() = symbol;
		}
	}

	void SymbolTable::push()
	{
		scope_stack.push_back(std::unordered_set<std::string>());
	}

	void SymbolTable::pop()
	{
		for(std::unordered_set<std::string>::const_iterator it = scope_stack.back().begin(), end = scope_stack.back().end()
			; it != end
			; ++it)
			table[*it].pop_back();
		scope_stack.pop_back();
	}
}
