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
#ifndef __FREEOCL_PARSER_OVERLOADED_BUILTIN_H__
#define __FREEOCL_PARSER_OVERLOADED_BUILTIN_H__

#include "callable.h"

namespace FreeOCL
{
	class OverloadedBuiltin : public Callable
	{
	public:
		OverloadedBuiltin(const std::string &signature, const std::deque<int> &gentype);
		virtual ~OverloadedBuiltin();

		virtual smartptr<Type> getReturnType(const std::deque<smartptr<Type> > &arg_types) const;
		virtual const std::string &getName() const;
		virtual size_t getNumParams() const;

		virtual void write(std::ostream& out) const;
	private:
		std::deque<std::deque<smartptr<Type> > > possible_types;
		std::string name;
		size_t num_params;
	};
}

#endif
