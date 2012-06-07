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
#ifndef __FREEOCL_PREPROCESSOR_PREPROCESSOR_H__
#define __FREEOCL_PREPROCESSOR_PREPROCESSOR_H__

#include <istream>
#include <ostream>
#include <vector>
#include <utils/map.h>
#include <utils/set.h>

namespace FreeOCL
{
	class preprocessor
	{
		enum token_id
		{
			IDENTIFIER, CONSTANT, DEFINED,
			LEFT_OP, RIGHT_OP,
			LE_OP, GE_OP, EQ_OP, NE_OP, L_OP, G_OP,
			ADD_OP, SUB_OP, MUL_OP, DIV_OP, MOD_OP,
			AND_OP, OR_OP, NOT_OP, XOR_OP,
			LAND_OP, LOR_OP, LNOT_OP,
			LP, RP, TER_0, TER_1
		};
	private:
		struct macro
		{
			std::vector<std::string>	params;
			std::string value;
		};

	public:
		preprocessor(std::ostream &out, std::ostream &err);

		void parse(std::istream &in, const std::string &filename = "<input>");
		inline bool errors() const {	return b_errors; }

		void define(const std::string &name, const std::string &value);
		void define(const std::string &name, const std::vector<std::string> &params, const std::string &value);

		void set_include_paths(const std::vector<std::string> &include_paths);
		void set_headers(const map<std::string, std::string> &headers);

	private:
		void error(const std::string &msg);	// called on (syntax) errors
		void warning(const std::string &msg);	// called on warnings

		std::string macro_expansion(std::string text, const set<std::string> &forbidden = set<std::string>(), const bool b_defined_rule = false);
		std::string parameters_substitution(const std::string &s,
											const std::vector<std::string> &names,
											const std::vector<std::string> &values);

		// support functions for parse()
		int get();
		int peek();
		void putback(char c);

		std::string get_line();
		std::string get_word();
		std::string get_path();
		void skip_whitespaces();
		std::string skip_bloc();

		void tokenize(const std::string &s);
		int eval(const std::string &exp);
		bool eval_expression(int *v);
		bool eval_conditional_expression(int *v);
		bool eval_logical_or_expression(int *v);
		bool eval_logical_and_expression(int *v);
		bool eval_or_expression(int *v);
		bool eval_xor_expression(int *v);
		bool eval_and_expression(int *v);
		bool eval_equality_expression(int *v);
		bool eval_relational_expression(int *v);
		bool eval_shift_expression(int *v);
		bool eval_additive_expression(int *v);
		bool eval_multiplicative_expression(int *v);
		bool eval_unary_expression(int *v);
		bool eval_primary_expression(int *v);
		bool eval_token(int v);
		bool eval_identifier(int *id);
		bool eval_constant(int *v);

	private:
		std::vector<std::istream*> in;	// Input streams
		std::ostream &out;
		std::ostream &err;			// Error output stream
		size_t line;
		std::string current_line;
		std::string current_file;
		bool b_errors;

		map<std::string, macro>	macros;
		std::vector<std::pair<int, int> > tokens;
		std::vector<std::string> identifiers;
		size_t token_id;

		std::vector<std::string> include_paths;
		map<std::string, std::string>	headers;

	public:
		static std::string fix_end_of_lines(const std::string &s);
	};
}

#endif
