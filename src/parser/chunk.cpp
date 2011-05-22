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
#include "chunk.h"
#include "native_type.h"

namespace FreeOCL
{
	namespace
	{
		template<class T>
		inline std::deque<T> &operator<<(std::deque<T> &out, const T &v)
		{
			out.push_back(v);
			return out;
		}
	}

	Chunk::Chunk()
	{}

	Chunk::Chunk(const smartptr<Node> &n0)
	{
		childs << n0;
	}

	Chunk::Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1)
	{
		childs << n0 << n1;
	}

	Chunk::Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2)
	{
		childs << n0 << n1 << n2;
	}

	Chunk::Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3)
	{
		childs << n0 << n1 << n2 << n3;
	}

	Chunk::Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3, const smartptr<Node> &n4)
	{
		childs << n0 << n1 << n2 << n3 << n4;
	}

	Chunk::Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3, const smartptr<Node> &n4, const smartptr<Node> &n5)
	{
		childs << n0 << n1 << n2 << n3 << n4 << n5;
	}

	Chunk::Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3, const smartptr<Node> &n4, const smartptr<Node> &n5, const smartptr<Node> &n6)
	{
		childs << n0 << n1 << n2 << n3 << n4 << n5 << n6;
	}

	Chunk::~Chunk()
	{
	}

	void Chunk::write(std::ostream &out) const
	{
		for(std::deque<smartptr<Node> >::const_iterator it = childs.begin(), end = childs.end() ; it != end ; ++it)
			out << **it;
	}

	smartptr<Type> Chunk::getType() const
	{
		return NativeType::t_void;
	}
}
