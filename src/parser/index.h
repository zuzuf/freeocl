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
#ifndef __FREEOCL_PARSER_INDEX_H__
#define __FREEOCL_PARSER_INDEX_H__

#include "expression.h"

namespace FreeOCL
{
	class index : public expression
	{
	public:
		index(const smartptr<expression> &ptr, const smartptr<expression> &idx);
		virtual ~index();

		virtual smartptr<type> get_type() const;
		virtual void write(std::ostream& out) const;

		virtual uint32_t eval_as_uint() const;

		virtual bool has_references_to(const std::string &function_name) const;

        virtual const char *get_node_type() const;

        void enable_boundary_check(bool b_check_bounds);
		void set_ref_string(const std::string &ref);
	private:
		const smartptr<expression> ptr;
		const smartptr<expression> idx;
		bool b_check_bounds;
		std::string ref;
	};
}

#endif
