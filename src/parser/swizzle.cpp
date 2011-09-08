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
#include "swizzle.h"
#include "native_type.h"

namespace FreeOCL
{
	Swizzle::Swizzle(const smartptr<Expression> &base, const std::string &components)
		: base(base), components(components)
	{
	}

	void Swizzle::write(std::ostream& out) const
	{
		smartptr<Type> type = getType();
		int values[16];
		parseComponents(components, values);
		if (type.as<NativeType>()->isScalar())
			out << '(' << *base << ").get<" << values[0] << ">() ";
		else
		{
			out << '(' << *base << ").swizzle<";
			out << *type << ',';
			for(size_t i = 0 ; i < 15 ; ++i)
				out << values[i] << ',';
			out << values[15];
			out << ">() ";
		}
	}

	smartptr<Type> Swizzle::getType() const
	{
		smartptr<NativeType> type = base->getType().as<NativeType>();
		int dim = getNumberOfComponents(components);
		if (dim == -1)
			dim = (type->getDim() + 1) / 2;
		return NativeType::makeVectorType(type->getScalarType(), dim)->clone(type->isConst(), type->getAddressSpace());
	}

	int Swizzle::getNumberOfComponents(const std::string &components)
	{
		if (components.empty())
			return 0;
		if (components[0] == 's' || components[0] == 'S')
			return components.size() - 1;
		if (components == "lo"
			|| components == "hi"
			|| components == "even"
			|| components == "odd")
			return -1;
		return components.size();
	}

	bool Swizzle::validateComponents(const std::string &components, int dim)
	{
		const int nc = getNumberOfComponents(components);
		if (nc == -1)		// lo, hi, odd, even
			return true;
		if (nc == 0)		// zero components oO ?
			return false;
		if (nc > dim)
			return false;
		switch(nc)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 8:
		case 16:
			break;
		default:
			return false;		// Invalid vector type
		}

		int m = 0;
		switch(components[0])
		{
		case 'S':
		case 's':
			for(size_t i = 1 ; i < components.size() ; ++i)
				switch(components[i])
				{
				case '0':	m = std::max(m, 0);	break;
				case '1':	m = std::max(m, 1);	break;
				case '2':	m = std::max(m, 2);	break;
				case '3':	m = std::max(m, 3);	break;
				case '4':	m = std::max(m, 4);	break;
				case '5':	m = std::max(m, 5);	break;
				case '6':	m = std::max(m, 6);	break;
				case '7':	m = std::max(m, 7);	break;
				case '8':	m = std::max(m, 8);	break;
				case '9':	m = std::max(m, 9);	break;
				case 'a':
				case 'A':	m = std::max(m, 10);	break;
				case 'b':
				case 'B':	m = std::max(m, 11);	break;
				case 'c':
				case 'C':	m = std::max(m, 12);	break;
				case 'd':
				case 'D':	m = std::max(m, 13);	break;
				case 'e':
				case 'E':	m = std::max(m, 14);	break;
				case 'f':
				case 'F':	m = std::max(m, 15);	break;
				default:	return false;				// unexpected character
				}
			break;
		case 'x':
		case 'y':
		case 'z':
		case 'w':
		case 'X':
		case 'Y':
		case 'Z':
		case 'W':
			for(size_t i = 0 ; i < components.size() ; ++i)
				switch(components[i])
				{
				case 'X':
				case 'x':	m = std::max(m, 0);	break;
				case 'Y':
				case 'y':	m = std::max(m, 1);	break;
				case 'Z':
				case 'z':	m = std::max(m, 2);	break;
				case 'W':
				case 'w':	m = std::max(m, 3);	break;
				default:	return false;				// unexpected character
				}
			break;
		}
		if (m >= dim)
			return false;

		return true;
	}

	void Swizzle::parseComponents(const std::string &components, int values[])
	{
		for(size_t i = 0 ; i < 16 ; ++i)
			values[i] = -1;
		int *p = values;
		switch(components[0])
		{
		case 'S':
		case 's':
			for(size_t i = 1 ; i < components.size() ; ++i, ++p)
				switch(components[i])
				{
				case '0':	*p = 0;	break;
				case '1':	*p = 1;	break;
				case '2':	*p = 2;	break;
				case '3':	*p = 3;	break;
				case '4':	*p = 4;	break;
				case '5':	*p = 5;	break;
				case '6':	*p = 6;	break;
				case '7':	*p = 7;	break;
				case '8':	*p = 8;	break;
				case '9':	*p = 9;	break;
				case 'a':
				case 'A':	*p = 10;	break;
				case 'b':
				case 'B':	*p = 11;	break;
				case 'c':
				case 'C':	*p = 12;	break;
				case 'd':
				case 'D':	*p = 13;	break;
				case 'e':
				case 'E':	*p = 14;	break;
				case 'f':
				case 'F':	*p = 15;	break;
				default:	return;				// unexpected character
				}
			break;
		case 'x':
		case 'y':
		case 'z':
		case 'w':
		case 'X':
		case 'Y':
		case 'Z':
		case 'W':
			for(size_t i = 0 ; i < components.size() ; ++i, ++p)
				switch(components[i])
				{
				case 'X':
				case 'x':	*p = 0;	break;
				case 'Y':
				case 'y':	*p = 1;	break;
				case 'Z':
				case 'z':	*p = 2;	break;
				case 'W':
				case 'w':	*p = 3;	break;
				default:	return;				// unexpected character
				}
			break;
		}
	}
}
