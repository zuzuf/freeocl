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
#ifndef __FREEOCL_PARSER_POINTER_TYPE_H__
#define __FREEOCL_PARSER_POINTER_TYPE_H__

#include "type.h"

namespace FreeOCL
{
	class pointer_type : public type
	{
	public:
		pointer_type(const smartptr<type> &base_type, const bool b_const, const address_space addr_space) : type(b_const, addr_space), base_type(base_type)	{}
		virtual bool operator==(const type &p_type) const;
		virtual void write(std::ostream &out) const;
		virtual std::string get_name() const;
		virtual std::string get_cxx_name() const;

		const smartptr<type> &get_base_type() const	{	return base_type;	}
		bool is_compatible_with(const pointer_type &p_type) const;

		virtual smartptr<type> clone(const bool b_const, const address_space addr_space) const;
		smartptr<type> clone() const;

		void set_root_type(const smartptr<type> &root_type);
	protected:
		smartptr<type> base_type;

	public:
		static smartptr<type> t_p_const_char;
	};
}

#endif
