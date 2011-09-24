#include "declarator.h"
#include "pointer_type.h"

namespace FreeOCL
{
	declarator::declarator(const smartptr<type> &p_type, const smartptr<node> &n0, const smartptr<node> &n1)
		: p_type(p_type),
		  n0(n0),
		  n1(n1)
	{

	}

	declarator::~declarator()
	{

	}

	void declarator::write(std::ostream &out) const
	{
		if (p_type->get_address_space() == type::LOCAL)
		{
			if (!p_type.as<pointer_type>())
			{
				out << "static ";
			}
		}
		out << *p_type << *n0 << *n1;
	}
}
