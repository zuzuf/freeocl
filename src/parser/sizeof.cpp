#include "sizeof.h"
#include "native_type.h"
#include <stdexcept>

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
        if (n.as<type>())
            return n.as<type>()->size();
        return n.as<expression>()->get_type()->size();
	}

	bool size_of::has_references_to(const std::string &function_name) const
	{
		return false;
	}

    const char *size_of::get_node_type() const
    {
        return "size_of";
    }
}
