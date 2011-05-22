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
#include "binary.h"
#include "unary.h"
#include "symbol_table.h"
#include "var.h"
#include "builtin.h"
#include "index.h"
#include "call.h"
#include "callable.h"
#include "../utils/smartptr.h"

namespace FreeOCL
{
	inline int Parser::readToken()
	{
		if (tokens.empty())
		{
			const int token = lex();
			processed.push_back(std::make_pair(token, d_val__));
			return token;
		}
		const std::pair<int, smartptr<Node> > token = tokens.back();
		tokens.pop_back();
		processed.push_back(token);
		d_val__ = token.second;
		return token.first;
	}

	inline int Parser::peekToken()
	{
		if (tokens.empty())
		{
			const int token = lex();
			tokens.push_back(std::make_pair(token, d_val__));
		}
		return tokens.back().first;
	}

	inline void Parser::rollBack()
	{
		tokens.push_back(processed.back());
		processed.pop_back();
	}

	inline void Parser::rollBackTo(size_t size)
	{
		while(processed.size() > size)
			rollBack();
	}

	int Parser::parse()
	{
		processed.clear();
		tokens.clear();
		bErrors = false;
		symbols = new SymbolTable;
		register_builtin();
		try
		{
			const int ret = __translation_unit();
			processed.clear();
			tokens.clear();
			delete symbols;
			return ret;
		}
		catch(...)
		{
			processed.clear();
			tokens.clear();
			delete symbols;
			return 0;
		}
	}

	int Parser::__translation_unit()
	{
		if (__external_declaration())
		{
			Chunk *chunk = new Chunk(d_val__);
			root = chunk;
			while(__external_declaration())
				chunk->push_back(d_val__);
		}
		else
			root = NULL;
		return 1;
	}

	int Parser::__external_declaration()
	{
		BEGIN();
		RULE1(function_definition);
		RULE1(declaration);
		END();
	}

	int Parser::__function_definition()
	{
		BEGIN();
		const bool b_qualifier = __function_qualifier();
		MATCH2(declaration_specifiers, declarator)
		{
			smartptr<Type> type = N[0];
			smartptr<Chunk> chunk = N[1];
			smartptr<PointerType> ptr = chunk->front().as<PointerType>();
			if (ptr)
			{
				ptr->setRootType(type);
				type = ptr;
				chunk = chunk->back();
			}

			// Push a new scope
			symbols->push();
			smartptr<Chunk> args = chunk->back();
			if (args->size() == 3)
			{
				// Register all function parameters
				args = (*args)[1];
				for(size_t i = 0 ; i < args->size() ; ++i)
				{
					const smartptr<Chunk> cur = (*args)[i].as<Chunk>();
					const smartptr<Type> type = cur->front().as<Type>();
					std::string name = cur->back().as<Token>()
									   ? cur->back().as<Token>()->getString()
									   : cur->back().as<Chunk>()->front().as<Token>()->getString();
					symbols->insert(name, new Var(name, type));
				}
			}

			__declaration_list();		// Ignore it for now
			if (__compound_statement())
			{
				symbols->pop();
				smartptr<Node> statement = d_val__;
				const std::string function_name = chunk->front().as<Token>()->getString();
				if (b_qualifier)
				{
					if (*type != NativeType(NativeType::VOID, false, Type::PRIVATE))
						error("return type for kernels must be void");
					d_val__ = kernels[function_name] = new Kernel(type, function_name, chunk->back(), statement);
				}
				else
					d_val__ = new Function(type, function_name, chunk->back(), statement);
				return 1;
			}
			symbols->pop();
			ERROR("syntax error: compound statement expected!");
		}
		if (b_qualifier)
			ERROR("syntax error: function definition expected!");

//		RULE3(declarator, declaration_list, compound_statement);
//		RULE2(declarator, compound_statement);
		END();
	}

	int Parser::__declaration()
	{
		BEGIN();

		if (__declaration_specifiers())
		{
			smartptr<Type> type = d_val__;
			if(__token<';'>())
			{
				warning("declaration doesn't declare anything!");
				d_val__ = new Token("", 0);
				return 1;
			}

			MATCH2(init_declarator_list, token<';'>)
			{
				// register variables
				const smartptr<Chunk> var_list = N[0].as<Chunk>();
				for(size_t i = 0 ; i < var_list->size() ; i += 2)
				{
					const smartptr<Chunk> declarator = (*var_list)[i].as<Chunk>()->front().as<Chunk>();
					smartptr<Type> l_type;
					std::string name;
					if (declarator->front().as<PointerType>())
					{
						smartptr<PointerType> ptr = declarator->front().as<PointerType>()->clone();
						ptr->setRootType(type);
						l_type = ptr;

						name = declarator->back().as<Chunk>()->front().as<Token>()->getString();
					}
					else
					{
						l_type = type;
						name = declarator->front().as<Token>()->getString();
					}
					symbols->insert(name, new Var(name, l_type));
				}
				d_val__ = new Chunk(type, N[0], N[1]);
				return 1;
			}
			ERROR("syntax error, ';' expected");
		}

		END();
	}

	int Parser::__declaration_specifier()
	{
		BEGIN();
		RULE1(storage_class_specifier);
		RULE1(type_specifier);
		RULE1(type_qualifier);
		END();
	}

	int Parser::__declaration_specifiers()
	{
		if (__declaration_specifier())
		{
			smartptr<Type> type;
			Type::AddressSpace address_space = Type::PRIVATE;
			bool b_address_space_set = false;
			bool b_const = false;
			do
			{
				if (d_val__.as<Type>())
					type = d_val__;
				else if (d_val__.as<Token>())
				{
					Token *token = d_val__.as<Token>();
					switch(token->getID())
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
						address_space = Type::GLOBAL;
						break;
					case __LOCAL:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						address_space = Type::LOCAL;
						break;
					case __PRIVATE:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						address_space = Type::PRIVATE;
						break;
					case __CONSTANT:
						if (b_address_space_set)
							ERROR("2 address space qualifiers");
						b_address_space_set = true;
						address_space = Type::CONSTANT;
						break;
					}
				}
			}
			while (__declaration_specifier());
			if (!type)
				ERROR("syntax error: missing type");
			if (b_const && address_space == Type::CONSTANT)
				warning("const used with __constant address space");
			d_val__ = type->clone(b_const, address_space);
			return 1;
		}
		return 0;
	}

	int Parser::__init_declarator_list()
	{
		if (__init_declarator())
		{
			smartptr<Chunk> N = new Chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				N->push_back(d_val__);
				if (!__init_declarator())
				{
					rollBackTo(l);
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

	int Parser::__declarator()
	{
		BEGIN();
		RULE2(pointer, direct_declarator);
		RULE1(direct_declarator);
		END();
	}

	int Parser::__declaration_list()
	{
		LISTOF_LEFT(declaration);
		return 0;
	}

	int Parser::__pointer()
	{
		BEGIN();
		if (peekToken() == '*')
		{
			smartptr<Type> ptr;
			while(__token<'*'>())
			{
				bool b_const = false;
				bool b_address_space_set = false;
				Type::AddressSpace address_space = Type::PRIVATE;
				if (__type_qualifier_list())
				{
					Chunk *chunk = d_val__.as<Chunk>();
					for(size_t i = 0 ; i < chunk->size() ; ++i)
					{
						switch((*chunk)[i].as<Token>()->getID())
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
							address_space = Type::GLOBAL;
							break;
						case __LOCAL:
							if (b_address_space_set)
								ERROR("2 address space qualifiers");
							b_address_space_set = true;
							address_space = Type::LOCAL;
							break;
						case __CONSTANT:
							if (b_address_space_set)
								ERROR("2 address space qualifiers");
							b_address_space_set = true;
							address_space = Type::CONSTANT;
							break;
						case __PRIVATE:
							if (b_address_space_set)
								ERROR("2 address space qualifiers");
							b_address_space_set = true;
							address_space = Type::PRIVATE;
							break;
						}
					}
				}
				ptr = new PointerType(ptr, b_const, address_space);
			}
			d_val__ = ptr;
			return 1;
		}
		END();
	}

	int Parser::__function_qualifier()
	{
		return __token<__KERNEL>();
	}

	int Parser::__storage_class_specifier()
	{
		return __token<TYPEDEF>();
	}

	int Parser::__type_specifier()
	{
		BEGIN();
		switch(readToken())
		{
		case BOOL:		d_val__ = new NativeType(NativeType::BOOL, false, Type::PRIVATE);		return 1;
		case HALF:		d_val__ = new NativeType(NativeType::HALF, false, Type::PRIVATE);		return 1;
		case VOID:		d_val__ = new NativeType(NativeType::VOID, false, Type::PRIVATE);		return 1;
		case CHAR:		d_val__ = new NativeType(NativeType::CHAR, false, Type::PRIVATE);		return 1;
		case SHORT:		d_val__ = new NativeType(NativeType::SHORT, false, Type::PRIVATE);		return 1;
		case INT:		d_val__ = new NativeType(NativeType::INT, false, Type::PRIVATE);		return 1;
		case LONG:		d_val__ = new NativeType(NativeType::LONG, false, Type::PRIVATE);		return 1;
		case UCHAR:		d_val__ = new NativeType(NativeType::UCHAR, false, Type::PRIVATE);		return 1;
		case USHORT:	d_val__ = new NativeType(NativeType::USHORT, false, Type::PRIVATE);		return 1;
		case UINT:		d_val__ = new NativeType(NativeType::UINT, false, Type::PRIVATE);		return 1;
		case ULONG:		d_val__ = new NativeType(NativeType::ULONG, false, Type::PRIVATE);		return 1;
		case FLOAT:		d_val__ = new NativeType(NativeType::FLOAT, false, Type::PRIVATE);		return 1;
		case DOUBLE:	d_val__ = new NativeType(NativeType::DOUBLE, false, Type::PRIVATE);		return 1;
		case CHAR2:		d_val__ = new NativeType(NativeType::CHAR2, false, Type::PRIVATE);		return 1;
		case SHORT2:	d_val__ = new NativeType(NativeType::SHORT2, false, Type::PRIVATE);		return 1;
		case INT2:		d_val__ = new NativeType(NativeType::INT2, false, Type::PRIVATE);		return 1;
		case LONG2:		d_val__ = new NativeType(NativeType::LONG2, false, Type::PRIVATE);		return 1;
		case UCHAR2:	d_val__ = new NativeType(NativeType::UCHAR2, false, Type::PRIVATE);		return 1;
		case USHORT2:	d_val__ = new NativeType(NativeType::USHORT2, false, Type::PRIVATE);	return 1;
		case UINT2:		d_val__ = new NativeType(NativeType::UINT2, false, Type::PRIVATE);		return 1;
		case ULONG2:	d_val__ = new NativeType(NativeType::ULONG2, false, Type::PRIVATE);		return 1;
		case FLOAT2:	d_val__ = new NativeType(NativeType::FLOAT2, false, Type::PRIVATE);		return 1;
		case DOUBLE2:	d_val__ = new NativeType(NativeType::DOUBLE2, false, Type::PRIVATE);	return 1;
		case CHAR3:		d_val__ = new NativeType(NativeType::CHAR3, false, Type::PRIVATE);		return 1;
		case SHORT3:	d_val__ = new NativeType(NativeType::SHORT3, false, Type::PRIVATE);		return 1;
		case INT3:		d_val__ = new NativeType(NativeType::INT3, false, Type::PRIVATE);		return 1;
		case LONG3:		d_val__ = new NativeType(NativeType::LONG3, false, Type::PRIVATE);		return 1;
		case UCHAR3:	d_val__ = new NativeType(NativeType::UCHAR3, false, Type::PRIVATE);		return 1;
		case USHORT3:	d_val__ = new NativeType(NativeType::USHORT3, false, Type::PRIVATE);	return 1;
		case UINT3:		d_val__ = new NativeType(NativeType::UINT3, false, Type::PRIVATE);		return 1;
		case ULONG3:	d_val__ = new NativeType(NativeType::ULONG3, false, Type::PRIVATE);		return 1;
		case FLOAT3:	d_val__ = new NativeType(NativeType::FLOAT3, false, Type::PRIVATE);		return 1;
		case DOUBLE3:	d_val__ = new NativeType(NativeType::DOUBLE3, false, Type::PRIVATE);	return 1;
		case CHAR4:		d_val__ = new NativeType(NativeType::CHAR4, false, Type::PRIVATE);		return 1;
		case SHORT4:	d_val__ = new NativeType(NativeType::SHORT4, false, Type::PRIVATE);		return 1;
		case INT4:		d_val__ = new NativeType(NativeType::INT4, false, Type::PRIVATE);		return 1;
		case LONG4:		d_val__ = new NativeType(NativeType::LONG4, false, Type::PRIVATE);		return 1;
		case UCHAR4:	d_val__ = new NativeType(NativeType::UCHAR4, false, Type::PRIVATE);		return 1;
		case USHORT4:	d_val__ = new NativeType(NativeType::USHORT4, false, Type::PRIVATE);	return 1;
		case UINT4:		d_val__ = new NativeType(NativeType::UINT4, false, Type::PRIVATE);		return 1;
		case ULONG4:	d_val__ = new NativeType(NativeType::ULONG4, false, Type::PRIVATE);		return 1;
		case FLOAT4:	d_val__ = new NativeType(NativeType::FLOAT4, false, Type::PRIVATE);		return 1;
		case DOUBLE4:	d_val__ = new NativeType(NativeType::DOUBLE4, false, Type::PRIVATE);	return 1;
		case CHAR8:		d_val__ = new NativeType(NativeType::CHAR8, false, Type::PRIVATE);		return 1;
		case SHORT8:	d_val__ = new NativeType(NativeType::SHORT8, false, Type::PRIVATE);		return 1;
		case INT8:		d_val__ = new NativeType(NativeType::INT8, false, Type::PRIVATE);		return 1;
		case LONG8:		d_val__ = new NativeType(NativeType::LONG8, false, Type::PRIVATE);		return 1;
		case UCHAR8:	d_val__ = new NativeType(NativeType::UCHAR8, false, Type::PRIVATE);		return 1;
		case USHORT8:	d_val__ = new NativeType(NativeType::USHORT8, false, Type::PRIVATE);	return 1;
		case UINT8:		d_val__ = new NativeType(NativeType::UINT8, false, Type::PRIVATE);		return 1;
		case ULONG8:	d_val__ = new NativeType(NativeType::ULONG8, false, Type::PRIVATE);		return 1;
		case FLOAT8:	d_val__ = new NativeType(NativeType::FLOAT8, false, Type::PRIVATE);		return 1;
		case DOUBLE8:	d_val__ = new NativeType(NativeType::DOUBLE8, false, Type::PRIVATE);	return 1;
		case CHAR16:	d_val__ = new NativeType(NativeType::CHAR16, false, Type::PRIVATE);		return 1;
		case SHORT16:	d_val__ = new NativeType(NativeType::SHORT16, false, Type::PRIVATE);	return 1;
		case INT16:		d_val__ = new NativeType(NativeType::INT16, false, Type::PRIVATE);		return 1;
		case LONG16:	d_val__ = new NativeType(NativeType::LONG16, false, Type::PRIVATE);		return 1;
		case UCHAR16:	d_val__ = new NativeType(NativeType::UCHAR16, false, Type::PRIVATE);	return 1;
		case USHORT16:	d_val__ = new NativeType(NativeType::USHORT16, false, Type::PRIVATE);	return 1;
		case UINT16:	d_val__ = new NativeType(NativeType::UINT16, false, Type::PRIVATE);		return 1;
		case ULONG16:	d_val__ = new NativeType(NativeType::ULONG16, false, Type::PRIVATE);	return 1;
		case FLOAT16:	d_val__ = new NativeType(NativeType::FLOAT16, false, Type::PRIVATE);	return 1;
		case DOUBLE16:	d_val__ = new NativeType(NativeType::DOUBLE16, false, Type::PRIVATE);	return 1;
		case SIZE_T:	d_val__ = new NativeType(NativeType::SIZE_T, false, Type::PRIVATE);		return 1;

		case SIGNED:	return 1;
		case UNSIGNED:	return 1;
		case TYPE_NAME:	return 1;
		default:
			rollBack();
			RULE1(struct_or_union_specifier);
			RULE1(enum_specifier);
		}
		END();
	}

	int Parser::__type_qualifier()
	{
		BEGIN();
		switch(peekToken())
		{
		case CONST:			RULE1(token<CONST>);		break;
		case VOLATILE:		RULE1(token<VOLATILE>);		break;
		case __GLOBAL:		RULE1(token<__GLOBAL>);		break;
		case __LOCAL:		RULE1(token<__LOCAL>);		break;
		case __PRIVATE:		RULE1(token<__PRIVATE>);	break;
		case __CONSTANT:	RULE1(token<__CONSTANT>);	break;
		}
		END();
	}

	int Parser::__type_qualifier_list()
	{
		BEGIN();
		if (__type_qualifier())
		{
			smartptr<Chunk> N = new Chunk(d_val__);
			while (__type_qualifier())
				N->push_back(d_val__);
			d_val__ = N;
			return 1;
		}
		END();
	}

	int Parser::__enum_specifier()
	{
		BEGIN();
		if (peekToken() != ENUM)
			END();
		RULE4(token<ENUM>, token<'{'>, enumerator_list, token<'}'>);
		RULE5(token<ENUM>, token<IDENTIFIER>, token<'{'>, enumerator_list, token<'}'>);
		RULE2(token<ENUM>, token<IDENTIFIER>);
		CHECK(1, "syntax error");
		END();
	}

	int Parser::__enumerator_list()
	{
		BEGIN();
		LISTOF_LEFT_SEP(enumerator, token<','>);
		END();
	}

	int Parser::__enumerator()
	{
		BEGIN();
		if (peekToken() == IDENTIFIER)
		{
			RULE3(token<IDENTIFIER>, token<'='>, constant_expression);
			RULE1(token<IDENTIFIER>);
		}
		END();
	}

	int Parser::__direct_declarator_suffix()
	{
		BEGIN();
		switch(peekToken())
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

	int Parser::__direct_declarator_base()
	{
		BEGIN();
		RULE1(token<IDENTIFIER>);
		RULE3(token<'('>, declarator, token<')'>);
		END();
	}

	int Parser::__direct_declarator()
	{
		BEGIN();
		if (__direct_declarator_base())
		{
			smartptr<Chunk> N = new Chunk(d_val__);
			while(__direct_declarator_suffix())
				N->push_back(d_val__);
			d_val__ = N;
			return 1;
		}
		END();
	}

	int Parser::__identifier_list()
	{
		BEGIN();
		LISTOF_LEFT_SEP(token<IDENTIFIER>, token<','>);
		END();
	}

	int Parser::__parameter_type_list()
	{
		return __parameter_list();
	}

	int Parser::__parameter_list()
	{
		if (__parameter_declaration())
		{
			smartptr<Chunk> N = new Chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__parameter_declaration())
				{
					rollBackTo(l);
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

	int Parser::__parameter_declaration()
	{
		BEGIN();
		MATCH2(declaration_specifiers, declarator)
		{
			smartptr<Chunk> chunk = N[1].as<Chunk>();
			smartptr<PointerType> ptr = chunk->front().as<PointerType>();
			if (ptr)
			{
				ptr->setRootType(N[0].as<Type>());
				N[0] = ptr;
				N[1] = chunk->back();
			}
			d_val__ = new Chunk(N[0], N[1]);
			return 1;
		}
		RULE2(declaration_specifiers, abstract_declarator);
		RULE1(declaration_specifiers);
		END();
	}

	int Parser::__abstract_declarator()
	{
		BEGIN();
		RULE2(pointer, direct_abstract_declarator);
		RULE1(pointer);
		RULE1(direct_abstract_declarator);
		END();
	}

	int Parser::__direct_abstract_declarator_base()
	{
		BEGIN();
		switch(peekToken())
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

	int Parser::__direct_abstract_declarator_suffix()
	{
		BEGIN();
		switch(peekToken())
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

	int Parser::__direct_abstract_declarator()
	{
		BEGIN();
		if (__direct_abstract_declarator_base())
		{
			smartptr<Node> N = d_val__;
			while(__direct_abstract_declarator_suffix())
				N = new Chunk(N, d_val__);
			d_val__ = N;
			return 1;
		}
		END();
	}

	int Parser::__constant_expression()
	{
		return __conditional_expression();
	}

	int Parser::__struct_or_union_specifier()
	{
		BEGIN();
		RULE5(struct_or_union, token<IDENTIFIER>, token<'{'>, struct_declaration_list, token<'}'>);
		RULE4(struct_or_union, token<'{'>, struct_declaration_list, token<'}'>);
		RULE2(struct_or_union, token<IDENTIFIER>);
		CHECK(2, "syntax error");
		CHECK(1, "syntax error, '{' or identifier expected");
		END();
	}

	int Parser::__struct_or_union()
	{
		BEGIN();
		RULE1(token<STRUCT>);
		RULE1(token<UNION>);
		END();
	}

	int Parser::__struct_declaration_list()
	{
		BEGIN();
		LISTOF_LEFT(struct_declaration);
		END();
	}

	int Parser::__struct_declaration()
	{
		BEGIN();
		RULE3(specifier_qualifier_list, struct_declarator_list, token<';'>);
		END();
	}

	int Parser::__struct_declarator_list()
	{
		BEGIN();
		LISTOF_LEFT_SEP(struct_declarator, token<','>);
		END();
	}

	int Parser::__struct_declarator()
	{
		BEGIN();
		RULE1(declarator);
		RULE2(token<':'>, constant_expression);
		RULE3(declarator, token<':'>, constant_expression);
		END();
	}

	int Parser::__specifier_qualifier()
	{
		BEGIN();
		RULE1(type_specifier);
		RULE1(type_qualifier);
		END();
	}

	int Parser::__specifier_qualifier_list()
	{
		BEGIN();
		LISTOF_RIGHT(specifier_qualifier);
		END();
	}

	int Parser::__compound_statement()
	{
		BEGIN();
		if (peekToken() == '{')
		{
			RULE2(token<'{'>, token<'}'>);
			RULE3(token<'{'>, declaration_statement_list, token<'}'>);
			CHECK(1, "syntax error");
		}
		END();
	}

	int Parser::__declaration_statement_list()
	{
		BEGIN();
		LISTOF_LEFT(declaration_statement);
		END();
	}

	int Parser::__declaration_statement()
	{
		BEGIN();
		RULE1(declaration);
		RULE1(statement);
		END();
	}

	int Parser::__statement_list()
	{
		BEGIN();
		LISTOF_LEFT(statement);
		END();
	}

	int Parser::__statement()
	{
		BEGIN();
		RULE1(labeled_statement);
		RULE1(compound_statement);
		RULE1(expression_statement);
		RULE1(selection_statement);
		RULE1(iteration_statement);
		RULE1(jump_statement);
		END();
	}

	int Parser::__labeled_statement()
	{
		BEGIN();
		switch(peekToken())
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

	int Parser::__jump_statement()
	{
		BEGIN();
		switch(peekToken())
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

	int Parser::__iteration_statement()
	{
		BEGIN();
		switch(peekToken())
		{
		case WHILE:
			RULE5(token<WHILE>, token<'('>, expression, token<')'>, statement);
			CHECK(4, "syntax error, statement expected");
			CHECK(3, "syntax error, ')' expected");
			CHECK(2, "syntax error, expression expected");
			CHECK(1, "syntax error, '(' expected");
			break;
		case DO:
			RULE7(token<DO>, statement, token<WHILE>, token<'('>, expression, token<')'>, token<';'>);
			CHECK(6, "syntax error, ';' expected");
			CHECK(5, "syntax error, ')' expected");
			CHECK(4, "syntax error, expression expected");
			CHECK(3, "syntax error, '(' expected");
			CHECK(2, "syntax error, \"while\" expected");
			CHECK(1, "syntax error, statement expected");
			break;
		case FOR:
			RULE6(token<FOR>, token<'('>, expression_statement, expression_statement, token<')'>, statement);
			RULE7(token<FOR>, token<'('>, expression_statement, expression_statement, expression, token<')'>, statement);
			RULE6(token<FOR>, token<'('>, declaration, expression_statement, token<')'>, statement);
			RULE7(token<FOR>, token<'('>, declaration, expression_statement, expression, token<')'>, statement);
			CHECK(2, "syntax error");
			CHECK(1, "syntax error, '(' expected");
			break;
		}
		END();
	}

	int Parser::__selection_statement()
	{
		BEGIN();
		switch(peekToken())
		{
		case IF:
			RULE7(token<IF>, token<'('>, expression, token<')'>, statement, token<ELSE>, statement);
			RULE5(token<IF>, token<'('>, expression, token<')'>, statement);
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

	int Parser::__expression_statement()
	{
		BEGIN();
		RULE1(token<';'>);
		RULE2(expression, token<';'>);
		CHECK(1, "syntax error, ';' expected");
		END();
	}

	int Parser::__expression()
	{
		if (__assignment_expression())
		{
			smartptr<Expression> N = d_val__;
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__assignment_expression())
				{
					rollBackTo(l);
					break;
				}
				N = new Binary(',', N, d_val__);
				l = processed.size();
			}
			d_val__ = N;
			return 1;
		}
		return 0;
	}

	int Parser::__assignment_expression()
	{
		BEGIN();
		MATCH3(unary_expression, assignment_operator, assignment_expression)
		{
			d_val__ = new Binary(N[1].as<Token>()->getID(), N[0], N[2]);
			return 1;
		}

		RULE1(conditional_expression);
		END();
	}

	int Parser::__assignment_operator()
	{
		BEGIN();
		switch(peekToken())
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

	int Parser::__conditional_expression()
	{
		BEGIN();
		RULE5(logical_or_expression, token<'?'>, expression, token<':'>, conditional_expression);
		CHECK(2, "syntax error");
		RULE1(logical_or_expression);
		END();
	}

	int Parser::__unary_expression()
	{
		BEGIN();
		switch(peekToken())
		{
		case INC_OP:
			MATCH2(token<INC_OP>, unary_expression)
			{
				d_val__ = new Unary(N[0].as<Token>()->getID(), N[1]);
				return 1;
			}
			CHECK(1, "syntax error, unary expression expected");
			break;
		case DEC_OP:
			MATCH2(token<DEC_OP>, unary_expression)
			{
				d_val__ = new Unary(N[0].as<Token>()->getID(), N[1]);
				return 1;
			}
			CHECK(1, "syntax error, unary expression expected");
			break;
		case SIZEOF:
			MATCH2(token<SIZEOF>, unary_expression)
			{
				d_val__ = new Unary(N[0].as<Token>()->getID(), N[1]);
				return 1;
			}
			RULE4(token<SIZEOF>, token<'('>, type_name, token<')'>);
			CHECK(1, "syntax error");
			break;
		default:
			RULE1(postfix_expression);
			MATCH2(unary_operator, cast_expression)
			{
				d_val__ = new Unary(N[0].as<Token>()->getID(), N[1]);
				return 1;
			}
		}
		END();
	}

	int Parser::__unary_operator()
	{
		BEGIN();
		switch(peekToken())
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

	int Parser::__type_name()
	{
		BEGIN();
		RULE2(specifier_qualifier_list, abstract_declarator);
		RULE1(specifier_qualifier_list);
		END();
	}

	int Parser::__cast_expression()
	{
		BEGIN();
		RULE1(unary_expression);
		RULE4(token<'('>, type_name, token<')'>, cast_expression);
		END();
	}

	int Parser::__logical_or_expression()
	{
		LISTOF_LEFT_OP(logical_and_expression, token<OR_OP>);
		return 0;
	}

	int Parser::__logical_and_expression()
	{
		LISTOF_LEFT_OP(inclusive_or_expression, token<AND_OP>);
		return 0;
	}

	int Parser::__inclusive_or_expression()
	{
		LISTOF_LEFT_OP(exclusive_or_expression, token<'|'>);
		return 0;
	}

	int Parser::__exclusive_or_expression()
	{
		LISTOF_LEFT_OP(and_expression, token<'^'>);
		return 0;
	}

	int Parser::__and_expression()
	{
		LISTOF_LEFT_OP(equality_expression, token<'&'>);
		return 0;
	}

	int Parser::__equality_operator()
	{
		BEGIN();
		switch(peekToken())
		{
		case EQ_OP:	RULE1(token<EQ_OP>);	break;
		case NE_OP:	RULE1(token<NE_OP>);	break;
		}
		END();
	}

	int Parser::__equality_expression()
	{
		BEGIN();
		LISTOF_LEFT_OP(relational_expression, equality_operator);
		END();
	}

	int Parser::__relational_operator()
	{
		BEGIN();
		switch(peekToken())
		{
		case '<':	RULE1(token<'<'>);	break;
		case '>':	RULE1(token<'>'>);	break;
		case LE_OP:	RULE1(token<LE_OP>);	break;
		case GE_OP:	RULE1(token<GE_OP>);	break;
		}
		END();
	}

	int Parser::__relational_expression()
	{
		BEGIN();
		LISTOF_LEFT_OP(shift_expression, relational_operator);
		END();
	}

	int Parser::__shift_operator()
	{
		BEGIN();
		switch(peekToken())
		{
		case LEFT_OP:	RULE1(token<LEFT_OP>);	break;
		case RIGHT_OP:	RULE1(token<RIGHT_OP>);	break;
		}
		END();
	}

	int Parser::__shift_expression()
	{
		BEGIN();
		LISTOF_LEFT_OP(additive_expression, shift_operator);
		END();
	}

	int Parser::__additive_operator()
	{
		BEGIN();
		switch(peekToken())
		{
		case '+':	RULE1(token<'+'>);	break;
		case '-':	RULE1(token<'-'>);	break;
		}
		END();
	}

	int Parser::__additive_expression()
	{
		BEGIN();
		LISTOF_LEFT_OP(multiplicative_expression, additive_operator);
		END();
	}

	int Parser::__multiplicative_operator()
	{
		BEGIN();
		switch(peekToken())
		{
		case '*':	RULE1(token<'*'>);	break;
		case '/':	RULE1(token<'/'>);	break;
		case '%':	RULE1(token<'%'>);	break;
		}
		END();
	}

	int Parser::__multiplicative_expression()
	{
		BEGIN();
		LISTOF_LEFT_OP(cast_expression, multiplicative_operator);
		END();
	}

	int Parser::__postfix_expression_suffix()
	{
		BEGIN();
		switch(peekToken())
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
			RULE2(token<'.'>, token<IDENTIFIER>);
			CHECK(1, "syntax error");
			break;
		case PTR_OP:
			RULE2(token<PTR_OP>, token<IDENTIFIER>);
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

	int Parser::__postfix_expression()
	{
		BEGIN();
		if (__primary_expression())
		{
			smartptr<Node> exp = d_val__;
			while (__postfix_expression_suffix())
			{
				const int token = d_val__.as<Token>()
								  ? d_val__.as<Token>()->getID()
								  : d_val__.as<Chunk>()->front().as<Token>()->getID();
				switch(token)
				{
				case INC_OP:
				case DEC_OP:
					exp = new Unary(token, exp, true);
					break;
				case '[':
					exp = new Index(exp, (*d_val__.as<Chunk>())[1].as<Expression>());
					break;
				case '(':
					if (!exp.as<Callable>())
						ERROR("this is not a function!");
					if (d_val__.as<Chunk>()->size() == 3)
					{
						smartptr<Chunk> args = (*d_val__.as<Chunk>())[1];
						if (exp.as<Callable>()->getNumParams() != args->size())
							ERROR("wrong number of function parameters!");
						exp = new Call(exp, args);
					}
					else
					{
						if (exp.as<Callable>()->getNumParams() != 0)
							ERROR("this function doesn't take parameters!");
						exp = new Call(exp, (Chunk*)NULL);
					}
					break;
				case '.':
					break;
				case PTR_OP:
					break;
				}
			}
			d_val__ = exp;
			return 1;
		}
		END();
	}

	int Parser::__primary_expression()
	{
		BEGIN();
		switch(peekToken())
		{
		case IDENTIFIER:
			MATCH1(token<IDENTIFIER>)
			{
				d_val__ = symbols->get<Node>(N[0].as<Token>()->getString());
				if (!d_val__)
					ERROR("unknown symbol");
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
			CHECK(1, "syntax error, expression expected");
			break;
		}
		END();
	}

	int Parser::__argument_expression_list()
	{
		if (__assignment_expression())
		{
			smartptr<Chunk> N = new Chunk(d_val__);
			size_t l = processed.size();
			while (__token<','>())
			{
				if (!__assignment_expression())
				{
					rollBackTo(l);
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

	int Parser::__init_declarator()
	{
		BEGIN();
		RULE3(declarator, token<'='>, initializer);
		MATCH1(declarator)
		{
			d_val__ = new Chunk(N[0]);
			return 1;
		}

		END();
	}

	int Parser::__initializer()
	{
		BEGIN();
		RULE1(assignment_expression);
		if (peekToken() == '{')
		{
			RULE3(token<'{'>, initializer_list, token<'}'>);
			RULE4(token<'{'>, initializer_list, token<','>, token<'}'>);
			CHECK(1, "syntax error");
		}
		END();
	}

	int Parser::__initializer_list()
	{
		LISTOF_LEFT_SEP(initializer, token<','>);
		return 0;
	}
}
