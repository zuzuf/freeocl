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
#ifndef __FREEOCL_PARSER_OVERLOADED_BUILTIN_H__
#define __FREEOCL_PARSER_OVERLOADED_BUILTIN_H__

#include "callable.h"

namespace FreeOCL
{
	class overloaded_builtin : public callable
	{
        friend class function;
	public:
		overloaded_builtin(const std::string &signature, const std::deque<int> &gentype);
		virtual ~overloaded_builtin();

		virtual smartptr<type> get_return_type(const std::deque<smartptr<type> > &arg_types) const;
		virtual const std::string &get_name() const;
		virtual size_t get_num_params() const;
        virtual std::deque<smartptr<type> > get_arg_types(const std::deque<smartptr<type> > &param_types) const;

		virtual void write(std::ostream& out) const;

		void print_debug_info() const;

		void merge(const smartptr<overloaded_builtin> rhs);

		virtual bool has_references_to(const std::string &function_name) const;

        virtual const char *get_node_type() const;
    private:
		void remove_duplicates();

	private:
		static bool all_types_match(const std::deque<smartptr<type> > &args, const std::deque<smartptr<type> > &sig);
		static bool all_types_weak_match(const std::deque<smartptr<type> > &args, const std::deque<smartptr<type> > &sig);
		static bool weak_match(const smartptr<type> &a, const smartptr<type> &b);
		static bool match(const smartptr<type> &a, const smartptr<type> &b);
	private:
		std::deque<std::deque<smartptr<type> > > possible_types;
		std::string name;
		size_t num_params;
	};
}

#endif
