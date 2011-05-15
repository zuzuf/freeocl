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

namespace FreeOCL
{
	int Parser::readToken()
	{
		if (tokens.empty())
		{
			const int token = lex();
			processed.push_back(std::make_pair(token, d_val__));
			return token;
		}
		const std::pair<int, Node> token = tokens.back();
		tokens.pop_back();
		processed.push_back(token);
		d_val__ = token.second;
		return token.first;
	}

	void Parser::rollBack()
	{
		tokens.push_back(processed.back());
		processed.pop_back();
	}

	void Parser::rollBackTo(size_t size)
	{
		while(processed.size() > size)
			rollBack();
	}

	int Parser::parse()
	{
		bErrors = false;
		try
		{
			return __translation_unit();
		}
		catch(...)
		{
			return 0;
		}
	}

	int Parser::__translation_unit()
	{
		if (__external_declaration())
		{
			root = d_val__;
			while(__external_declaration())
				root = Node(root, d_val__);
		}
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
		RULE4(declaration_specifiers, declarator, declaration_list, compound_statement);
		RULE3(declaration_specifiers, declarator, compound_statement);
		MATCH5(function_qualifier, declaration_specifiers, declarator, declaration_list, compound_statement)
		{
			if (d_val__[1].getValue() != "void")
				error("return type for kernels must be void");
			const std::string kernel_name = d_val__[2].front().getValue();
			kernels[kernel_name] = d_val__[2][2].clone();
			kernels[kernel_name].pop_front();
			kernels[kernel_name].pop_back();
			return 1;
		}
		MATCH4(function_qualifier, declaration_specifiers, declarator, compound_statement)
		{
			if (d_val__[1].getValue() != "void")
				error("return type for kernels must be void");
			const std::string kernel_name = d_val__[2].front().getValue();
			kernels[kernel_name] = d_val__[2][2].clone();
			return 1;
		}
		RULE3(declarator, declaration_list, compound_statement);
		RULE2(declarator, compound_statement);
		END();
	}

	int Parser::__declaration()
	{
		BEGIN();

		RULE2(declaration_specifiers, token<';'>);
		RULE3(declaration_specifiers, init_declarator_list, token<';'>);
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
		LISTOF_RIGHT(declaration_specifier);
		return 0;
	}

	int Parser::__init_declarator_list()
	{
		LISTOF_LEFT_SEP(init_declarator, token<','>);
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
		RULE3(token<'*'>, type_qualifier_list, pointer);
		RULE2(token<'*'>, type_qualifier_list);
		RULE2(token<'*'>, pointer);
		RULE1(token<'*'>);
		END();
	}

	int Parser::__function_qualifier()
	{
		BEGIN();
		RULE1(token<__KERNEL>);
		END();
	}

	int Parser::__storage_class_specifier()
	{
		BEGIN();
		RULE1(token<TYPEDEF>);
		END();
	}

	int Parser::__type_specifier()
	{
		BEGIN();
		RULE1(token<VOID>);
		RULE1(token<CHAR>);
		RULE1(token<SHORT>);
		RULE1(token<INT>);
		RULE1(token<LONG>);
		RULE1(token<UCHAR>);
		RULE1(token<USHORT>);
		RULE1(token<UINT>);
		RULE1(token<ULONG>);
		RULE1(token<FLOAT>);
		RULE1(token<DOUBLE>);
		RULE1(token<CHAR2>);
		RULE1(token<SHORT2>);
		RULE1(token<INT2>);
		RULE1(token<LONG2>);
		RULE1(token<UCHAR2>);
		RULE1(token<USHORT2>);
		RULE1(token<UINT2>);
		RULE1(token<ULONG2>);
		RULE1(token<FLOAT2>);
		RULE1(token<DOUBLE2>);
		RULE1(token<CHAR3>);
		RULE1(token<SHORT3>);
		RULE1(token<INT3>);
		RULE1(token<LONG3>);
		RULE1(token<UCHAR3>);
		RULE1(token<USHORT3>);
		RULE1(token<UINT3>);
		RULE1(token<ULONG3>);
		RULE1(token<FLOAT3>);
		RULE1(token<DOUBLE3>);
		RULE1(token<CHAR4>);
		RULE1(token<SHORT4>);
		RULE1(token<INT4>);
		RULE1(token<LONG4>);
		RULE1(token<UCHAR4>);
		RULE1(token<USHORT4>);
		RULE1(token<UINT4>);
		RULE1(token<ULONG4>);
		RULE1(token<FLOAT4>);
		RULE1(token<DOUBLE4>);
		RULE1(token<CHAR8>);
		RULE1(token<SHORT8>);
		RULE1(token<INT8>);
		RULE1(token<LONG8>);
		RULE1(token<UCHAR8>);
		RULE1(token<USHORT8>);
		RULE1(token<UINT8>);
		RULE1(token<ULONG8>);
		RULE1(token<FLOAT8>);
		RULE1(token<DOUBLE8>);
		RULE1(token<CHAR16>);
		RULE1(token<SHORT16>);
		RULE1(token<INT16>);
		RULE1(token<LONG16>);
		RULE1(token<UCHAR16>);
		RULE1(token<USHORT16>);
		RULE1(token<UINT16>);
		RULE1(token<ULONG16>);
		RULE1(token<FLOAT16>);
		RULE1(token<DOUBLE16>);
		RULE1(token<SIZE_T>);
		RULE1(token<SIGNED>);
		RULE1(token<UNSIGNED>);
		RULE1(struct_or_union_specifier);
		RULE1(enum_specifier);
		RULE1(token<TYPE_NAME>);
		END();
	}

	int Parser::__type_qualifier()
	{
		BEGIN();
		RULE1(token<CONST>);
		RULE1(token<VOLATILE>);
		RULE1(token<__GLOBAL>);
		RULE1(token<__LOCAL>);
		RULE1(token<__PRIVATE>);
		RULE1(token<__CONSTANT>);
		END();
	}

	int Parser::__type_qualifier_list()
	{
		BEGIN();
		LISTOF_LEFT(type_qualifier);
		END();
	}

	int Parser::__enum_specifier()
	{
		BEGIN();
		RULE4(token<ENUM>, token<'{'>, enumerator_list, token<'}'>);
		RULE5(token<ENUM>, token<IDENTIFIER>, token<'{'>, enumerator_list, token<'}'>);
		RULE2(token<ENUM>, token<IDENTIFIER>);
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
		RULE3(token<IDENTIFIER>, token<'='>, constant_expression);
		RULE1(token<IDENTIFIER>);
		END();
	}

	int Parser::__direct_declarator_suffix()
	{
		BEGIN();
		RULE3(token<'['>, constant_expression, token<']'>);
		RULE2(token<'['>, token<']'>);
		RULE3(token<'('>, parameter_type_list, token<')'>);
		RULE3(token<'('>, identifier_list, token<')'>);
		RULE2(token<'('>, token<')'>);
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
			Node N = d_val__;
			while(__direct_declarator_suffix())
			{
				d_val__.push_front(N);
				N = d_val__;
			}
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
		BEGIN();
		RULE1(parameter_list);
		END();
	}

	int Parser::__parameter_list()
	{
		LISTOF_LEFT_SEP(parameter_declaration, token<','>);
		return 0;
	}

	int Parser::__parameter_declaration()
	{
		BEGIN();
		RULE2(declaration_specifiers, declarator);
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
		RULE3(token<'('>, abstract_declarator, token<')'>);
		RULE2(token<'['>, token<']'>);
		RULE3(token<'['>, constant_expression, token<']'>);
		RULE2(token<'('>, token<')'>);
		RULE3(token<'('>, parameter_type_list, token<')'>);
		END();
	}

	int Parser::__direct_abstract_declarator_suffix()
	{
		BEGIN();
		RULE2(token<'['>, token<']'>);
		RULE3(token<'['>, constant_expression, token<']'>);
		RULE2(token<'('>, token<')'>);
		RULE3(token<'('>, parameter_type_list, token<')'>);
		END();
	}

	int Parser::__direct_abstract_declarator()
	{
		BEGIN();
		if (__direct_abstract_declarator_base())
		{
			Node N = d_val__;
			while(__direct_abstract_declarator_suffix())
			{
				d_val__.push_front(N);
				N = d_val__;
			}
			d_val__ = N;
			return 1;
		}
		END();
	}

	int Parser::__constant_expression()
	{
		BEGIN();
		RULE1(conditional_expression);
		END();
	}

	int Parser::__struct_or_union_specifier()
	{
		BEGIN();
		RULE5(struct_or_union, token<IDENTIFIER>, token<'{'>, struct_declaration_list, token<'}'>);
		RULE4(struct_or_union, token<'{'>, struct_declaration_list, token<'}'>);
		RULE2(struct_or_union, token<IDENTIFIER>);
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
		RULE2(token<'{'>, token<'}'>);
		RULE3(token<'{'>, statement_list, token<'}'>);
		RULE3(token<'{'>, declaration_list, token<'}'>);
		RULE4(token<'{'>, declaration_list, statement_list, token<'}'>);
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
		RULE3(token<IDENTIFIER>, token<':'>, statement);
		RULE4(token<CASE>, constant_expression, token<':'>, statement);
		RULE3(token<DEFAULT>, token<':'>, statement);
		END();
	}

	int Parser::__jump_statement()
	{
		BEGIN();
		RULE3(token<GOTO>, token<IDENTIFIER>, token<';'>);
		RULE2(token<CONTINUE>, token<';'>);
		RULE2(token<BREAK>, token<';'>);
		RULE2(token<RETURN>, token<';'>);
		RULE3(token<RETURN>, expression, token<';'>);
		END();
	}

	int Parser::__iteration_statement()
	{
		BEGIN();
		RULE5(token<WHILE>, token<'('>, expression, token<')'>, statement);
		RULE7(token<DO>, statement, token<WHILE>, token<'('>, expression, token<')'>, token<';'>);
		RULE6(token<FOR>, token<'('>, expression_statement, expression_statement, token<')'>, statement);
		RULE7(token<FOR>, token<'('>, expression_statement, expression_statement, expression, token<')'>, statement);
		RULE6(token<FOR>, token<'('>, declaration, expression_statement, token<')'>, statement);
		RULE7(token<FOR>, token<'('>, declaration, expression_statement, expression, token<')'>, statement);
		END();
	}

	int Parser::__selection_statement()
	{
		BEGIN();
		RULE7(token<IF>, token<'('>, expression, token<')'>, statement, token<ELSE>, statement);
		RULE5(token<IF>, token<'('>, expression, token<')'>, statement);
		RULE5(token<SWITCH>, token<'('>, expression, token<')'>, statement);
		END();
	}

	int Parser::__expression_statement()
	{
		BEGIN();
		RULE1(token<';'>);
		RULE2(expression, token<';'>);
		END();
	}

	int Parser::__expression()
	{
		LISTOF_LEFT_SEP(assignment_expression, token<','>);
		return 0;
	}

	int Parser::__assignment_expression()
	{
		BEGIN();
		RULE3(unary_expression, assignment_operator, assignment_expression);
		RULE1(conditional_expression);
		END();
	}

	int Parser::__assignment_operator()
	{
		BEGIN();
		RULE1(token<'='>);
		RULE1(token<MUL_ASSIGN>);
		RULE1(token<DIV_ASSIGN>);
		RULE1(token<MOD_ASSIGN>);
		RULE1(token<ADD_ASSIGN>);
		RULE1(token<SUB_ASSIGN>);
		RULE1(token<LEFT_ASSIGN>);
		RULE1(token<RIGHT_ASSIGN>);
		RULE1(token<AND_ASSIGN>);
		RULE1(token<XOR_ASSIGN>);
		RULE1(token<OR_ASSIGN>);
		END();
	}

	int Parser::__conditional_expression()
	{
		BEGIN();
		RULE5(logical_or_expression, token<'?'>, expression, token<':'>, conditional_expression);
		RULE1(logical_or_expression);
		END();
	}

	int Parser::__unary_expression()
	{
		BEGIN();
		RULE1(postfix_expression);
		RULE2(token<INC_OP>, unary_expression);
		RULE2(token<DEC_OP>, unary_expression);
		RULE2(unary_operator, cast_expression);
		RULE2(token<SIZEOF>, unary_expression);
		RULE4(token<SIZEOF>, token<'('>, type_name, token<')'>);
		END();
	}

	int Parser::__unary_operator()
	{
		BEGIN();
		RULE1(token<'&'>);
		RULE1(token<'*'>);
		RULE1(token<'+'>);
		RULE1(token<'-'>);
		RULE1(token<'~'>);
		RULE1(token<'!'>);
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
		LISTOF_LEFT_SEP(logical_and_expression, token<OR_OP>);
		return 0;
	}

	int Parser::__logical_and_expression()
	{
		LISTOF_LEFT_SEP(inclusive_or_expression, token<AND_OP>);
		return 0;
	}

	int Parser::__inclusive_or_expression()
	{
		LISTOF_LEFT_SEP(exclusive_or_expression, token<'|'>);
		return 0;
	}

	int Parser::__exclusive_or_expression()
	{
		LISTOF_LEFT_SEP(and_expression, token<'^'>);
		return 0;
	}

	int Parser::__and_expression()
	{
		LISTOF_LEFT_SEP(equality_expression, token<'&'>);
		return 0;
	}

	int Parser::__equality_operator()
	{
		BEGIN();
		RULE1(token<EQ_OP>);
		RULE1(token<NE_OP>);
		END();
	}

	int Parser::__equality_expression()
	{
		BEGIN();
		LISTOF_LEFT_SEP(relational_expression, equality_operator);
		END();
	}

	int Parser::__relational_operator()
	{
		BEGIN();
		RULE1(token<'<'>);
		RULE1(token<'>'>);
		RULE1(token<LE_OP>);
		RULE1(token<GE_OP>);
		END();
	}

	int Parser::__relational_expression()
	{
		BEGIN();
		LISTOF_LEFT_SEP(shift_expression, relational_operator);
		END();
	}

	int Parser::__shift_operator()
	{
		BEGIN();
		RULE1(token<LEFT_OP>);
		RULE1(token<RIGHT_OP>);
		END();
	}

	int Parser::__shift_expression()
	{
		BEGIN();
		LISTOF_LEFT_SEP(additive_expression, shift_operator);
		END();
	}

	int Parser::__additive_operator()
	{
		BEGIN();
		RULE1(token<'+'>);
		RULE1(token<'-'>);
		END();
	}

	int Parser::__additive_expression()
	{
		BEGIN();
		LISTOF_LEFT_SEP(multiplicative_expression, additive_operator);
		END();
	}

	int Parser::__multiplicative_operator()
	{
		BEGIN();
		RULE1(token<'*'>);
		RULE1(token<'/'>);
		RULE1(token<'%'>);
		END();
	}

	int Parser::__multiplicative_expression()
	{
		BEGIN();
		LISTOF_LEFT_SEP(cast_expression, multiplicative_operator);
		END();
	}

	int Parser::__postfix_expression_suffix()
	{
		BEGIN();
		RULE3(token<'['>, expression, token<']'>);
		RULE2(token<'('>, token<')'>);
		RULE3(token<'('>, argument_expression_list, token<')'>);
		RULE2(token<'.'>, token<IDENTIFIER>);
		RULE2(token<PTR_OP>, token<IDENTIFIER>);
		RULE1(token<INC_OP>);
		RULE1(token<DEC_OP>);
		END();
	}

	int Parser::__postfix_expression()
	{
		BEGIN();
		if (__primary_expression())
		{
			Node N = d_val__;
			while (__postfix_expression_suffix())
			{
				if (d_val__.size() == 0)
					N = Node(N, d_val__);
				else
				{
					d_val__.push_front(N);
					N = d_val__;
				}
			}
			d_val__ = N;
			return 1;
		}
		END();
	}

	int Parser::__primary_expression()
	{
		BEGIN();
		RULE1(token<IDENTIFIER>);
		RULE1(token<CONSTANT>);
		RULE1(token<STRING_LITERAL>);
		RULE3(token<'('>, expression, token<')'>);
		END();
	}

	int Parser::__argument_expression_list()
	{
		LISTOF_LEFT_SEP(assignment_expression, token<','>);
		return 0;
	}

	int Parser::__init_declarator()
	{
		BEGIN();
		RULE3(declarator, token<'='>, initializer);
		RULE1(declarator);
		END();
	}

	int Parser::__initializer()
	{
		BEGIN();
		RULE1(assignment_expression);
		RULE3(token<'{'>, initializer_list, token<'}'>);
		RULE4(token<'{'>, initializer_list, token<','>, token<'}'>);
		END();
	}

	int Parser::__initializer_list()
	{
		LISTOF_LEFT_SEP(initializer, token<','>);
		return 0;
	}
}
