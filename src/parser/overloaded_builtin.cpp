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
#include "overloaded_builtin.h"
#include "native_type.h"
#include "pointer_type.h"
#include "../freeocl.h"
#include <unordered_map>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace FreeOCL
{
	namespace
	{
		static const char *alphanum_ = "0123456789abcdefghijklmnopqrstuvwxyz_";
		inline string readWord(const string &str, const char *chars, size_t &pos)
		{
			const size_t n = str.find_first_not_of(chars, pos);
			const size_t opos = pos;
			pos = n;
			return str.substr(opos, n - opos);
		}

		smartptr<Type> parseType(const string &str, size_t &pos, const int gentype)
		{
			static unordered_map<string, int> m_types;
			static unordered_map<string, int> m_types_pattern;
			static unordered_map<string, int> m_addr_space;
			if (m_types.empty())
			{
				m_types["bool"] = NativeType::BOOL;
				m_types["half"] = NativeType::HALF;
				m_types["size_t"] = NativeType::SIZE_T;
				m_types["char"] = NativeType::CHAR;
				m_types["short"] = NativeType::SHORT;
				m_types["int"] = NativeType::INT;
				m_types["long"] = NativeType::LONG;
				m_types["uchar"] = NativeType::UCHAR;
				m_types["ushort"] = NativeType::USHORT;
				m_types["uint"] = NativeType::UINT;
				m_types["ulong"] = NativeType::ULONG;
				m_types["float"] = NativeType::FLOAT;
				m_types["double"] = NativeType::DOUBLE;
				m_types["char2"] = NativeType::CHAR2;
				m_types["short2"] = NativeType::SHORT2;
				m_types["int2"] = NativeType::INT2;
				m_types["long2"] = NativeType::LONG2;
				m_types["uchar2"] = NativeType::UCHAR2;
				m_types["ushort2"] = NativeType::USHORT2;
				m_types["uint2"] = NativeType::UINT2;
				m_types["ulong2"] = NativeType::ULONG2;
				m_types["float2"] = NativeType::FLOAT2;
				m_types["double2"] = NativeType::DOUBLE2;
				m_types["char3"] = NativeType::CHAR3;
				m_types["short3"] = NativeType::SHORT3;
				m_types["int3"] = NativeType::INT3;
				m_types["long3"] = NativeType::LONG3;
				m_types["uchar3"] = NativeType::UCHAR3;
				m_types["ushort3"] = NativeType::USHORT3;
				m_types["uint3"] = NativeType::UINT3;
				m_types["ulong3"] = NativeType::ULONG3;
				m_types["float3"] = NativeType::FLOAT3;
				m_types["double3"] = NativeType::DOUBLE3;
				m_types["char4"] = NativeType::CHAR4;
				m_types["short4"] = NativeType::SHORT4;
				m_types["int4"] = NativeType::INT4;
				m_types["long4"] = NativeType::LONG4;
				m_types["uchar4"] = NativeType::UCHAR4;
				m_types["ushort4"] = NativeType::USHORT4;
				m_types["uint4"] = NativeType::UINT4;
				m_types["ulong4"] = NativeType::ULONG4;
				m_types["float4"] = NativeType::FLOAT4;
				m_types["double4"] = NativeType::DOUBLE4;
				m_types["char8"] = NativeType::CHAR8;
				m_types["short8"] = NativeType::SHORT8;
				m_types["int8"] = NativeType::INT8;
				m_types["long8"] = NativeType::LONG8;
				m_types["uchar8"] = NativeType::UCHAR8;
				m_types["ushort8"] = NativeType::USHORT8;
				m_types["uint8"] = NativeType::UINT8;
				m_types["ulong8"] = NativeType::ULONG8;
				m_types["float8"] = NativeType::FLOAT8;
				m_types["double8"] = NativeType::DOUBLE8;
				m_types["char16"] = NativeType::CHAR16;
				m_types["short16"] = NativeType::SHORT16;
				m_types["int16"] = NativeType::INT16;
				m_types["long16"] = NativeType::LONG16;
				m_types["uchar16"] = NativeType::UCHAR16;
				m_types["ushort16"] = NativeType::USHORT16;
				m_types["uint16"] = NativeType::UINT16;
				m_types["ulong16"] = NativeType::ULONG16;
				m_types["float16"] = NativeType::FLOAT16;
				m_types["double16"] = NativeType::DOUBLE16;

				m_types_pattern["charn"] = NativeType::CHAR;
				m_types_pattern["shortn"] = NativeType::SHORT;
				m_types_pattern["intn"] = NativeType::INT;
				m_types_pattern["longn"] = NativeType::LONG;
				m_types_pattern["ucharn"] = NativeType::UCHAR;
				m_types_pattern["ushortn"] = NativeType::USHORT;
				m_types_pattern["uintn"] = NativeType::UINT;
				m_types_pattern["ulongn"] = NativeType::ULONG;
				m_types_pattern["floatn"] = NativeType::FLOAT;
				m_types_pattern["doublen"] = NativeType::DOUBLE;

				m_addr_space["__global"] = Type::GLOBAL;
				m_addr_space["__local"] = Type::LOCAL;
				m_addr_space["__constant"] = Type::CONSTANT;
				m_addr_space["__private"] = Type::PRIVATE;
				m_addr_space["global"] = Type::GLOBAL;
				m_addr_space["local"] = Type::LOCAL;
				m_addr_space["constant"] = Type::CONSTANT;
				m_addr_space["private"] = Type::PRIVATE;
			}
			const int n = NativeType::getDimFor(gentype);
			bool b_pointer = false;
			bool b_const = false;
			Type::AddressSpace address_space = Type::PRIVATE;
			int typeID = 0;
			while(pos < str.size() && str[pos] != ')' && str[pos] != '(' && str[pos] != ',')
			{
				while (pos < str.size() && str[pos] == ' ')
					++pos;
				if (str[pos] == '*')
				{
					b_pointer = true;
					break;
				}
				const string word = readWord(str, alphanum_, pos);

				unordered_map<string, int>::const_iterator it = m_types.find(word);
				if (it != m_types.end())
					typeID = it->second;
				else
				{
					it = m_types_pattern.find(word);
					if (it != m_types_pattern.end())
					{
						stringstream buf;
						buf << word.substr(0, word.size() - 1);
						if (n > 1)
							buf << n;
						typeID = m_types[buf.str()];
					}
					else
					{
						it = m_addr_space.find(word);
						if (it != m_addr_space.end())
							address_space = (Type::AddressSpace)it->second;
						else
						{
							if (word == "gentype")
								typeID = gentype;
							else if (word == "const")
								b_const = true;
						}
					}
				}
			}
			smartptr<Type> type = new NativeType((NativeType::TypeID)typeID, b_const, address_space);
			if (b_pointer)
				type = new PointerType(type, false, Type::PRIVATE);
			return type;
		}
	}


	OverloadedBuiltin::OverloadedBuiltin(const string &signature, const deque<int> &gentype)
	{
		deque<int> gentypes = gentype;
		if (gentypes.empty())
			gentypes.push_back(NativeType::INT);
		const deque<string> patterns = FreeOCL::split(signature, "|");
		const size_t e = patterns.front().find_first_of('(');
		const size_t b = patterns.front().rfind(' ', e);
		if (b == string::npos)
			name = patterns.front().substr(0, e);
		else
			name = patterns.front().substr(b + 1, e - b - 1);
		for(deque<int>::const_iterator gtype = gentypes.begin(), end = gentypes.end() ; gtype != end ; ++gtype)
		{
			for(deque<string>::const_iterator p = patterns.begin(), end = patterns.end() ; p != end ; ++p)
			{
				size_t pos = 0;
				deque<smartptr<Type> > types;
				while((*p)[pos] != ')')
				{
					if (pos)	++pos;
					types.push_back(parseType(*p, pos, *gtype));
				}
				possible_types.push_back(types);
			}
		}

		num_params = possible_types.front().size() - 1;
	}

	OverloadedBuiltin::~OverloadedBuiltin()
	{
	}

	smartptr<Type> OverloadedBuiltin::getReturnType(const deque<smartptr<Type> > &arg_types) const
	{
		if (arg_types.size() != num_params)
			return (Type*)NULL;

		smartptr<Type> match;
		for(size_t i = 0 ; i < possible_types.size() ; ++i)
		{
			if (allTypesMatch(arg_types, possible_types[i]))
			{
				if (match)
					throw std::runtime_error("type matching is ambiguous");
				match = possible_types[i].front();
			}
		}

		return match;
	}

	const string &OverloadedBuiltin::getName() const
	{
		return name;
	}

	size_t OverloadedBuiltin::getNumParams() const
	{
		return num_params;
	}

	void OverloadedBuiltin::write(ostream& out) const
	{
		out << name << ' ';
	}

	bool OverloadedBuiltin::allTypesMatch(const std::deque<smartptr<Type> > &args, const std::deque<smartptr<Type> > &sig)
	{
		if (args.size() + 1 != sig.size())
			return false;
		for(size_t i = 0 ; i < args.size() ; ++i)
		{
			if (!weakMatch(args[i], sig[i + 1]))
				return false;
		}
		return true;
	}

	bool OverloadedBuiltin::weakMatch(const smartptr<Type> &a, const smartptr<Type> &b)
	{
		if (*a == *b)
			return true;

		const PointerType *ptrA = a.as<PointerType>();
		const PointerType *ptrB = b.as<PointerType>();
		if (ptrA && ptrB)
			return ptrA->isCompatibleWith(*ptrB);

		const NativeType *natA = a.as<NativeType>();
		const NativeType *natB = b.as<NativeType>();
		if (natA && natB)
		{
			return natA->getTypeID() == natB->getTypeID()
					|| (natA->isScalar() && natB->isScalar());
		}
		return false;
	}

	void OverloadedBuiltin::printDebugInfo() const
	{
		std::cout << possible_types.size() << " signatures:" << std::endl;
		for(size_t i = 0 ; i < possible_types.size() ; ++i)
		{
			std::cout << possible_types[i].front()->getName() << ' ' << name << '(';
			for(size_t j = 1 ; j < possible_types[i].size() ; ++j)
			{
				if (j > 1)
					std::cout << ',';
				std::cout << possible_types[i][j]->getName();
			}
			std::cout << ')' << std::endl;
		}
	}
}
