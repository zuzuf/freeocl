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
#ifndef __FREEOCL_PARSER_FUNCTION_H__
#define __FREEOCL_PARSER_FUNCTION_H__

#include <deque>
#include "node.h"
#include "chunk.h"
#include "type.h"
#include "callable.h"

namespace FreeOCL
{
	class Function : public Callable
	{
	public:
		Function(const smartptr<Type> &return_type, const std::string &name, const smartptr<Chunk> &arguments, const smartptr<Chunk> &body);
		virtual ~Function();

		virtual void write(std::ostream &out) const;

		virtual smartptr<Type> getReturnType() const;
		virtual const std::string &getName() const;
		virtual size_t getNumParams() const;

		smartptr<Chunk> getArguments() const	{	return arguments;	}
		smartptr<Chunk> getBody() const	{	return body;	}

	private:
		const std::string name;
		smartptr<Type>	return_type;
		smartptr<Chunk>	arguments;
		smartptr<Chunk>	body;
	};
}

#endif
