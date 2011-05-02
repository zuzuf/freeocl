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
#include "node.h"
#include <sstream>
#include "../utils/string.h"

namespace
{
	inline std::deque<FreeOCL::Node> &operator<<(std::deque<FreeOCL::Node> &childs, const FreeOCL::Node &n)
	{
		childs.push_back(n);
		return childs;
	}
}

namespace FreeOCL
{
	Node::Node(const std::string &value) : value(value)
	{
	}

	Node::Node(const int32_t i) : value(FreeOCL::toString(i))
	{
	}

	Node::Node(const uint32_t ui) : value(FreeOCL::toString(ui))
	{
	}

	Node::Node(const int64_t i) : value(FreeOCL::toString(i))
	{
	}

	Node::Node(const uint64_t ui) : value(FreeOCL::toString(ui))
	{
	}

	Node::Node(const float f) : value(FreeOCL::toString(f))
	{
	}

	Node::Node(const double d) : value(FreeOCL::toString(d))
	{
	}

	Node::Node(const Node &n0) : value(n0.value), childs(n0.childs)
	{
	}

	Node::Node(const Node &n0, const Node &n1) : childs(new std::deque<Node>)
	{
		*childs << n0 << n1;
	}

	Node::Node(const Node &n0, const Node &n1, const Node &n2) : childs(new std::deque<Node>)
	{
		*childs << n0 << n1 << n2;
	}

	Node::Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3) : childs(new std::deque<Node>)
	{
		*childs << n0 << n1 << n2 << n3;
	}

	Node::Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3, const Node &n4) : childs(new std::deque<Node>)
	{
		*childs << n0 << n1 << n2 << n3 << n4;
	}

	Node::Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3, const Node &n4, const Node &n5) : childs(new std::deque<Node>)
	{
		*childs << n0 << n1 << n2 << n3 << n4 << n5;
	}

	Node::Node(const Node &n0, const Node &n1, const Node &n2, const Node &n3, const Node &n4, const Node &n5, const Node &n6) : childs(new std::deque<Node>)
	{
		*childs << n0 << n1 << n2 << n3 << n4 << n5 << n6;
	}

	Node::~Node()
	{

	}

	void Node::push_back(const Node &n)
	{
		if (childs)
			childs->push_back(n);
	}

	void Node::push_front(const Node &n)
	{
		if (childs)
			childs->push_front(n);
	}

	void Node::pop_back()
	{
		if (childs)
			childs->pop_back();
	}

	void Node::pop_front()
	{
		if (childs)
			childs->pop_front();
	}

	std::string Node::toString() const
	{
		std::stringstream tmp;
		tmp << value;
		if (value == "{" || value == "}" || value == ";")
			tmp << std::endl;
		if (childs && !childs->empty())
		{
			if (!value.empty())
				tmp << ' ';
			for(std::deque<Node>::const_iterator i = childs->begin(), end = childs->end() ; i != end ; ++i)
			{
				tmp << i->toString();
				if (i + 1 != end)
					tmp << ' ';
			}
		}
		return tmp.str();
	}

	Node Node::clone() const
	{
		Node copy(value);
		copy.childs = new std::deque<Node>;
		*copy.childs = *childs;
		return copy;
	}
}
