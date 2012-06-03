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
#include "preprocessor.h"

#define BEGIN()\
		if (token_id >= tokens.size())\
			return false;\
		size_t _token_id = token_id
#define MATCH(R)\
		if (!(R))\
			token_id = _token_id;\
		else
#define RULE(R)\
		MATCH(R)\
			return true
#define END()	return false

namespace FreeOCL
{
	int preprocessor::eval(const std::string &exp)
	{
		tokenize(exp);
		token_id = 0;
		int v;
		if (!eval_expression(&v))
			error("preprocessor: expression expected");
		if (token_id < tokens.size())
			error("unexpected extra tokens in preprocessor expression");
		return v;
	}

	bool preprocessor::eval_expression(int *v)
	{
		BEGIN();
		RULE(eval_conditional_expression(v));
		END();
	}

	bool preprocessor::eval_conditional_expression(int *v)
	{
		BEGIN();
		int v0, v1, v2;
		MATCH(eval_logical_or_expression(&v0) && eval_token(TER_0) && eval_expression(&v1) && eval_token(TER_1) && eval_conditional_expression(&v2))
		{
			*v = v0 ? v1 : v2;
			return true;
		}

		RULE(eval_logical_or_expression(v));
		END();
	}

	bool preprocessor::eval_logical_or_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_logical_and_expression(&v0) && eval_token(LOR_OP) && eval_logical_or_expression(&v1))
		{
			*v = v0 || v1;
			return true;
		}
		RULE (eval_logical_and_expression(v));
		END();
	}

	bool preprocessor::eval_logical_and_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_or_expression(&v0) && eval_token(LAND_OP) && eval_logical_and_expression(&v1))
		{
			*v = v0 && v1;
			return true;
		}
		RULE (eval_or_expression(v));
		END();
	}

	bool preprocessor::eval_or_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_xor_expression(&v0) && eval_token(OR_OP) && eval_or_expression(&v1))
		{
			*v = v0 | v1;
			return true;
		}
		RULE (eval_xor_expression(v));
		END();
	}

	bool preprocessor::eval_xor_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_and_expression(&v0) && eval_token(XOR_OP) && eval_xor_expression(&v1))
		{
			*v = v0 ^ v1;
			return true;
		}
		RULE (eval_and_expression(v));
		END();
	}

	bool preprocessor::eval_and_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_equality_expression(&v0) && eval_token(AND_OP) && eval_and_expression(&v1))
		{
			*v = v0 & v1;
			return true;
		}
		RULE (eval_equality_expression(v));
		END();
	}

	bool preprocessor::eval_equality_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_relational_expression(&v0) && eval_token(EQ_OP) && eval_equality_expression(&v1))
		{
			*v = (v0 == v1);
			return true;
		}
		MATCH(eval_relational_expression(&v0) && eval_token(NE_OP) && eval_equality_expression(&v1))
		{
			*v = (v0 != v1);
			return true;
		}
		RULE (eval_relational_expression(v));
		END();
	}

	bool preprocessor::eval_relational_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_shift_expression(&v0) && eval_token(L_OP) && eval_relational_expression(&v1))
		{
			*v = (v0 < v1);
			return true;
		}
		MATCH(eval_shift_expression(&v0) && eval_token(G_OP) && eval_relational_expression(&v1))
		{
			*v = (v0 > v1);
			return true;
		}
		MATCH(eval_shift_expression(&v0) && eval_token(LE_OP) && eval_relational_expression(&v1))
		{
			*v = (v0 <= v1);
			return true;
		}
		MATCH(eval_shift_expression(&v0) && eval_token(GE_OP) && eval_relational_expression(&v1))
		{
			*v = (v0 >= v1);
			return true;
		}
		RULE (eval_shift_expression(v));
		END();
	}

	bool preprocessor::eval_shift_expression(int *v)
	{
		BEGIN();
		int v0, v1;
		MATCH(eval_additive_expression(&v0) && eval_token(LEFT_OP) && eval_shift_expression(&v1))
		{
			*v = (v0 << v1);
			return true;
		}
		MATCH(eval_additive_expression(&v0) && eval_token(RIGHT_OP) && eval_shift_expression(&v1))
		{
			*v = (v0 >> v1);
			return true;
		}
		RULE (eval_additive_expression(v));
		END();
	}

	bool preprocessor::eval_additive_expression(int *v)
	{
		BEGIN();
		int v0;
		MATCH(eval_multiplicative_expression(v))
		{
			do
			{
				_token_id = token_id;
				MATCH (eval_token(ADD_OP) && eval_multiplicative_expression(&v0))
				{
					*v += v0;
					continue;
				}
				MATCH (eval_token(SUB_OP) && eval_multiplicative_expression(&v0))
				{
					*v -= v0;
					continue;
				}
				break;
			} while(true);
			return true;
		}
		END();
	}

	bool preprocessor::eval_multiplicative_expression(int *v)
	{
		BEGIN();
		int v0;
		MATCH(eval_unary_expression(v))
		{
			do
			{
				_token_id = token_id;
				MATCH (eval_token(MUL_OP) && eval_unary_expression(&v0))
				{
					*v *= v0;
					continue;
				}
				MATCH (eval_token(DIV_OP) && eval_unary_expression(&v0))
				{
					*v /= v0;
					continue;
				}
				MATCH (eval_token(MOD_OP) && eval_unary_expression(&v0))
				{
					*v %= v0;
					continue;
				}
				break;
			} while(true);
			return true;
		}
		END();
	}

	bool preprocessor::eval_unary_expression(int *v)
	{
		BEGIN();
		RULE(eval_primary_expression(v));
		MATCH (eval_token(SUB_OP) && eval_primary_expression(v))
		{
			*v = -*v;
			return true;
		}
		MATCH (eval_token(NOT_OP) && eval_primary_expression(v))
		{
			*v = ~*v;
			return true;
		}
		MATCH (eval_token(LNOT_OP) && eval_primary_expression(v))
		{
			*v = !*v;
			return true;
		}

		END();
	}

	bool preprocessor::eval_primary_expression(int *v)
	{
		BEGIN();
		RULE (eval_token(LP) && eval_expression(v) && eval_token(RP));
		MATCH (eval_token(DEFINED) && eval_identifier(v))
		{
			*v = macros.count(identifiers[*v]) > 0 ? 1 : 0;
			return true;
		}
		RULE (eval_constant(v));
		END();
	}

	bool preprocessor::eval_token(int v)
	{
		if (token_id >= tokens.size())
			return false;
		if (tokens[token_id].second == v)
		{
			++token_id;
			return true;
		}
		return false;
	}

	bool preprocessor::eval_identifier(int *id)
	{
		if (token_id >= tokens.size())
			return false;
		if (tokens[token_id].second == IDENTIFIER)
		{
			*id = tokens[token_id].first;
			++token_id;
			return true;
		}
		return false;
	}

	bool preprocessor::eval_constant(int *v)
	{
		if (token_id >= tokens.size())
			return false;
		if (tokens[token_id].second == CONSTANT)
		{
			*v = tokens[token_id].first;
			++token_id;
			return true;
		}
		return false;
	}
}
