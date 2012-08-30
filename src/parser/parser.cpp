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
#include "parser.h"
#include <iostream>
#include <sstream>
#include "macros.h"
#include "chunk.h"
#include "value.h"
#include "token.h"
#include "kernel.h"
#include "native_type.h"
#include "pointer_type.h"
#include "array_type.h"
#include "binary.h"
#include "unary.h"
#include "symbol_table.h"
#include "var.h"
#include "builtin.h"
#include "index.h"
#include "call.h"
#include "callable.h"
#include "member.h"
#include "struct_type.h"
#include "union_type.h"
#include "typedef.h"
#include "swizzle.h"
#include "cast.h"
#include "utils/smartptr.h"
#include "overloaded_builtin.h"
#include "ternary.h"
#include "declarator.h"
#include "qualifier.h"
#include "sizeof.h"
#include "enum_type.h"
#include "struct_literal.h"
#include <utils/string.h>
#include "if.h"
#include "do.h"
#include "while.h"
#include "overloaded_function.h"

namespace FreeOCL
{
	inline int parser::read_token()
	{
		if (tokens.empty())
		{
			const int token = lex();
			processed.push_back(std::make_pair(token, d_val__));
			return token;
		}
		processed.push_back(tokens.back());
		tokens.pop_back();
		d_val__ = processed.back().second;
		return processed.back().first;
	}

	inline int parser::peek_token()
	{
		if (tokens.empty())
		{
			const int token = lex();
			tokens.push_back(std::make_pair(token, d_val__));
		}
		return tokens.back().first;
	}

	inline void parser::roll_back()
	{
		tokens.push_back(processed.back());
		processed.pop_back();
	}

	inline void parser::roll_back_to(size_t size)
	{
		while(processed.size() > size)
			roll_back();
	}

	int parser::parse()
	{
		processed.clear();
		tokens.clear();
		b_errors = false;
		symbols = new symbol_table;
		register_builtin();
		try
		{
			const int ret = __translation_unit();
			processed.clear();
			tokens.clear();
			delete symbols;
			return ret;
		}
		catch(const std::exception &e)
		{
			std::cerr << "[FreeOCL::parser] exception caught : " << e.what() << std::endl;
			processed.clear();
			tokens.clear();
			delete symbols;
			return 0;
		}
		catch(...)
		{
			processed.clear();
			tokens.clear();
			delete symbols;
			return 0;
		}
	}

	int parser::__translation_unit()
	{
		if (__external_declaration())
		{
			chunk *p_chunk = new chunk(d_val__);
			root = p_chunk;
			while(__external_declaration())
				p_chunk->push_back(d_val__);
		}
		else
			root = NULL;
		if (peek_token() != _EOF)
		{
			std::stringstream buf;
			read_token();
			buf << "unexpected '" << *d_val__ << "'";
			ERROR(buf.str());
		}
		return 1;
	}

	int parser::__external_declaration()
	{
		BEGIN(1);
		RULE1(function_definition);
		RULE1(declaration);
		END();
	}

	int parser::__function_definition()
	{
		BEGIN(2);
		const bool b_qualifier = __function_qualifier();
		smartptr<qualifier> qualifiers = d_val__.as<qualifier>();
		const bool b_attribute_qualifier = __attribute_qualifier();
		MATCH2(declaration_specifiers, declarator)
		{
			smartptr<type> p_type = N[0];
			smartptr<chunk> p_chunk = N[1];
			smartptr<pointer_type> ptr = p_chunk->front().as<pointer_type>();
			if (ptr)
			{
				ptr->set_root_type(p_type);
				p_type = ptr;
				p_chunk = p_chunk->back();
			}
			const std::string function_name = p_chunk->front().as<token>()->get_string();
			smartptr<callable> f_from_symbols = symbols->get<callable>(function_name);
			if (f_from_symbols && f_from_symbols->is_defined())
				ERROR(function_name + " already defined");

			// Push a new scope
			symbols->push();
			smartptr<chunk> args = p_chunk->back();
			if (!args)
				END();
            std::deque<smartptr<type> > arg_types;
			if (args->size() == 3)
			{
				// Register all function parameters
				args = (*args)[1];
				for(size_t i = 0 ; i < args->size() ; ++i)
				{
					const smartptr<chunk> cur = (*args)[i].as<chunk>();
					smartptr<type> p_type = cur->front().as<type>();
					std::string name;
					if (cur->back().as<chunk>())
					{
						smartptr<node> front = cur->back().as<chunk>()->front();
						if (front.as<token>())
							name = cur->back().as<chunk>()->front().as<token>()->get_string();
						else if (front.as<chunk>())
						{
							smartptr<chunk> ch = front.as<chunk>();
							if (ch->at(1).as<token>())
								name = ch->at(1).as<token>()->get_string();
							else if (ch->at(1).as<chunk>())
							{
								ch = ch->at(1).as<chunk>();
								smartptr<pointer_type> ptr = ch->front().as<pointer_type>()->clone();
								ptr->set_root_type(p_type);
								p_type = ptr;
								name = ch->at(1).as<chunk>()->front().as<token>()->get_string();
							}
						}
						smartptr<chunk> back = cur->back().as<chunk>()->back().as<chunk>();
						if (back)
						{
							const type::address_space addr_space = p_type->get_address_space();
							if (back->size() > 0 && !back->front().as<chunk>())
								back = new chunk(back);
							for(size_t i = 0 ; i < back->size() ; ++i)
							{
								const smartptr<chunk> ch = (*back)[i].as<chunk>();
								if (!ch)
									continue;
								if (ch->front().as<token>() && ch->front().as<token>()->get_id() == '[')
									p_type = new pointer_type(p_type->clone(p_type->is_const(), addr_space), false, type::PRIVATE);
							}
						}
					}
					else if (cur->back().as<token>())
						name = cur->back().as<token>()->get_string();
                    arg_types.push_back(p_type);
					symbols->insert(name, new var(name, p_type));
				}
			}

			std::string current_line_bak = current_line;
			const size_t line_bak = line;

			__declaration_list();		// Ignore it for now
			if (__compound_statement())
			{
				symbols->pop();
				smartptr<node> statement = d_val__;
				if (b_qualifier && qualifiers->is_set<qualifier::KERNEL>())
				{
					if (*p_type != native_type(native_type::VOID, false, type::PRIVATE))
						error("return type for kernels must be void");
                    d_val__ = kernels[function_name] = new kernel(p_type, function_name, p_chunk->back(), statement, arg_types);
				}
				else
                    d_val__ = new function(p_type, function_name, p_chunk->back(), statement, arg_types);
				if (f_from_symbols && *f_from_symbols.as<function>() != *d_val__.as<function>())
				{
					current_line.swap(current_line_bak);
					line = line_bak;
					ERROR("error: conflicting function declarations!");
				}
				symbols->insert(function_name, d_val__);
				if (b_qualifier
						&& !qualifiers->is_set<qualifier::KERNEL>())
					d_val__ = new chunk(qualifiers, d_val__);
				return 1;
			}
			symbols->pop();

			// Function declaration
			if (__token<';'>())
			{
				if (b_qualifier && qualifiers->is_set<qualifier::KERNEL>())
				{
					if (*p_type != native_type(native_type::VOID, false, type::PRIVATE))
						error("return type for kernels must be void");
					d_val__ = new kernel(p_type, function_name, p_chunk->back(), smartptr<chunk>(), arg_types);
					if (kernels.count(function_name) == 0)
						kernels[function_name] = d_val__.as<kernel>();
				}
				else
					d_val__ = new function(p_type, function_name, p_chunk->back(), smartptr<chunk>(), arg_types);
				if (!f_from_symbols)
					symbols->insert(function_name, d_val__);
				else if (*f_from_symbols.as<function>() != *d_val__.as<function>())
					ERROR("error: conflicting function declarations!");
				if (b_qualifier
						&& !qualifiers->is_set<qualifier::KERNEL>())
					d_val__ = new chunk(qualifiers, d_val__);
				return 1;
			}
			ERROR("syntax error: compound statement expected!");
		}
		if (b_qualifier)
			ERROR("syntax error: function definition expected!");

//		RULE3(declarator, declaration_list, compound_statement);
//		RULE2(declarator, compound_statement);
		END();
	}

	int parser::__declaration()
	{
		BEGIN(2);

		if (__declaration_specifiers())
		{
			smartptr<type> p_type = d_val__;
			if(__token<';'>())
			{
				if (p_type.as<enum_type>())
				{
					d_val__ = new chunk(p_type, d_val__);
					return 1;
				}
				warning("declaration doesn't declare anything!");
				d_val__ = new token("", 0);
				return 1;
			}

			d_val__ = p_type;
			MATCH2(init_declarator_list, token<';'>)
			{
				d_val__ = new declarator(p_type, N[0], N[1]);
				return 1;
			}
			ERROR("syntax error, ';' expected");
		}

		END();
	}

	int parser::__declaration_specifier()
	{
		BEGIN(1);
		RULE1(storage_class_specifier);
		RULE1(type_specifier);
		RULE1(type_qualifier);
		END();
	}

	int parser::__declaration_specifiers()
	{
		if (__declaration_specifier())
		{
			smartptr<type> p_type;
			type::address_space addr_space = type::PRIVATE;
			bool b_address_space_set = false;
			bool b_const = false;
			bool b_typedef = false;
			do
			{
				if (d_val__.as<type>())
					p_type = d_val__;
				else if (d_val__.as<token>())
				{
					token *p_token = d_val__.as<token>();
					switch(p_token->get_id())
					{
					case CONST:
						if (b_const)
							ERROR("duplicate const");
						b_const = true;
						break;
					case __GLOBAL:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::GLOBAL;
						break;
					case __LOCAL:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::LOCAL;
						break;
					case __PRIVATE:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::PRIVATE;
						break;
					case __CONSTANT:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::CONSTANT;
						break;
					case TYPEDEF:
						if (b_typedef)
							ERROR("typedef duplicated");
						b_typedef = true;
						break;
					}
				}
			}
			while (__declaration_specifier());
			if (!p_type)
				ERROR("syntax error: missing type");
			if (b_const && addr_space == type::CONSTANT)
				warning("const used with __constant address space");
			p_type = p_type->clone(b_const, addr_space);
			if (b_typedef)
				p_type = new type_decl(p_type);
			d_val__ = p_type;
			return 1;
		}
		return 0;
	}

	int parser::__init_declarator_list()
	{
		smartptr<type> p_type = d_val__;
		const bool decl = p_type.as<type_decl>();
		if (p_type.as<type_def>())
			p_type = p_type.as<type_def>()->get_type();

		smartptr<chunk> N = new chunk;
		size_t l = processed.size();
		while(true)
		{
			if (!__init_declarator())
			{
				roll_back_to(l);
				if (N->size() == 0)
					return 0;
				ERROR("syntax error, init_declarator expected");
			}
			N->push_back(d_val__);
			// register variables
			{
				smartptr<chunk> p_declarator = d_val__.as<chunk>()->front().as<chunk>();
				smartptr<type> l_type;
				std::string name;
				bool b_use_local_hack = false;
				if (p_declarator->front().as<pointer_type>())
				{
					smartptr<pointer_type> ptr = p_declarator->front().as<pointer_type>()->clone();
					ptr->set_root_type(p_type);
					l_type = ptr;

					name = p_declarator->back().as<chunk>()->front().as<token>()->get_string();

					smartptr<chunk> suffix_list = p_declarator->back().as<chunk>();
					for(size_t j = 1 ; j < suffix_list->size() ; ++j)
					{
						smartptr<chunk> p_chunk = (*suffix_list)[j].as<chunk>();
						if (!p_chunk)
							continue;
						if (p_chunk->front().as<token>()->get_id() == '[')
							l_type = new array_type(l_type, l_type->is_const(), l_type->get_address_space(), p_chunk->at(1).as<expression>()->eval_as_uint());
					}
				}
				else
				{
					l_type = p_type;
					b_use_local_hack = (l_type->get_address_space() == type::LOCAL);
					name = p_declarator->front().as<token>()->get_string();
					if (b_use_local_hack)
						p_declarator->front() = new chunk(new token("(&",parser::SPECIAL),
														  p_declarator->front(),
														  new token(")",')'));

					for(size_t j = 1 ; j < p_declarator->size() ; ++j)
					{
						smartptr<chunk> p_chunk = (*p_declarator)[j].as<chunk>();
						if (!p_chunk)
							continue;
						if (p_chunk->front().as<token>()->get_id() == '[')
						{
							smartptr<expression> exp = p_chunk->at(1).as<expression>();
							if (exp)
							{
								try
								{
									l_type = new array_type(l_type, l_type->is_const(), l_type->get_address_space(), exp->eval_as_uint());
								}
								catch(const char *msg)
								{
									ERROR(msg);
								}
							}
						}
					}
				}
				if (decl)
					symbols->insert(name, new type_def(name, l_type));
				else
					symbols->insert(name, new var(name, l_type));

				if (d_val__.as<chunk>()->size() == 3)		// Check initializer
				{
					// If it's not an expression, then it's a vector/struct initializer which will be handleded by the C++ compiler
					smartptr<expression> init = d_val__.as<chunk>()->back().as<expression>();
					if (init && init->get_type().as<native_type>() && l_type.as<native_type>())
					{
						smartptr<native_type> init_type = init->get_type().as<native_type>();
						smartptr<native_type> v_type = l_type.as<native_type>();
						const std::string v_type_name = native_type(v_type->get_type_id(), false, native_type::PRIVATE).get_cxx_name();
						if (v_type->is_vector() && init_type->is_scalar())
							d_val__.as<chunk>()->back() = new chunk(new token(v_type_name + "::make(", parser::SPECIAL), init, new token(")", ')'));
						else if (v_type->is_vector() && init_type->is_vector())
						{
							if (v_type->get_dim() != init_type->get_dim())
								ERROR("vector dimensions must match!");
							if (v_type->get_scalar_type() != init_type->get_scalar_type())
								d_val__.as<chunk>()->back() = new call(symbols->get<callable>("convert_" + v_type_name), new chunk(init));
						}
					}
				}
				// Special hack for local values (use references to template generated static variables)
				if (b_use_local_hack)
				{
					smartptr<chunk> ch = d_val__.as<chunk>();
					const std::string regular_type_name = l_type.as<array_type>() ? l_type.as<array_type>()->complete_name() : l_type->get_cxx_name();
					const size_t ID = (line << 10) ^ current_line.size();
					if (d_val__.as<chunk>()->size() == 1)
					{
						ch->push_back(new token("=", '='));
						ch->push_back(new token("__create_local<" + regular_type_name + ", " + FreeOCL::to_string(ID) + ">()", parser::SPECIAL));
					}
					else
					{
						smartptr<node> init = ch->at(2);
						ch->at(2) = new chunk(new token("__create_local<" + regular_type_name + ", " + FreeOCL::to_string(ID) + ">(", parser::SPECIAL),
											  init,
											  new token(")", ')'));
					}
				}
			}
			l = processed.size();
			if (!__token<','>())
			{
				d_val__ = N;
				return 1;
			}
			N->push_back(d_val__);
		}
//		if (__init_declarator())
//		{
//			smartptr<chunk> N = new chunk(d_val__);
//			size_t l = processed.size();
//			while (__token<','>())
//			{
//				N->push_back(d_val__);
//				if (!__init_declarator())
//				{
//					roll_back_to(l);
//					break;
//				}
//				N->push_back(d_val__);
//				l = processed.size();
//			}
//			d_val__ = N;
//			return 1;
//		}
		return 0;
	}

	int parser::__declarator()
	{
		BEGIN(2);
		RULE2(pointer, direct_declarator);
		RULE1(direct_declarator);
		END();
	}

	int parser::__declaration_list()
	{
		LISTOF_LEFT(declaration);
		return 0;
	}

	int parser::__pointer()
	{
		BEGIN_SMALL();
		if (peek_token() == '*')
		{
			smartptr<type> ptr;
			while(__token<'*'>())
			{
				bool b_const = false;
				bool b_address_space_set = false;
				type::address_space addr_space = type::PRIVATE;
				if (__type_qualifier_list())
				{
					chunk *p_chunk = d_val__.as<chunk>();
					for(size_t i = 0 ; i < p_chunk->size() ; ++i)
					{
						switch((*p_chunk)[i].as<token>()->get_id())
						{
						case CONST:
							if (b_const)
								ERROR("duplicate const");
							b_const = true;
							break;
						case __GLOBAL:
							if (b_address_space_set)
								ERROR("2 address space qualifiers");
							b_address_space_set = true;
							addr_space = type::GLOBAL;
							break;
						case __LOCAL:
							if (b_address_space_set)
								ERROR("2 address space qualifiers");
							b_address_space_set = true;
							addr_space = type::LOCAL;
							break;
						case __CONSTANT:
							if (b_address_space_set)
								ERROR("2 address space qualifiers");
							b_address_space_set = true;
							addr_space = type::CONSTANT;
							break;
						case __PRIVATE:
							if (b_address_space_set)
								ERROR("2 address space qualifiers");
							b_address_space_set = true;
							addr_space = type::PRIVATE;
							break;
						}
					}
				}
				ptr = new pointer_type(ptr, b_const, addr_space);
			}
			d_val__ = ptr;
			return 1;
		}
		END();
	}

	int parser::__function_qualifier()
	{
		smartptr<qualifier> q = new qualifier;
		while(true)
			switch (peek_token())
			{
			case __KERNEL:
				if (q->is_set<qualifier::KERNEL>())
					ERROR("__kernel keyword duplicated");
				q->set<qualifier::KERNEL>();
				read_token();
				break;
			case INLINE:
				if (q->is_set<qualifier::INLINE>())
					ERROR("inline keyword duplicated");
				q->set<qualifier::INLINE>();
				read_token();
				break;
			default:
				d_val__ = q;
				return q->get_flags() != 0;
			}
	}

	int parser::__storage_class_specifier()
	{
		return __token<TYPEDEF>() || __token<STATIC>() || __token<EXTERN>();
	}

	int parser::__type_specifier()
	{
		BEGIN(1);
		switch(read_token())
		{
		case BOOL:		d_val__ = new native_type(native_type::BOOL, false, type::PRIVATE);		return 1;
		case HALF:		d_val__ = new native_type(native_type::HALF, false, type::PRIVATE);		return 1;
		case VOID:		d_val__ = new native_type(native_type::VOID, false, type::PRIVATE);		return 1;
		case CHAR:		d_val__ = new native_type(native_type::CHAR, false, type::PRIVATE);		return 1;
		case SHORT:		d_val__ = new native_type(native_type::SHORT, false, type::PRIVATE);	return 1;
		case INT:		d_val__ = new native_type(native_type::INT, false, type::PRIVATE);		return 1;
		case LONG:		d_val__ = new native_type(native_type::LONG, false, type::PRIVATE);		return 1;
		case UCHAR:		d_val__ = new native_type(native_type::UCHAR, false, type::PRIVATE);	return 1;
		case USHORT:	d_val__ = new native_type(native_type::USHORT, false, type::PRIVATE);	return 1;
		case UINT:		d_val__ = new native_type(native_type::UINT, false, type::PRIVATE);		return 1;
		case ULONG:		d_val__ = new native_type(native_type::ULONG, false, type::PRIVATE);	return 1;
		case FLOAT:		d_val__ = new native_type(native_type::FLOAT, false, type::PRIVATE);	return 1;
		case DOUBLE:	d_val__ = new native_type(native_type::DOUBLE, false, type::PRIVATE);	return 1;
		case CHAR2:		d_val__ = new native_type(native_type::CHAR2, false, type::PRIVATE);	return 1;
		case SHORT2:	d_val__ = new native_type(native_type::SHORT2, false, type::PRIVATE);	return 1;
		case INT2:		d_val__ = new native_type(native_type::INT2, false, type::PRIVATE);		return 1;
		case LONG2:		d_val__ = new native_type(native_type::LONG2, false, type::PRIVATE);	return 1;
		case UCHAR2:	d_val__ = new native_type(native_type::UCHAR2, false, type::PRIVATE);	return 1;
		case USHORT2:	d_val__ = new native_type(native_type::USHORT2, false, type::PRIVATE);	return 1;
		case UINT2:		d_val__ = new native_type(native_type::UINT2, false, type::PRIVATE);	return 1;
		case ULONG2:	d_val__ = new native_type(native_type::ULONG2, false, type::PRIVATE);	return 1;
		case FLOAT2:	d_val__ = new native_type(native_type::FLOAT2, false, type::PRIVATE);	return 1;
		case DOUBLE2:	d_val__ = new native_type(native_type::DOUBLE2, false, type::PRIVATE);	return 1;
		case CHAR3:		d_val__ = new native_type(native_type::CHAR3, false, type::PRIVATE);	return 1;
		case SHORT3:	d_val__ = new native_type(native_type::SHORT3, false, type::PRIVATE);	return 1;
		case INT3:		d_val__ = new native_type(native_type::INT3, false, type::PRIVATE);		return 1;
		case LONG3:		d_val__ = new native_type(native_type::LONG3, false, type::PRIVATE);	return 1;
		case UCHAR3:	d_val__ = new native_type(native_type::UCHAR3, false, type::PRIVATE);	return 1;
		case USHORT3:	d_val__ = new native_type(native_type::USHORT3, false, type::PRIVATE);	return 1;
		case UINT3:		d_val__ = new native_type(native_type::UINT3, false, type::PRIVATE);	return 1;
		case ULONG3:	d_val__ = new native_type(native_type::ULONG3, false, type::PRIVATE);	return 1;
		case FLOAT3:	d_val__ = new native_type(native_type::FLOAT3, false, type::PRIVATE);	return 1;
		case DOUBLE3:	d_val__ = new native_type(native_type::DOUBLE3, false, type::PRIVATE);	return 1;
		case CHAR4:		d_val__ = new native_type(native_type::CHAR4, false, type::PRIVATE);	return 1;
		case SHORT4:	d_val__ = new native_type(native_type::SHORT4, false, type::PRIVATE);	return 1;
		case INT4:		d_val__ = new native_type(native_type::INT4, false, type::PRIVATE);		return 1;
		case LONG4:		d_val__ = new native_type(native_type::LONG4, false, type::PRIVATE);	return 1;
		case UCHAR4:	d_val__ = new native_type(native_type::UCHAR4, false, type::PRIVATE);	return 1;
		case USHORT4:	d_val__ = new native_type(native_type::USHORT4, false, type::PRIVATE);	return 1;
		case UINT4:		d_val__ = new native_type(native_type::UINT4, false, type::PRIVATE);	return 1;
		case ULONG4:	d_val__ = new native_type(native_type::ULONG4, false, type::PRIVATE);	return 1;
		case FLOAT4:	d_val__ = new native_type(native_type::FLOAT4, false, type::PRIVATE);	return 1;
		case DOUBLE4:	d_val__ = new native_type(native_type::DOUBLE4, false, type::PRIVATE);	return 1;
		case CHAR8:		d_val__ = new native_type(native_type::CHAR8, false, type::PRIVATE);	return 1;
		case SHORT8:	d_val__ = new native_type(native_type::SHORT8, false, type::PRIVATE);	return 1;
		case INT8:		d_val__ = new native_type(native_type::INT8, false, type::PRIVATE);		return 1;
		case LONG8:		d_val__ = new native_type(native_type::LONG8, false, type::PRIVATE);	return 1;
		case UCHAR8:	d_val__ = new native_type(native_type::UCHAR8, false, type::PRIVATE);	return 1;
		case USHORT8:	d_val__ = new native_type(native_type::USHORT8, false, type::PRIVATE);	return 1;
		case UINT8:		d_val__ = new native_type(native_type::UINT8, false, type::PRIVATE);	return 1;
		case ULONG8:	d_val__ = new native_type(native_type::ULONG8, false, type::PRIVATE);	return 1;
		case FLOAT8:	d_val__ = new native_type(native_type::FLOAT8, false, type::PRIVATE);	return 1;
		case DOUBLE8:	d_val__ = new native_type(native_type::DOUBLE8, false, type::PRIVATE);	return 1;
		case CHAR16:	d_val__ = new native_type(native_type::CHAR16, false, type::PRIVATE);	return 1;
		case SHORT16:	d_val__ = new native_type(native_type::SHORT16, false, type::PRIVATE);	return 1;
		case INT16:		d_val__ = new native_type(native_type::INT16, false, type::PRIVATE);	return 1;
		case LONG16:	d_val__ = new native_type(native_type::LONG16, false, type::PRIVATE);	return 1;
		case UCHAR16:	d_val__ = new native_type(native_type::UCHAR16, false, type::PRIVATE);	return 1;
		case USHORT16:	d_val__ = new native_type(native_type::USHORT16, false, type::PRIVATE);	return 1;
		case UINT16:	d_val__ = new native_type(native_type::UINT16, false, type::PRIVATE);	return 1;
		case ULONG16:	d_val__ = new native_type(native_type::ULONG16, false, type::PRIVATE);	return 1;
		case FLOAT16:	d_val__ = new native_type(native_type::FLOAT16, false, type::PRIVATE);	return 1;
		case DOUBLE16:	d_val__ = new native_type(native_type::DOUBLE16, false, type::PRIVATE);	return 1;
		case SIZE_T:	d_val__ = new native_type(native_type::SIZE_T, false, type::PRIVATE);	return 1;
		case SAMPLER_T:	d_val__ = new native_type(native_type::SAMPLER_T, false, type::PRIVATE);	return 1;
		case IMAGE1D_T:	d_val__ = new native_type(native_type::IMAGE1D_T, false, type::PRIVATE);	return 1;
		case IMAGE1D_BUFFER_T:	d_val__ = new native_type(native_type::IMAGE1D_BUFFER_T, false, type::PRIVATE);	return 1;
		case IMAGE1D_ARRAY_T:	d_val__ = new native_type(native_type::IMAGE1D_ARRAY_T, false, type::PRIVATE);	return 1;
		case IMAGE2D_T:	d_val__ = new native_type(native_type::IMAGE2D_T, false, type::PRIVATE);	return 1;
		case IMAGE2D_ARRAY_T:	d_val__ = new native_type(native_type::IMAGE2D_ARRAY_T, false, type::PRIVATE);	return 1;
		case IMAGE3D_T:	d_val__ = new native_type(native_type::IMAGE3D_T, false, type::PRIVATE);	return 1;
		case EVENT_T:	d_val__ = new native_type(native_type::EVENT_T, false, type::PRIVATE);	return 1;

		case SIGNED:
			switch (peek_token())
			{
			case CHAR:
				read_token();
				d_val__ = new native_type(native_type::CHAR, false, type::PRIVATE);
				return 1;
			case SHORT:
				read_token();
				d_val__ = new native_type(native_type::SHORT, false, type::PRIVATE);
				return 1;
			case INT:
				read_token();
				d_val__ = new native_type(native_type::INT, false, type::PRIVATE);
				return 1;
			case LONG:
				read_token();
				d_val__ = new native_type(native_type::LONG, false, type::PRIVATE);
				return 1;
			default:
				d_val__ = new native_type(native_type::INT, false, type::PRIVATE);
				return 1;
			}
			break;

		case UNSIGNED:
			switch (peek_token())
			{
			case CHAR:
				read_token();
				d_val__ = new native_type(native_type::UCHAR, false, type::PRIVATE);
				return 1;
			case SHORT:
				read_token();
				d_val__ = new native_type(native_type::USHORT, false, type::PRIVATE);
				return 1;
			case INT:
				read_token();
				d_val__ = new native_type(native_type::UINT, false, type::PRIVATE);
				return 1;
			case LONG:
				read_token();
				d_val__ = new native_type(native_type::ULONG, false, type::PRIVATE);
				return 1;
			default:
				d_val__ = new native_type(native_type::UINT, false, type::PRIVATE);
				return 1;
			}
			break;

		case TYPE_NAME:	return 1;
		default:
			roll_back();
			RULE1(struct_or_union_specifier);
			RULE1(enum_specifier);
		}
		END();
	}

	int parser::__type_qualifier()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case CONST:			RULE1(token<CONST>);		break;
		case VOLATILE:		RULE1(token<VOLATILE>);		break;
		case RESTRICT:		RULE1(token<RESTRICT>);		break;
		case __GLOBAL:		RULE1(token<__GLOBAL>);		break;
		case __LOCAL:		RULE1(token<__LOCAL>);		break;
		case __PRIVATE:		RULE1(token<__PRIVATE>);	break;
		case __CONSTANT:	RULE1(token<__CONSTANT>);	break;
		case __READ_ONLY:	RULE1(token<__READ_ONLY>);	break;
		case __WRITE_ONLY:	RULE1(token<__WRITE_ONLY>);	break;
		case __READ_WRITE:	RULE1(token<__READ_WRITE>);	break;
		}
		END();
	}

	int parser::__type_qualifier_list()
	{
		BEGIN_SMALL();
		if (__type_qualifier())
		{
			smartptr<chunk> N = new chunk(d_val__);
			while (__type_qualifier())
				N->push_back(d_val__);
			d_val__ = N;
			return 1;
		}
		END();
	}

	int parser::__enum_specifier()
	{
		if (peek_token() != ENUM)
			return 0;
		BEGIN(5);
		MATCH4(token<ENUM>, token<'{'>, enumerator_list, token<'}'>)
		{
			std::vector<smartptr<node> > values;
			const chunk *p_enum_list = N[2].as<chunk>();
			values.reserve((p_enum_list->size() + 1) / 2);
			for(size_t i = 0 ; i < p_enum_list->size() ; i += 2)
			{
				values.push_back(p_enum_list->at(i));
				const chunk *p_chunk = values.back().as<chunk>();
				const std::string &enum_name = p_chunk
											   ? p_chunk->front().as<token>()->get_string()
											   : values.back().as<token>()->get_string();
				symbols->insert(enum_name, new var(enum_name, native_type::t_int));
			}
			d_val__ = new enum_type(std::string(), values, false, type::PRIVATE);
			return 1;
		}
		MATCH5(token<ENUM>, token<IDENTIFIER>, token<'{'>, enumerator_list, token<'}'>)
		{
			std::vector<smartptr<node> > values;
			const chunk *p_enum_list = N[3].as<chunk>();
			values.reserve((p_enum_list->size() + 1) / 2);
			for(size_t i = 0 ; i < p_enum_list->size() ; i += 2)
			{
				values.push_back(p_enum_list->at(i));
				const chunk *p_chunk = values.back().as<chunk>();
				const std::string &enum_name = p_chunk
											   ? p_chunk->front().as<token>()->get_string()
											   : values.back().as<token>()->get_string();
				warning(FreeOCL::to_string(i) + ":" + enum_name);
				symbols->insert(enum_name, new var(enum_name, native_type::t_int));
			}
			d_val__ = new enum_type(N[1].as<token>()->get_string(), values, false, type::PRIVATE);
			return 1;
		}
		MATCH2(token<ENUM>, token<IDENTIFIER>)
		{
			d_val__ = new enum_type(N[1].as<token>()->get_string(), std::vector<smartptr<node> >(), false, type::PRIVATE);
			return 1;
		}

		CHECK(1, "syntax error");
		END();
	}

	int parser::__enumerator_list()
	{
		BEGIN_SMALL();
		if (__enumerator())
		{
			smartptr<chunk> N = new chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				const smartptr<node> N1 = d_val__;
				if (!__enumerator())
				{
					roll_back_to(l);
					break;
				}
				N->push_back(N1);
				N->push_back(d_val__);
				l = processed.size();
			}
			d_val__ = N;
			return 1;
		}
		END();
	}

	int parser::__enumerator()
	{
		BEGIN(3);
		if (peek_token() == IDENTIFIER)
		{
			RULE3(token<IDENTIFIER>, token<'='>, constant_expression);
			RULE1(token<IDENTIFIER>);
		}
		END();
	}

	int parser::__direct_declarator_suffix()
	{
		BEGIN(3);
		switch(peek_token())
		{
		case '[':
			RULE3(token<'['>, constant_expression, token<']'>);
			RULE2(token<'['>, token<']'>);
			break;
		case '(':
			RULE3(token<'('>, parameter_type_list, token<')'>);
			RULE3(token<'('>, identifier_list, token<')'>);
			RULE2(token<'('>, token<')'>);
			break;
		}
		END();
	}

	int parser::__direct_declarator_base()
	{
		BEGIN(3);
		RULE1(token<IDENTIFIER>);
		RULE3(token<'('>, declarator, token<')'>);
		END();
	}

	int parser::__direct_declarator()
	{
		BEGIN_SMALL();
		if (__direct_declarator_base())
		{
			smartptr<chunk> N = new chunk(d_val__);
			while(__direct_declarator_suffix())
				N->push_back(d_val__);
			d_val__ = N;
			return 1;
		}
		END();
	}

	int parser::__identifier_list()
	{
		BEGIN_SMALL();
		LISTOF_LEFT_SEP(token<IDENTIFIER>, token<','>);
		END();
	}

	int parser::__parameter_type_list()
	{
		return __parameter_list();
	}

	int parser::__parameter_list()
	{
		if (__parameter_declaration())
		{
			smartptr<chunk> N = new chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__parameter_declaration())
				{
					roll_back_to(l);
					break;
				}
				N->push_back(d_val__);
				l = processed.size();
			}
			d_val__ = N;
			return 1;
		}
		return 0;
	}

	int parser::__parameter_declaration()
	{
		BEGIN(2);
		MATCH1(declaration_specifiers)
		{
			smartptr<node> N0 = N[0];
			MATCH2(declarator, attribute_qualifier)
			{
				smartptr<chunk> p_chunk = N[0].as<chunk>();
				smartptr<pointer_type> ptr = p_chunk->front().as<pointer_type>();
				if (ptr)
				{
					ptr->set_root_type(N0.as<type>());
					N0 = ptr;
					N[0] = p_chunk->back();
				}
				d_val__ = new chunk(N0, N[0]);
				return 1;
			}
			MATCH2(abstract_declarator, attribute_qualifier)
			{
				d_val__ = new chunk(N0, N[0]);
				return 1;
			}
			MATCH1(attribute_qualifier)
			{
				d_val__ = N0;
				return 1;
			}
		}

		END();
	}

	int parser::__abstract_declarator()
	{
		BEGIN(1);
		MATCH1(pointer)
		{
			const smartptr<node> N0 = N[0];
			MATCH1(direct_abstract_declarator)
			{
				d_val__ = new chunk(N0, d_val__);
				return 1;
			}
			d_val__ = N0;
			return 1;
		}
		RULE1(direct_abstract_declarator);
		END();
	}

	int parser::__direct_abstract_declarator_base()
	{
		BEGIN(3);
		switch(peek_token())
		{
		case '(':
			RULE3(token<'('>, abstract_declarator, token<')'>);
			RULE2(token<'('>, token<')'>);
			RULE3(token<'('>, parameter_type_list, token<')'>);
			break;
		case '[':
			RULE2(token<'['>, token<']'>);
			RULE3(token<'['>, constant_expression, token<']'>);
			break;
		}
		END();
	}

	int parser::__direct_abstract_declarator_suffix()
	{
		BEGIN(3);
		switch(peek_token())
		{
		case '[':
			RULE2(token<'['>, token<']'>);
			RULE3(token<'['>, constant_expression, token<']'>);
			break;
		case '(':
			RULE2(token<'('>, token<')'>);
			RULE3(token<'('>, parameter_type_list, token<')'>);
			break;
		}
		END();
	}

	int parser::__direct_abstract_declarator()
	{
		BEGIN_SMALL();
		if (__direct_abstract_declarator_base())
		{
			smartptr<node> N = d_val__;
			while(__direct_abstract_declarator_suffix())
				N = new chunk(N, d_val__);
			d_val__ = N;
			return 1;
		}
		END();
	}

	int parser::__constant_expression()
	{
		return __conditional_expression();
	}

	int parser::__struct_or_union_specifier()
	{
		BEGIN(5);
		if (__struct_or_union())
		{
			smartptr<token> tok = d_val__.as<token>();
			std::string name;
			if (__token<IDENTIFIER>())
				name = d_val__.as<token>()->get_string();
			if (__token<'{'>())
			{
				if (__struct_declaration_list())
				{
					smartptr<struct_type> p_type = (tok->get_id() == STRUCT) ? new struct_type(name) : new union_type(name);

					smartptr<chunk> members = d_val__.as<chunk>();
					for(size_t i = 0 ; i < members->size() ; ++i)
					{
						smartptr<chunk> member_list = (*members)[i].as<chunk>();
						for(size_t j = 0 ; j < member_list->size() ; ++j)
						{
							smartptr<chunk> member = (*member_list)[j].as<chunk>();
							*p_type << std::make_pair(member->back().as<token>()->get_string(), member->front().as<type>());
						}
					}

					if (!__token<'}'>())
						ERROR("syntax error : '}' expected");

					d_val__ = p_type;
					return 1;
				}
				ERROR("syntax error, structure declaration expected");
			}
			ERROR("syntax error, '{' or identifier expected");
		}
//		RULE5(struct_or_union, token<IDENTIFIER>, token<'{'>, struct_declaration_list, token<'}'>);
//		RULE4(struct_or_union, token<'{'>, struct_declaration_list, token<'}'>);
//		RULE2(struct_or_union, token<IDENTIFIER>);
		CHECK(2, "syntax error");
		CHECK(1, "syntax error, '{' or identifier expected");
		END();
	}

	int parser::__struct_or_union()
	{
		BEGIN(1);
		RULE1(token<STRUCT>);
		RULE1(token<UNION>);
		END();
	}

	int parser::__struct_declaration_list()
	{
		BEGIN_SMALL();
		if (__struct_declaration())
		{
			smartptr<chunk> p_chunk = new chunk;
			p_chunk->push_back(d_val__);
			while (__struct_declaration())
				p_chunk->push_back(d_val__);
			d_val__ = p_chunk;
			return 1;
		}
		END();
	}

	int parser::__struct_declaration()
	{
		BEGIN(3);
		MATCH3(specifier_qualifier_list, struct_declarator_list, token<';'>)
		{
			smartptr<chunk> members = new chunk;
			smartptr<type> p_type = N[0].as<type>();
			const smartptr<chunk> var_list = N[1].as<chunk>();
			for(size_t i = 0 ; i < var_list->size() ; ++i)
			{
				smartptr<chunk> declarator = (*var_list)[i].as<chunk>();
				smartptr<type> l_type;
				smartptr<token> name;
				if (declarator->front().as<pointer_type>())
				{
					smartptr<pointer_type> ptr = declarator->front().as<pointer_type>()->clone();
					ptr->set_root_type(p_type);
					l_type = ptr;

					name = declarator->back().as<chunk>()->front().as<token>();
					declarator = declarator->back();
				}
				else
				{
					l_type = p_type;
					name = declarator->front().as<token>();
				}
				for(size_t i = 1 ; i < declarator->size() ; ++i)
				{
					smartptr<chunk> cur = declarator->at(i).as<chunk>();
					if (cur)
					{
						const type::address_space addr_space = l_type->get_address_space();
						if (cur->size() > 0 && !cur->front().as<chunk>())
							cur = new chunk(cur);
						for(size_t i = 0 ; i < cur->size() ; ++i)
						{
							const smartptr<chunk> ch = (*cur)[i].as<chunk>();
							if (!ch)
								continue;
							if (ch->front().as<token>() && ch->front().as<token>()->get_id() == '[')
								l_type = new array_type(l_type->clone(l_type->is_const(), addr_space), false, type::PRIVATE, ch->at(1).as<expression>()->eval_as_uint());
						}
					}
				}
				members->push_back(new chunk(l_type, name));
			}
			d_val__ = members;
			return 1;
		}
		END();
	}

	int parser::__struct_declarator_list()
	{
		BEGIN_SMALL();
		if (__struct_declarator())
		{
			smartptr<chunk> p_chunk = new chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__struct_declarator())
				{
					roll_back_to(l);
					break;
				}
				p_chunk->push_back(d_val__);
				l = processed.size();
			}
			d_val__ = p_chunk;
			return 1;
		}
		END();
	}

	int parser::__struct_declarator()
	{
		return __declarator();
	}

	int parser::__specifier_qualifier()
	{
		BEGIN(1);
		RULE1(type_specifier);
		RULE1(type_qualifier);
		END();
	}

	int parser::__specifier_qualifier_list()
	{
		if (__specifier_qualifier())
		{
			smartptr<type> p_type;
			type::address_space addr_space = type::PRIVATE;
			bool b_address_space_set = false;
			bool b_const = false;
			do
			{
				if (d_val__.as<type>())
					p_type = d_val__;
				else if (d_val__.as<token>())
				{
					token *p_token = d_val__.as<token>();
					switch(p_token->get_id())
					{
					case CONST:
						if (b_const)
							ERROR("duplicate const");
						b_const = true;
						break;
					case __GLOBAL:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::GLOBAL;
						break;
					case __LOCAL:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::LOCAL;
						break;
					case __PRIVATE:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::PRIVATE;
						break;
					case __CONSTANT:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						addr_space = type::CONSTANT;
						break;
					}
				}
			}
			while (__specifier_qualifier());
			if (!p_type)
				ERROR("syntax error: missing type");
			if (b_const && addr_space == type::CONSTANT)
				warning("const used with __constant address space");
			d_val__ = p_type->clone(b_const, addr_space);
			return 1;
		}
		return 0;
	}

	int parser::__compound_statement()
	{
		BEGIN(3);
		if (peek_token() == '{')
		{
			RULE2(token<'{'>, token<'}'>);
			RULE3(token<'{'>, declaration_statement_list, token<'}'>);
			CHECK(1, "syntax error");
		}
		END();
	}

	int parser::__declaration_statement_list()
	{
		BEGIN_SMALL();
		LISTOF_LEFT(declaration_statement);
		END();
	}

	int parser::__declaration_statement()
	{
		BEGIN(1);
		RULE1(declaration);
		RULE1(statement);
		END();
	}

	int parser::__statement_list()
	{
		BEGIN_SMALL();
		LISTOF_LEFT(statement);
		END();
	}

	int parser::__statement()
	{
		BEGIN(1);
		RULE1(labeled_statement);
		RULE1(compound_statement);
		RULE1(expression_statement);
		RULE1(selection_statement);
		RULE1(iteration_statement);
		RULE1(jump_statement);
		END();
	}

	int parser::__labeled_statement()
	{
		BEGIN(4);
		switch(peek_token())
		{
		case IDENTIFIER:
			RULE3(token<IDENTIFIER>, token<':'>, statement);
			CHECK(2, "syntax error, statement expected");
			break;
		case CASE:
			RULE4(token<CASE>, constant_expression, token<':'>, statement);
			CHECK(3, "syntax error, statement expected");
			CHECK(2, "syntax error, ':' expected");
			CHECK(1, "syntax error, constant expression expected");
			break;
		case DEFAULT:
			RULE3(token<DEFAULT>, token<':'>, statement);
			CHECK(2, "syntax error, statement expected");
			CHECK(1, "syntax error, ':' expected");
			break;
		}
		END();
	}

	int parser::__jump_statement()
	{
		BEGIN(3);
		switch(peek_token())
		{
		case GOTO:
			RULE3(token<GOTO>, token<IDENTIFIER>, token<';'>);
			CHECK(2, "syntax error, ';' expected");
			CHECK(1, "syntax error, identifier expected");
			break;
		case CONTINUE:
			RULE2(token<CONTINUE>, token<';'>);
			CHECK(1, "syntax error, ';' expected");
			break;
		case BREAK:
			RULE2(token<BREAK>, token<';'>);
			CHECK(1, "syntax error, ';' expected");
			break;
		case RETURN:
			RULE2(token<RETURN>, token<';'>);
			RULE3(token<RETURN>, expression, token<';'>);
			CHECK(1, "syntax error");
			break;
		}
		END();
	}

	int parser::__iteration_statement()
	{
		BEGIN(7);
		switch(peek_token())
		{
		case WHILE:
			MATCH5(token<WHILE>, token<'('>, expression, token<')'>, statement)
			{
				d_val__ = new _while(N[2], N[4]);
				return 1;
			}
			CHECK(4, "syntax error, statement expected");
			CHECK(3, "syntax error, ')' expected");
			CHECK(2, "syntax error, expression expected");
			CHECK(1, "syntax error, '(' expected");
			break;
		case DO:
			MATCH7(token<DO>, statement, token<WHILE>, token<'('>, expression, token<')'>, token<';'>)
			{
				d_val__ = new _do(N[4], N[1]);
				return 1;
			}
			CHECK(6, "syntax error, ';' expected");
			CHECK(5, "syntax error, ')' expected");
			CHECK(4, "syntax error, expression expected");
			CHECK(3, "syntax error, '(' expected");
			CHECK(2, "syntax error, \"while\" expected");
			CHECK(1, "syntax error, statement expected");
			break;
		case FOR:
			MATCH4(token<FOR>, token<'('>, expression_statement, expression_statement)
			{
				const smartptr<node> N0 = N[0];
				const smartptr<node> N1 = N[1];
				const smartptr<node> N2 = N[2];
				const smartptr<node> N3 = N[3];
				MATCH2(token<')'>, statement)
				{
					d_val__ = new chunk(N0,N1,N2,N3,N[0],N[1]);
					return 1;
				}
				MATCH3(expression, token<')'>, statement)
				{
					d_val__ = new chunk(N0,N1,N2,N3,N[0],N[1],N[2]);
					return 1;
				}
				ERROR("syntax error");
			}
			MATCH4(token<FOR>, token<'('>, declaration, expression_statement)
			{
				const smartptr<node> N0 = N[0];
				const smartptr<node> N1 = N[1];
				const smartptr<node> N2 = N[2];
				const smartptr<node> N3 = N[3];
				MATCH2(token<')'>, statement)
				{
					d_val__ = new chunk(N0,N1,N2,N3,N[0],N[1]);
					return 1;
				}
				MATCH3(expression, token<')'>, statement)
				{
					d_val__ = new chunk(N0,N1,N2,N3,N[0],N[1],N[2]);
					return 1;
				}
				ERROR("syntax error");
			}
			CHECK(2, "syntax error");
			CHECK(1, "syntax error, '(' expected");
			break;
		}
		END();
	}

	int parser::__selection_statement()
	{
		BEGIN(5);
		switch(peek_token())
		{
		case IF:
			MATCH5(token<IF>, token<'('>, expression, token<')'>, statement)
			{
				const smartptr<node> N2 = N[2];
				const smartptr<node> N4 = N[4];
				__max = 0;
				MATCH2(token<ELSE>, statement)
				{
					d_val__ = new _if(N2, N4, N[1]);
					return 1;
				}
				CHECK(1, "syntax error, statement expected after 'else'");

				d_val__ = new _if(N2, N4, (node*)NULL);
				return 1;
			}
			CHECK(5, "syntax error");
			CHECK(4, "syntax error, statement expected");
			CHECK(3, "syntax error, ')' expected");
			CHECK(2, "syntax error, expression expected");
			CHECK(1, "syntax error, '(' expected");
			break;
		case SWITCH:
			RULE5(token<SWITCH>, token<'('>, expression, token<')'>, statement);
			CHECK(4, "syntax error, statement expected");
			CHECK(3, "syntax error, ')' expected");
			CHECK(2, "syntax error, expression expected");
			CHECK(1, "syntax error, '(' expected");
			break;
		}
		END();
	}

	int parser::__expression_statement()
	{
		BEGIN(2);
		RULE1(token<';'>);
		RULE2(expression, token<';'>);
		CHECK(1, "syntax error, ';' expected");
		END();
	}

	int parser::__expression()
	{
		if (__assignment_expression())
		{
			smartptr<expression> N = d_val__;
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__assignment_expression())
				{
					roll_back_to(l);
					break;
				}
				N = new binary(',', N, d_val__);
				l = processed.size();
			}
			d_val__ = N;
			return 1;
		}
		return 0;
	}

	int parser::__assignment_expression()
	{
		BEGIN(3);
		MATCH3(unary_expression, assignment_operator, assignment_expression)
		{
			d_val__ = new binary(N[1].as<token>()->get_id(), N[0], N[2]);
			return 1;
		}

		RULE1(conditional_expression);
		END();
	}

	int parser::__assignment_operator()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case '=':			RULE1(token<'='>);	break;
		case MUL_ASSIGN:	RULE1(token<MUL_ASSIGN>);	break;
		case DIV_ASSIGN:	RULE1(token<DIV_ASSIGN>);	break;
		case MOD_ASSIGN:	RULE1(token<MOD_ASSIGN>);	break;
		case ADD_ASSIGN:	RULE1(token<ADD_ASSIGN>);	break;
		case SUB_ASSIGN:	RULE1(token<SUB_ASSIGN>);	break;
		case LEFT_ASSIGN:	RULE1(token<LEFT_ASSIGN>);	break;
		case RIGHT_ASSIGN:	RULE1(token<RIGHT_ASSIGN>);	break;
		case AND_ASSIGN:	RULE1(token<AND_ASSIGN>);	break;
		case XOR_ASSIGN:	RULE1(token<XOR_ASSIGN>);	break;
		case OR_ASSIGN:		RULE1(token<OR_ASSIGN>);	break;
		}
		END();
	}

	int parser::__conditional_expression()
	{
		BEGIN(4);
		MATCH1(logical_or_expression)
		{
			smartptr<node> N0 = N[0];
			__max = 0;
			MATCH4(token<'?'>, expression, token<':'>, conditional_expression)
			{
				smartptr<type> result_type = N0.as<expression>()->get_type();
				if (result_type.as<native_type>() && !result_type.as<native_type>()->is_scalar())
				{			// select(exp3, exp2, exp1)
					smartptr<callable> select = symbols->get<callable>("select");
					d_val__ = new call(select, new chunk(N[3], N[1], N0));
				}
				else		// C/C++ ternary selection operator
					d_val__ = new ternary(N0, N[1], N[3]);
				return 1;
			}

			CHECK(1, "syntax error");
			d_val__ = N0;
			return 1;
		}
		END();
	}

	int parser::__unary_expression()
	{
		BEGIN(2);
		switch(peek_token())
		{
		case INC_OP:
			MATCH2(token<INC_OP>, unary_expression)
			{
				d_val__ = new unary(N[0].as<token>()->get_id(), N[1]);
				return 1;
			}
			CHECK(1, "syntax error, unary expression expected");
			break;
		case DEC_OP:
			MATCH2(token<DEC_OP>, unary_expression)
			{
				d_val__ = new unary(N[0].as<token>()->get_id(), N[1]);
				return 1;
			}
			CHECK(1, "syntax error, unary expression expected");
			break;
		case SIZEOF:
			MATCH2(token<SIZEOF>, token<'('>)
			{
				MATCH2(unary_expression, token<')'>)
				{
					d_val__ = new size_of(N[0]);
					return 1;
				}
				MATCH2(type_name, token<')'>)
				{
					d_val__ = new size_of(N[0]);
					return 1;
				}
				ERROR("syntax error");
			}

			CHECK(1, "syntax error");
			break;
		default:
			RULE1(postfix_expression);
			MATCH2(unary_operator, cast_expression)
			{
				if (N[0].as<token>()->get_id() == '&' && N[1].as<swizzle>())
					ERROR("taking address of vector component is not allowed");
				if (N[0].as<token>()->get_id() == '*' && !N[1].as<expression>()->get_type().as<pointer_type>())
					ERROR("invalid type argument of unary '*' (" + N[1].as<expression>()->get_type()->get_name() + ')');
				d_val__ = new unary(N[0].as<token>()->get_id(), N[1]);
				return 1;
			}
		}
		END();
	}

	int parser::__unary_operator()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case '&':	RULE1(token<'&'>);	break;
		case '*':	RULE1(token<'*'>);	break;
		case '+':	RULE1(token<'+'>);	break;
		case '-':	RULE1(token<'-'>);	break;
		case '~':	RULE1(token<'~'>);	break;
		case '!':	RULE1(token<'!'>);	break;
		}
		END();
	}

	int parser::__type_name()
	{
		BEGIN(1);
		MATCH1(specifier_qualifier_list)
		{
			const smartptr<node> N0 = N[0];
			MATCH1(abstract_declarator)
			{
				smartptr<type> p_type = N0.as<type>();
				if (N[0].as<pointer_type>())
				{
					N[0].as<pointer_type>()->set_root_type(p_type);
					p_type = N[0].as<type>();
				}
				else
					while(N[0].as<chunk>() && N[0].as<chunk>()->front().as<pointer_type>())
					{
						smartptr<chunk> p_chunk = N[0].as<chunk>();
						p_chunk->front().as<pointer_type>()->set_root_type(p_type);
						p_type = p_chunk->front().as<type>();
						N[0] = p_chunk->back();
					}
				d_val__ = p_type;
				return 1;
			}

			d_val__ = N0;
			return 1;
		}
		END();
	}

	int parser::__cast_expression()
	{
		BEGIN(3);
		MATCH3(token<'('>, type_name, token<')'>)
		{
			const smartptr<node> N1 = N[1];
			MATCH1(cast_expression)
			{
				smartptr<cast> pcast = new cast(N1.as<type>(), N[0].as<expression>());
				if (!pcast->validate())
					ERROR("vector literals must take either 1 scalar or match vector type dimension.");
				d_val__ = pcast;
				return 1;
			}
			MATCH3(token<'{'>, initializer_list, token<'}'>)
			{
				smartptr<struct_literal> literal = new struct_literal(N1.as<type>(), N[1]);
				if (!literal->validate())
					ERROR("incorrect struct literal.");
				d_val__ = literal;
				return 1;
			}
		}

		RULE1(unary_expression);
		END();
	}

	int parser::__logical_or_expression()
	{
		LISTOF_LEFT_OP(logical_and_expression, token<OR_OP>);
		return 0;
	}

	int parser::__logical_and_expression()
	{
		LISTOF_LEFT_OP(inclusive_or_expression, token<AND_OP>);
		return 0;
	}

	int parser::__inclusive_or_expression()
	{
		LISTOF_LEFT_OP(exclusive_or_expression, token<'|'>);
		return 0;
	}

	int parser::__exclusive_or_expression()
	{
		LISTOF_LEFT_OP(and_expression, token<'^'>);
		return 0;
	}

	int parser::__and_expression()
	{
		LISTOF_LEFT_OP(equality_expression, token<'&'>);
		return 0;
	}

	int parser::__equality_operator()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case EQ_OP:	RULE1(token<EQ_OP>);	break;
		case NE_OP:	RULE1(token<NE_OP>);	break;
		}
		END();
	}

	int parser::__equality_expression()
	{
		BEGIN_SMALL();
		LISTOF_LEFT_OP(relational_expression, equality_operator);
		END();
	}

	int parser::__relational_operator()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case '<':	RULE1(token<'<'>);	break;
		case '>':	RULE1(token<'>'>);	break;
		case LE_OP:	RULE1(token<LE_OP>);	break;
		case GE_OP:	RULE1(token<GE_OP>);	break;
		}
		END();
	}

	int parser::__relational_expression()
	{
		BEGIN_SMALL();
		LISTOF_LEFT_OP(shift_expression, relational_operator);
		END();
	}

	int parser::__shift_operator()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case LEFT_OP:	RULE1(token<LEFT_OP>);	break;
		case RIGHT_OP:	RULE1(token<RIGHT_OP>);	break;
		}
		END();
	}

	int parser::__shift_expression()
	{
		BEGIN_SMALL();
		LISTOF_LEFT_OP(additive_expression, shift_operator);
		END();
	}

	int parser::__additive_operator()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case '+':	RULE1(token<'+'>);	break;
		case '-':	RULE1(token<'-'>);	break;
		}
		END();
	}

	int parser::__additive_expression()
	{
		BEGIN_SMALL();
		LISTOF_LEFT_OP(multiplicative_expression, additive_operator);
		END();
	}

	int parser::__multiplicative_operator()
	{
		BEGIN(1);
		switch(peek_token())
		{
		case '*':	RULE1(token<'*'>);	break;
		case '/':	RULE1(token<'/'>);	break;
		case '%':	RULE1(token<'%'>);	break;
		}
		END();
	}

	int parser::__multiplicative_expression()
	{
		BEGIN_SMALL();
		LISTOF_LEFT_OP(cast_expression, multiplicative_operator);
		END();
	}

	int parser::__postfix_expression_suffix()
	{
		BEGIN(3);
		switch(peek_token())
		{
		case '[':
			RULE3(token<'['>, expression, token<']'>);
			CHECK(2, "syntax error, ']' expected");
			CHECK(1, "syntax error, expression expected");
			break;
		case '(':
			RULE2(token<'('>, token<')'>);
			RULE3(token<'('>, argument_expression_list, token<')'>);
			CHECK(2, "syntax error, ')' expected");
			CHECK(1, "syntax error");
			break;
		case '.':
			RULE2(token<'.'>, identifier);
			CHECK(1, "syntax error");
			break;
		case PTR_OP:
			RULE2(token<PTR_OP>, identifier);
			CHECK(1, "syntax error, identifier expected");
			break;
		case INC_OP:
			RULE1(token<INC_OP>);
			break;
		case DEC_OP:
			RULE1(token<DEC_OP>);
			break;
		}
		END();
	}

	int parser::__postfix_expression()
	{
		BEGIN_SMALL();
		if (__primary_expression())
		{
			smartptr<node> exp = d_val__;
			while (__postfix_expression_suffix())
			{
				const int i_token = d_val__.as<token>()
								  ? d_val__.as<token>()->get_id()
								  : d_val__.as<chunk>()->front().as<token>()->get_id();
				switch(i_token)
				{
				case INC_OP:
				case DEC_OP:
					exp = new unary(i_token, exp, true);
					break;
				case '[':
					if (!exp.as<expression>()->get_type().as<pointer_type>())
					{
						ERROR("pointer or array type expected!");
					}
					else
					{
						index *idx = new index(exp, (*d_val__.as<chunk>())[1].as<expression>());
						if (b_debug_mode)
						{
							idx->enable_boundary_check(true);
							idx->set_ref_string(current_file + " l." + to_string(line));
						}
						exp = idx;
					}
					break;
				case '(':
					if (!exp.as<callable>())
						ERROR("this is not a function!");
					if (d_val__.as<chunk>()->size() == 3)
					{
						const chunk *args = (*d_val__.as<chunk>())[1].as<chunk>();
						const std::deque<smartptr<type> > &arg_types = args->get_as_types();
						const overloaded_function *overloaded = exp.as<overloaded_function>();
						if (overloaded)
						{
							const smartptr<callable> &fn = overloaded->get_function(arg_types);
							if (!fn)
								ERROR("no instance of overloaded function '" + overloaded->get_name() + "' matches the argument list");
							exp = fn;
						}

						if (!exp.as<callable>()->check_num_params(args->size()))
						{
							std::stringstream buf;
							buf << "wrong number of function parameters! (" << args->size()
								<< " instead of " << exp.as<callable>()->get_num_params() << ')';
							ERROR(buf.str());
						}
						try
						{
							if (!exp.as<callable>()->get_return_type(arg_types))
							{
								std::string arg_list("(");
								for(size_t i = 0 ; i < arg_types.size() ; ++i)
								{
									if (i)
										arg_list += ',';
									arg_list += arg_types[i]->get_name();
								}
								arg_list += ')';
								ERROR("no matching function for call to '" + exp.as<callable>()->get_name() + arg_list + "' !");
							}
						}
						catch(const std::exception &e)
						{
							std::string arg_list("(");
							for(size_t i = 0 ; i < arg_types.size() ; ++i)
							{
								if (i)
									arg_list += ',';
								arg_list += arg_types[i]->get_name();
							}
							arg_list += ')';
							std::cerr << arg_list << std::endl;
							if (exp.as<overloaded_builtin>())
								exp.as<overloaded_builtin>()->print_debug_info();
							ERROR(e.what());
						}

						exp = new call(exp, args);
					}
					else
					{
						if (exp.as<callable>()->get_num_params() != 0)
							ERROR("this function takes parameters!");
						exp = new call(exp, (chunk*)NULL);
					}
					break;
				case '.':
					{
						const expression *pexp = exp.as<expression>();
						if (!pexp)
							ERROR("syntax error: expression expected!");
						smartptr<type> p_type = pexp->get_type();
						smartptr<type_def> tdef = p_type.as<type_def>();
						if (tdef)	p_type = tdef->get_type();
						const smartptr<struct_type> stype = p_type.as<struct_type>();
						const smartptr<native_type> ntype = p_type.as<native_type>();
						if (stype)
						{
							const std::string &member_name = d_val__.as<chunk>()->back().as<token>()->get_string();
							if (!stype->has_member(member_name))
								ERROR("member not found!");
							exp = new member(exp, member_name);
						}
						else if (ntype)
						{
							if (!ntype->is_vector())
								ERROR("struct, union or vector type expected!");
							const std::string &components = d_val__.as<chunk>()->back().as<token>()->get_string();
							if (!swizzle::validate_components(components, ntype->get_dim()))
								ERROR("invalid vector components");
							exp = new swizzle(exp, components);
						}
						else
							ERROR("struct, union or vector type expected!");
					}
					break;
				case PTR_OP:
					{
						const expression *pexp = exp.as<expression>();
						if (!pexp)
							ERROR("syntax error: expression expected!");
						const smartptr<pointer_type> ptr_type = pexp->get_type().as<pointer_type>();
						if (!ptr_type)
							ERROR("base operand of '->' is not a pointer!");
						smartptr<type> p_type = ptr_type->get_base_type();
						smartptr<type_def> tdef = p_type.as<type_def>();
						if (tdef)	p_type = tdef->get_type();
						const smartptr<struct_type> stype = p_type.as<struct_type>();
						const smartptr<native_type> ntype = p_type.as<native_type>();

						if (stype)
						{
							const std::string &member_name = d_val__.as<chunk>()->back().as<token>()->get_string();
							if (!stype->has_member(member_name))
								ERROR("member not found!");
							exp = new member(exp, member_name);
						}
						else if (ntype)
						{
							if (!ntype->is_vector())
								ERROR("struct, union or vector type expected!");
							const std::string &components = d_val__.as<chunk>()->back().as<token>()->get_string();
							if (!swizzle::validate_components(components, ntype->get_dim()))
								ERROR("invalid vector components");
							exp = new swizzle(exp, components);
						}
						else
							ERROR("struct, union or vector type expected!");
					}
					break;
				}
			}
			d_val__ = exp;
			return 1;
		}
		END();
	}

	int parser::__primary_expression()
	{
		BEGIN(3);
		switch(peek_token())
		{
		case IDENTIFIER:
			MATCH1(token<IDENTIFIER>)
			{
				d_val__ = symbols->get<node>(N[0].as<token>()->get_string());
				if (!d_val__)
					ERROR("unknown symbol \"" + N[0].as<token>()->get_string() + '"');
				return 1;
			}

			break;
		case CONSTANT:
			RULE1(token<CONSTANT>);
			break;
		case STRING_LITERAL:
			RULE1(token<STRING_LITERAL>);
			break;
		case '(':
			MATCH3(token<'('>, expression, token<')'>)
			{
				d_val__ = N[1];
				return 1;
			}
			CHECK(2, "syntax error, ')' expected");
			break;
		}
		END();
	}

	int parser::__argument_expression_list()
	{
		if (__assignment_expression())
		{
			smartptr<chunk> N = new chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__assignment_expression())
				{
					roll_back_to(l);
					break;
				}
				N->push_back(d_val__);
				l = processed.size();
			}
			d_val__ = N;
			return 1;
		}
		return 0;
	}

	int parser::__init_declarator()
	{
		BEGIN(2);
		MATCH1(declarator)
		{
			smartptr<node> N0 = N[0];
			MATCH2(token<'='>, initializer)
			{
				d_val__ = new chunk(N0, N[0], N[1]);
				return 1;
			}

			d_val__ = new chunk(N0);
			return 1;
		}

		END();
	}

	int parser::__initializer()
	{
		BEGIN(2);
		RULE1(assignment_expression);
		if (peek_token() == '{')
		{
			MATCH2(token<'{'>, initializer_list)
			{
				const smartptr<node> N0 = N[0];
				const smartptr<node> N1 = N[1];
				MATCH1(token<'}'>)
				{
					d_val__ = new chunk(N0, N1, N[0]);
					return 1;
				}
				MATCH2(token<','>, token<'}'>)
				{
					d_val__ = new chunk(N0, N1, N[0], N[1]);
					return 1;
				}
			}
			CHECK(1, "syntax error");
		}
		END();
	}

	int parser::__initializer_list()
	{
		LISTOF_LEFT_SEP(initializer, token<','>);
		return 0;
	}

	int parser::__identifier()
	{
		BEGIN(1);
		RULE1(token<IDENTIFIER>);
		RULE1(token<TYPE_NAME>);
		END();
	}

	int parser::__attribute_qualifier()
	{
		BEGIN(6);
		if (peek_token() == __ATTRIBUTE__)
		{
			MATCH6(token<__ATTRIBUTE__>, token<'('>, token<'('>, attribute_list, token<')'>, token<')'>)
			{
				d_val__ = N[3];
				return 1;
			}
			CHECK(5, "')' missing");
			CHECK(4, "'))' missing after attribute-list");
			CHECK(3, "expected attribute-list");
			CHECK(2, "'(' missing");
			CHECK(1, "'((' missing after __attribute__ keyword");
		}
		smartptr<chunk> p_chunk = new chunk;
		p_chunk->push_back(new token("(", '('));
		p_chunk->push_back(new token(")", ')'));
		d_val__ = new chunk(p_chunk);
		return 1;
	}

	int parser::__attribute_list()
	{
		if (__attributeopt())
		{
			smartptr<chunk> N = new chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__attributeopt())
				{
					roll_back_to(l);
					break;
				}
				N->push_back(d_val__);
				l = processed.size();
			}
			d_val__ = N;
			return 1;
		}
		return 0;
	}

	int parser::__attributeopt()
	{
		BEGIN(2);
		RULE2(attribute_token, attribute_argument_clauseopt);

		smartptr<chunk> p_chunk = new chunk;
		p_chunk->push_back(new token("(", '('));
		p_chunk->push_back(new token(")", ')'));
		d_val__ = p_chunk;
		return 1;
	}

	int parser::__attribute_token()
	{
		return __identifier();
	}

	int parser::__attribute_argument_clauseopt()
	{
		BEGIN(3);
		RULE3(token<'('>, attribute_argument_list, token<')'>);

		smartptr<chunk> p_chunk = new chunk;
		p_chunk->push_back(new token("(", '('));
		p_chunk->push_back(new token(")", ')'));
		d_val__ = p_chunk;
		return 1;
	}

	int parser::__attribute_argument_list()
	{
		if (__attribute_argument())
		{
			smartptr<chunk> N = new chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__attribute_argument())
				{
					roll_back_to(l);
					break;
				}
				N->push_back(d_val__);
				l = processed.size();
			}
			d_val__ = N;
			return 1;
		}
		return 0;
	}

	int parser::__attribute_argument()
	{
		return __assignment_expression();
	}
	template<int token>
	inline int parser::__token()
	{
		const int next = this->read_token();
		if (token == next)
			return 1;
		roll_back();
		return 0;
	}
}
