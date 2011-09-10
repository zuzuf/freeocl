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
#ifndef __FREEOCL_PARSER_CHUNK_H__
#define __FREEOCL_PARSER_CHUNK_H__

#include <deque>
#include "node.h"
#include "type.h"

namespace FreeOCL
{
	class Chunk : public Node
	{
	public:
		Chunk();
		Chunk(const smartptr<Node> &n0);
		Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1);
		Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2);
		Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3);
		Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3, const smartptr<Node> &n4);
		Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3, const smartptr<Node> &n4, const smartptr<Node> &n5);
		Chunk(const smartptr<Node> &n0, const smartptr<Node> &n1, const smartptr<Node> &n2, const smartptr<Node> &n3, const smartptr<Node> &n4, const smartptr<Node> &n5, const smartptr<Node> &n6);
		virtual ~Chunk();

		virtual void write(std::ostream &out) const;

		smartptr<Node> &operator[](size_t idx)	{	return childs[idx];	}
		const smartptr<Node> &operator[](size_t idx) const	{	return childs[idx];	}

		size_t size() const	{	return childs.size();	}

		void push_back(const smartptr<Node> &n)	{	childs.push_back(n);	}
		void push_front(const smartptr<Node> &n)	{	childs.push_front(n);	}
		void pop_back()	{	childs.pop_back();	}
		void pop_front()	{	childs.pop_front();	}

		smartptr<Node> &front()	{	return childs.front();	}
		const smartptr<Node> &front() const	{	return childs.front();	}
		smartptr<Node> &back()	{	return childs.back();	}
		const smartptr<Node> &back() const	{	return childs.back();	}

		Chunk &operator<<(const smartptr<Node> &n)
		{
			childs.push_back(n);
			return *this;
		}

		std::deque<smartptr<Type> > getAsTypes() const;

	private:
		std::deque<smartptr<Node> >	childs;
	};
}

#endif
