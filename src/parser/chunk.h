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
	class chunk : public node
	{
	public:
		chunk();
		chunk(const smartptr<node> &n0);
		chunk(const smartptr<node> &n0, const smartptr<node> &n1);
		chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2);
		chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3);
		chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3, const smartptr<node> &n4);
		chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3, const smartptr<node> &n4, const smartptr<node> &n5);
		chunk(const smartptr<node> &n0, const smartptr<node> &n1, const smartptr<node> &n2, const smartptr<node> &n3, const smartptr<node> &n4, const smartptr<node> &n5, const smartptr<node> &n6);
		virtual ~chunk();

		virtual void write(std::ostream &out) const;

		smartptr<node> &operator[](size_t idx)	{	return childs[idx];	}
		const smartptr<node> &operator[](size_t idx) const	{	return childs[idx];	}
		smartptr<node> &at(size_t idx)	{	return childs[idx];	}
		const smartptr<node> &at(size_t idx) const	{	return childs[idx];	}

		size_t size() const	{	return childs.size();	}

		void push_back(const smartptr<node> &n)	{	childs.push_back(n);	}
		void push_front(const smartptr<node> &n)	{	childs.push_front(n);	}
		void pop_back()	{	childs.pop_back();	}
		void pop_front()	{	childs.pop_front();	}

		smartptr<node> &front()	{	return childs.front();	}
		const smartptr<node> &front() const	{	return childs.front();	}
		smartptr<node> &back()	{	return childs.back();	}
		const smartptr<node> &back() const	{	return childs.back();	}

		chunk &operator<<(const smartptr<node> &n)
		{
			childs.push_back(n);
			return *this;
		}

		std::deque<smartptr<type> > get_as_types() const;

	private:
		std::deque<smartptr<node> >	childs;
	};
}

#endif
