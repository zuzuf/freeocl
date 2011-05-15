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
#ifndef __FREEOCL_PARSER_NODE_H__
#define __FREEOCL_PARSER_NODE_H__

#include <deque>
#include <string>
#include <stdint.h>
#include "../utils/smartptr.h"

namespace FreeOCL
{
	class Node
	{
	public:
		Node(const std::string &value = std::string());
		Node(const int64_t i);
		Node(const uint64_t ui);
		Node(const int32_t i);
		Node(const uint32_t ui);
		Node(const float f);
		Node(const double d);
		Node(const Node &n0);
		Node(const Node &n0, const Node &n1);
		Node(const Node &n0, const Node &n1, const Node &n2);
		Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3);
		Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3, const Node &n4);
		Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3, const Node &n4, const Node &n5);
		Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3, const Node &n4, const Node &n5, const Node &n6);
		~Node();

		Node &operator=(const Node &n);

		void push_back(const Node &n);
		void push_front(const Node &n);
		void pop_back();
		void pop_front();

		const std::deque<Node> &getChilds() const	{	return *childs;	}
		const std::string &getValue() const	{	return value;	}
		const Node &operator[](size_t idx) const	{	return (*childs)[idx];	}
		Node &operator[](size_t idx)	{	return (*childs)[idx];	}

		const Node &front() const	{	return childs->front();	}
		Node &front()	{	return childs->front();	}

		const Node &back() const	{	return childs->back();	}
		Node &back()	{	return childs->back();	}

		size_t size() const	{	return childs ? childs->size() : 0;	}

		Node clone() const;

		std::string toString() const;
	private:
		std::string	value;
		smartptr<std::deque<Node> >	childs;
	};
}

#endif
