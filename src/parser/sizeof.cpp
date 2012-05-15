#include "sizeof.h"
#include "native_type.h"

namespace FreeOCL
{
	size_of::size_of(const smartptr<node> &n)
		: n(n)
	{

	}

	size_of::~size_of()
	{

	}

	void size_of::write(std::ostream &out) const
	{
		out << "sizeof(" << *n << ')';
	}

	smartptr<type> size_of::get_type() const
	{
		return native_type::t_size_t;
	}

	uint32_t size_of::eval_as_uint() const
	{
		throw "static evaluation of sizeof operator not implemented!";
		return 0;
	}

	bool size_of::has_references_to(const std::string &function_name) const
	{
		return false;
	}
}
