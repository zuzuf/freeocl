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
#include "binary.h"
#include "parser.h"
#include "native_type.h"

namespace FreeOCL
{
	Binary::Binary(int op, const smartptr<Expression> &left, const smartptr<Expression> &right)
		: left(left),
		right(right),
		op(op)
	{
		const smartptr<Type> t0 = left->getType();
		const smartptr<Type> t1 = right->getType();
		switch(op)
		{
		case '+':
		case '-':
		case '*':
		case '/':
			type = Type::computeResultingType(t0, t1);
			break;

		case ',':
			type = t1;
			break;

		case '%':
		case '|':
		case '^':
		case '&':
		case Parser::LEFT_OP:
		case Parser::RIGHT_OP:
			type = Type::computeResultingType(t0, t1);
			break;

		case '<':
		case '>':
		case Parser::LE_OP:
		case Parser::GE_OP:
		case Parser::EQ_OP:
		case Parser::NE_OP:
		case Parser::AND_OP:
		case Parser::OR_OP:
			type = NativeType::getIntForDim(std::max(t0.as<NativeType>()->getDim(), t1.as<NativeType>()->getDim()));
			break;

		case '=':
		case Parser::MUL_ASSIGN:
		case Parser::DIV_ASSIGN:
		case Parser::MOD_ASSIGN:
		case Parser::ADD_ASSIGN:
		case Parser::SUB_ASSIGN:
		case Parser::LEFT_ASSIGN:
		case Parser::RIGHT_ASSIGN:
		case Parser::AND_ASSIGN:
		case Parser::XOR_ASSIGN:
		case Parser::OR_ASSIGN:
			type = t0;
			break;
		}
	}

	Binary::~Binary()
	{
	}

	void Binary::write(std::ostream &out) const
	{
		out << '(' <<  *left << ' ';

		switch(op)
		{
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '|':
		case '^':
		case '&':
		case '<':
		case '>':
		case '=':
		case ',':
			out << (char)op;
			break;
		case Parser::LEFT_OP:		out << "<<";	break;
		case Parser::RIGHT_OP:		out << ">>";	break;
		case Parser::LE_OP:			out << "<=";	break;
		case Parser::GE_OP:			out << ">=";	break;
		case Parser::EQ_OP:			out << "==";	break;
		case Parser::NE_OP:			out << "!=";	break;
		case Parser::AND_OP:		out << "&&";	break;
		case Parser::OR_OP:			out << "||";	break;
		case Parser::MUL_ASSIGN:	out << "*=";	break;
		case Parser::DIV_ASSIGN:	out << "/=";	break;
		case Parser::MOD_ASSIGN:	out << "%=";	break;
		case Parser::ADD_ASSIGN:	out << "+=";	break;
		case Parser::SUB_ASSIGN:	out << "-=";	break;
		case Parser::LEFT_ASSIGN:	out << "<<=";	break;
		case Parser::RIGHT_ASSIGN:	out << ">>=";	break;
		case Parser::AND_ASSIGN:	out << "&=";	break;
		case Parser::XOR_ASSIGN:	out << "^=";	break;
		case Parser::OR_ASSIGN:		out << "|=";	break;
		}

		out << ' ' << *right << ')';
	}

	smartptr<Type> Binary::getType() const
	{
		return type;
	}
}
