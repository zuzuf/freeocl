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
#ifndef __FREEOCL_PARSER_PARSER_H__
#define __FREEOCL_PARSER_PARSER_H__

#include <istream>
#include <ostream>
#include <unordered_map>
#include <deque>
#include <vector>
#include "node.h"
#include "kernel.h"

namespace FreeOCL
{
	class SymbolTable;
	class Parser
	{
	public:
		enum { _EOF = 256 };
		enum TokenID
		{
			IDENTIFIER = 257, CONSTANT, STRING_LITERAL, SIZEOF,
			PTR_OP, INC_OP, DEC_OP, LEFT_OP, RIGHT_OP, LE_OP, GE_OP, EQ_OP, NE_OP,
			AND_OP, OR_OP, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, ADD_ASSIGN,
			SUB_ASSIGN, LEFT_ASSIGN, RIGHT_ASSIGN, AND_ASSIGN,
			XOR_ASSIGN, OR_ASSIGN,
			TYPE_NAME,

			TYPEDEF, BOOL, HALF,
			CHAR, SHORT, INT, LONG, UCHAR, USHORT, UINT, ULONG, FLOAT, DOUBLE,
			CHAR2, SHORT2, INT2, LONG2, UCHAR2, USHORT2, UINT2, ULONG2, FLOAT2, DOUBLE2,
			CHAR3, SHORT3, INT3, LONG3, UCHAR3, USHORT3, UINT3, ULONG3, FLOAT3, DOUBLE3,
			CHAR4, SHORT4, INT4, LONG4, UCHAR4, USHORT4, UINT4, ULONG4, FLOAT4, DOUBLE4,
			CHAR8, SHORT8, INT8, LONG8, UCHAR8, USHORT8, UINT8, ULONG8, FLOAT8, DOUBLE8,
			CHAR16, SHORT16, INT16, LONG16, UCHAR16, USHORT16, UINT16, ULONG16, FLOAT16, DOUBLE16,
			SIGNED, UNSIGNED, SIZE_T, CONST, VOLATILE, VOID,
			STRUCT, UNION, ENUM,

			CASE, DEFAULT, IF, ELSE, SWITCH, WHILE, DO, FOR, GOTO, CONTINUE, BREAK, RETURN,

			__KERNEL, __GLOBAL, __LOCAL, __PRIVATE, __CONSTANT,

			__ATTRIBUTE__,	SPECIAL
		};
	public:
		Parser(std::istream &in, std::ostream &err) : in(in), err(err), line(0), bErrors(false)	{}

		int parse();
		inline bool errors() const {	return bErrors; }
		inline const smartptr<Node> &getAST() const	{	return root;	}
		const std::unordered_map<std::string, smartptr<Kernel> >	&getKernels() const	{	return kernels;	}

	private:
		void error(const std::string &msg);	// called on (syntax) errors
		void warning(const std::string &msg);	// called on warnings
		int lex();							// returns the next token from the

		// support functions for parse()
		inline int readToken();
		inline int peekToken();
		inline void rollBack();
		inline void rollBackTo(size_t size);

		// support functions for lex()
		int get();
		std::istream &get(char &c);
		void putback(char c);
		int peek();

		void register_builtin();

	private:
		std::istream &in;			// Input stream
		std::ostream &err;			// Error output stream
		size_t line;
		std::string current_line;
		std::string current_file;
		bool bErrors;
		smartptr<Node> root;

		smartptr<Node> d_val__;

		std::unordered_map<std::string, smartptr<Kernel> >	kernels;
		std::vector<std::pair<int, smartptr<Node> > > tokens;
		std::vector<std::pair<int, smartptr<Node> > > processed;
		SymbolTable *symbols;

	private:
		// Parser grammar rules
		int __translation_unit();
		int __external_declaration();
		int __function_definition();
		int __declaration();
		int __declaration_specifiers();
		int __declaration_specifier();
		int __init_declarator_list();
		int __declarator();
		int __declaration_list();
		int __pointer();
		int __function_qualifier();
		int __storage_class_specifier();
		int __type_specifier();
		int __type_qualifier();
		int __type_qualifier_list();
		int __enum_specifier();
		int __enumerator_list();
		int __enumerator();
		int __direct_declarator();
		int __direct_declarator_suffix();
		int __direct_declarator_base();
		int __identifier_list();
		int __parameter_type_list();
		int __parameter_list();
		int __parameter_declaration();
		int __abstract_declarator();
		int __direct_abstract_declarator();
		int __direct_abstract_declarator_base();
		int __direct_abstract_declarator_suffix();
		int __constant_expression();
		int __struct_or_union_specifier();
		int __struct_or_union();
		int __struct_declaration_list();
		int __struct_declaration();
		int __struct_declarator_list();
		int __struct_declarator();
		int __specifier_qualifier_list();
		int __compound_statement();
		int __statement_list();
		int __statement();
		int __labeled_statement();
		int __jump_statement();
		int __iteration_statement();
		int __selection_statement();
		int __expression_statement();
		int __expression();
		int __assignment_expression();
		int __assignment_operator();
		int __conditional_expression();
		int __unary_expression();
		int __unary_operator();
		int __type_name();
		int __cast_expression();
		int __logical_or_expression();
		int __logical_and_expression();
		int __inclusive_or_expression();
		int __exclusive_or_expression();
		int __and_expression();
		int __equality_expression();
		int __equality_operator();
		int __relational_expression();
		int __relational_operator();
		int __shift_expression();
		int __shift_operator();
		int __additive_expression();
		int __additive_operator();
		int __multiplicative_expression();
		int __multiplicative_operator();
		int __postfix_expression();
		int __postfix_expression_suffix();
		int __primary_expression();
		int __argument_expression_list();
		int __init_declarator();
		int __initializer();
		int __initializer_list();
		int __specifier_qualifier();
		int __declaration_statement_list();
		int __declaration_statement();
		int __identifier();
		int __attribute_qualifier();
		int __attribute_list();
		int __attributeopt();
		int __attribute_token();
		int __attribute_argument_clauseopt();
		int __attribute_argument_list();
		int __attribute_argument();

		template<int token>
		inline int __token()
		{
			const int next = readToken();
			if (token == next)
				return 1;
			rollBack();
			return 0;
		}
	};
}

#endif
