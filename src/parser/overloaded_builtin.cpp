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
#include "freeocl.h"
#include <utils/map.h>
#include <sstream>
#include <stdexcept>
#include <utils/set.h>

using namespace std;

namespace FreeOCL
{
	namespace
	{
		static const char *alphanum_ = "0123456789abcdefghijklmnopqrstuvwxyz_";
		inline string read_word(const string &str, const char *chars, size_t &pos)
		{
			const size_t n = str.find_first_not_of(chars, pos);
			const size_t opos = pos;
			pos = n;
			return str.substr(opos, n - opos);
		}

		inline smartptr<type> parse_type(const string &str, size_t &pos, const int gentype)
		{
			static FreeOCL::map<string, int> m_types;
			static FreeOCL::map<string, int> m_types_pattern;
			static FreeOCL::map<string, int> m_addr_space;
			if (m_types.empty())
			{
				m_types["bool"] = native_type::BOOL;
				m_types["half"] = native_type::HALF;
				m_types["size_t"] = native_type::SIZE_T;
				m_types["char"] = native_type::CHAR;
				m_types["short"] = native_type::SHORT;
				m_types["int"] = native_type::INT;
				m_types["long"] = native_type::LONG;
				m_types["uchar"] = native_type::UCHAR;
				m_types["ushort"] = native_type::USHORT;
				m_types["uint"] = native_type::UINT;
				m_types["ulong"] = native_type::ULONG;
				m_types["float"] = native_type::FLOAT;
				m_types["double"] = native_type::DOUBLE;
				m_types["char2"] = native_type::CHAR2;
				m_types["short2"] = native_type::SHORT2;
				m_types["int2"] = native_type::INT2;
				m_types["long2"] = native_type::LONG2;
				m_types["uchar2"] = native_type::UCHAR2;
				m_types["ushort2"] = native_type::USHORT2;
				m_types["uint2"] = native_type::UINT2;
				m_types["ulong2"] = native_type::ULONG2;
				m_types["float2"] = native_type::FLOAT2;
				m_types["double2"] = native_type::DOUBLE2;
				m_types["char3"] = native_type::CHAR3;
				m_types["short3"] = native_type::SHORT3;
				m_types["int3"] = native_type::INT3;
				m_types["long3"] = native_type::LONG3;
				m_types["uchar3"] = native_type::UCHAR3;
				m_types["ushort3"] = native_type::USHORT3;
				m_types["uint3"] = native_type::UINT3;
				m_types["ulong3"] = native_type::ULONG3;
				m_types["float3"] = native_type::FLOAT3;
				m_types["double3"] = native_type::DOUBLE3;
				m_types["char4"] = native_type::CHAR4;
				m_types["short4"] = native_type::SHORT4;
				m_types["int4"] = native_type::INT4;
				m_types["long4"] = native_type::LONG4;
				m_types["uchar4"] = native_type::UCHAR4;
				m_types["ushort4"] = native_type::USHORT4;
				m_types["uint4"] = native_type::UINT4;
				m_types["ulong4"] = native_type::ULONG4;
				m_types["float4"] = native_type::FLOAT4;
				m_types["double4"] = native_type::DOUBLE4;
				m_types["char8"] = native_type::CHAR8;
				m_types["short8"] = native_type::SHORT8;
				m_types["int8"] = native_type::INT8;
				m_types["long8"] = native_type::LONG8;
				m_types["uchar8"] = native_type::UCHAR8;
				m_types["ushort8"] = native_type::USHORT8;
				m_types["uint8"] = native_type::UINT8;
				m_types["ulong8"] = native_type::ULONG8;
				m_types["float8"] = native_type::FLOAT8;
				m_types["double8"] = native_type::DOUBLE8;
				m_types["char16"] = native_type::CHAR16;
				m_types["short16"] = native_type::SHORT16;
				m_types["int16"] = native_type::INT16;
				m_types["long16"] = native_type::LONG16;
				m_types["uchar16"] = native_type::UCHAR16;
				m_types["ushort16"] = native_type::USHORT16;
				m_types["uint16"] = native_type::UINT16;
				m_types["ulong16"] = native_type::ULONG16;
				m_types["float16"] = native_type::FLOAT16;
				m_types["double16"] = native_type::DOUBLE16;

				m_types_pattern["charn"] = native_type::CHAR;
				m_types_pattern["shortn"] = native_type::SHORT;
				m_types_pattern["intn"] = native_type::INT;
				m_types_pattern["longn"] = native_type::LONG;
				m_types_pattern["ucharn"] = native_type::UCHAR;
				m_types_pattern["ushortn"] = native_type::USHORT;
				m_types_pattern["uintn"] = native_type::UINT;
				m_types_pattern["ulongn"] = native_type::ULONG;
				m_types_pattern["floatn"] = native_type::FLOAT;
				m_types_pattern["doublen"] = native_type::DOUBLE;

				m_addr_space["__global"] = type::GLOBAL;
				m_addr_space["__local"] = type::LOCAL;
				m_addr_space["__constant"] = type::CONSTANT;
				m_addr_space["__private"] = type::PRIVATE;
				m_addr_space["global"] = type::GLOBAL;
				m_addr_space["local"] = type::LOCAL;
				m_addr_space["constant"] = type::CONSTANT;
				m_addr_space["private"] = type::PRIVATE;
			}
			const native_type _gentype((native_type::type_id)gentype, false, native_type::PRIVATE);
			std::string uname = _gentype.get_name();
			std::string iname = _gentype.get_name();
			if (_gentype.is_signed() && m_types.count('u' + uname))	uname = 'u' + uname;
			if (!_gentype.is_signed() && m_types.count(uname.substr(1)))	uname = uname.substr(1);

			const int n = native_type::get_dim_for(gentype);
			bool b_pointer = false;
			bool b_const = false;
			type::address_space addr_space = type::PRIVATE;
			int type_id = 0;
			while(pos < str.size() && str[pos] != ')' && str[pos] != '(' && str[pos] != ',')
			{
				while (pos < str.size() && str[pos] == ' ')
					++pos;
				if (str[pos] == '*')
				{
					++pos;
					b_pointer = true;
					break;
				}
				string word = read_word(str, alphanum_, pos);

				FreeOCL::map<string, int>::const_iterator it = m_types.find(word);
				if (it != m_types.end())
					type_id = it->second;
				else
				{
					it = m_types_pattern.find(word);
					if (it != m_types_pattern.end())
					{
						if (n <= 1)
						{
							word.erase(word.size() - 1);
							type_id = m_types[word];
						}
						else if (n < 10)
						{
							*word.rbegin() = '0' + n;
							type_id = m_types[word];
						}
						else
						{
							*word.rbegin() = '1';
							word += '0' + n - 10;
							type_id = m_types[word];
						}
					}
					else
					{
						it = m_addr_space.find(word);
						if (it != m_addr_space.end())
							addr_space = (type::address_space)it->second;
						else
						{
							if (word == "gentype")
								type_id = gentype;
							else if (word == "ugentype")
								type_id = m_types[uname];
							else if (word == "igentype")
								type_id = m_types[iname];
							else if (word == "sgentype")
								type_id = _gentype.get_scalar_type();
							else if (word == "const")
								b_const = true;
						}
					}
				}
			}
			smartptr<type> p_type = new native_type((native_type::type_id)type_id, b_const, addr_space);
			if (b_pointer)
				p_type = new pointer_type(p_type, false, type::PRIVATE);
			return p_type;
		}
	}


	overloaded_builtin::overloaded_builtin(const string &signature, const deque<int> &gentype)
	{
		deque<int> gentypes = gentype;
		if (gentypes.empty())
			gentypes.push_back(native_type::INT);
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
				possible_types.push_back(deque<smartptr<type> >());
				deque<smartptr<type> > &types = possible_types.back();
				while((*p)[pos] != ')')
				{
					if (pos)	++pos;
					types.push_back(parse_type(*p, pos, *gtype));
				}
			}
		}
		remove_duplicates();

		num_params = possible_types.front().size() - 1;
	}

	overloaded_builtin::~overloaded_builtin()
	{
	}

	smartptr<type> overloaded_builtin::get_return_type(const deque<smartptr<type> > &arg_types) const
	{
		if (arg_types.size() != num_params)
			return (type*)NULL;

		std::stringstream match_info;
		for(deque<smartptr<type> >::const_iterator i = arg_types.begin() ; i != arg_types.end() ; ++i)
			match_info << **i << ',';
		match_info << std::endl;

		smartptr<type> match;
		bool b_multimatch = false;
		for(size_t i = 0 ; i < possible_types.size() ; ++i)
		{
			if (all_types_match(arg_types, possible_types[i]))
			{
				if (match)
					b_multimatch = true;
				match = possible_types[i].front();

				match_info << possible_types[i].front()->get_name() << ' ' << name << '(';
				for(size_t j = 1 ; j < possible_types[i].size() ; ++j)
				{
					if (j > 1)
						match_info << ',';
					match_info << possible_types[i][j]->get_name();
				}
				match_info << ')' << std::endl;
			}
		}
		if (b_multimatch)
			throw std::runtime_error("type matching is ambiguous\n" + match_info.str());
		if (match)
			return match;

		for(size_t i = 0 ; i < possible_types.size() ; ++i)
		{
			if (all_types_weak_match(arg_types, possible_types[i]))
			{
				if (match)
					b_multimatch = true;
				match = possible_types[i].front();

				match_info << possible_types[i].front()->get_name() << ' ' << name << '(';
				for(size_t j = 1 ; j < possible_types[i].size() ; ++j)
				{
					if (j > 1)
						match_info << ',';
					match_info << possible_types[i][j]->get_name();
				}
				match_info << ')' << std::endl;
			}
		}
		if (b_multimatch)
			throw std::runtime_error("type matching is ambiguous\n" + match_info.str());

		return match;
	}

	const string &overloaded_builtin::get_name() const
	{
		return name;
	}

	size_t overloaded_builtin::get_num_params() const
	{
		return num_params;
	}

	void overloaded_builtin::write(ostream& out) const
	{
		out << name << ' ';
	}

	bool overloaded_builtin::all_types_match(const std::deque<smartptr<type> > &args, const std::deque<smartptr<type> > &sig)
	{
		if (args.size() + 1 != sig.size())
			return false;
		for(size_t i = 0 ; i < args.size() ; ++i)
		{
			if (!match(args[i], sig[i + 1]))
				return false;
		}
		return true;
	}

	bool overloaded_builtin::all_types_weak_match(const std::deque<smartptr<type> > &args, const std::deque<smartptr<type> > &sig)
	{
		if (args.size() + 1 != sig.size())
			return false;
		for(size_t i = 0 ; i < args.size() ; ++i)
		{
			if (!weak_match(args[i], sig[i + 1]))
				return false;
		}
		return true;
	}

	bool overloaded_builtin::weak_match(const smartptr<type> &a, const smartptr<type> &b)
	{
		if (*a == *b)
			return true;

		const pointer_type *ptrA = a.as<pointer_type>();
		const pointer_type *ptrB = b.as<pointer_type>();
		if (ptrA && ptrB)
			return ptrA->is_compatible_with(*ptrB) && *ptrA->get_base_type() == *ptrB->get_base_type();

		const native_type *natA = a.as<native_type>();
		const native_type *natB = b.as<native_type>();
		if (natA && natB)
		{
			return natA->get_type_id() == natB->get_type_id()
					|| (natA->is_scalar() && natB->is_scalar());
		}
		return false;
	}

	bool overloaded_builtin::match(const smartptr<type> &a, const smartptr<type> &b)
	{
		if (*a == *b)
			return true;

		const pointer_type *ptrA = a.as<pointer_type>();
		const pointer_type *ptrB = b.as<pointer_type>();
		if (ptrA && ptrB)
			return ptrA->is_compatible_with(*ptrB) && *ptrA->get_base_type() == *ptrB->get_base_type();

		const native_type *natA = a.as<native_type>();
		const native_type *natB = b.as<native_type>();
		if (natA && natB)
			return natA->get_type_id() == natB->get_type_id();
		return false;
	}

	void overloaded_builtin::print_debug_info() const
	{
		std::cout << possible_types.size() << " signatures:" << std::endl;
		for(size_t i = 0 ; i < possible_types.size() ; ++i)
		{
			std::cout << possible_types[i].front()->get_name() << ' ' << name << '(';
			for(size_t j = 1 ; j < possible_types[i].size() ; ++j)
			{
				if (j > 1)
					std::cout << ',';
				std::cout << possible_types[i][j]->get_name();
			}
			std::cout << ')' << std::endl;
		}
	}

	void overloaded_builtin::merge(const smartptr<overloaded_builtin> rhs)
	{
		possible_types.insert(possible_types.end(), rhs->possible_types.begin(), rhs->possible_types.end());
		remove_duplicates();
	}

	void overloaded_builtin::remove_duplicates()
	{
		FreeOCL::set<std::string> types;

		size_t e = 0;
		std::string name;
		for(size_t i = 0 ; i + e < possible_types.size() ; )
		{
			const std::deque<smartptr<type> > &cur = possible_types[i + e];
			name.clear();
			for(size_t k = 0 ; k < cur.size() ; ++k)
				name += cur[k]->get_name();
			if (types.count(name))
			{
				++e;
				continue;
			}
			types.insert(name);
			if (e > 0)
				possible_types[i].swap(possible_types[i + e]);
			++i;
		}
		possible_types.resize(possible_types.size() - e);
	}
}
