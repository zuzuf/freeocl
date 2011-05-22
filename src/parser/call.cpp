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
#include "call.h"

namespace FreeOCL
{
	Call::Call(const smartptr<Callable> &fn, const smartptr<Chunk> &args)
		: fn(fn),
		args(args)
	{
	}

	Call::~Call()
	{
	}

	smartptr<Type> Call::getType() const
	{
		std::deque<smartptr<Type> > arg_types;
		if (args)
			for(size_t i = 0 ; i < args->size() ; ++i)
				arg_types.push_back((*args)[i].as<Expression>()->getType());
		return fn->getReturnType(arg_types);
	}

	void Call::write(std::ostream& out) const
	{
		out << fn->getName() << '(';
		if (args)
		{
			for(size_t i = 0 ; i < args->size() ; ++i)
			{
				if (i)
					out << ',';
				out << *((*args)[i]);
			}
		}
		out << ')';
	}
}
