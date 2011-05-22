#include "builtin.h"
#include "parser.h"
#include "native_type.h"
#include "symbol_table.h"
#include "var.h"

namespace FreeOCL
{
	Builtin::Builtin(const smartptr<Type> &return_type, const std::string &name, const size_t num_params)
		: return_type(return_type),
		name(name),
		num_params(num_params)
	{
	}

	Builtin::~Builtin()
	{
	}

	smartptr<Type> Builtin::getReturnType(const std::deque<smartptr<Type> > &/*arg_types*/) const
	{
		return return_type;
	}

	const std::string &Builtin::getName() const
	{
		return name;
	}

	size_t Builtin::getNumParams() const
	{
		return num_params;
	}

	void Builtin::write(std::ostream& out) const
	{
		out << name << ' ';
	}

	void Parser::register_builtin()
	{
#define REGISTER(type, name, num)		symbols->insert(#name, new Builtin(NativeType::t_##type, #name, num))
#define REGISTER_VAR(type, name)		symbols->insert(#name, new Var(#name, NativeType::t_##type))

		// Workitem functions
		REGISTER(uint, get_work_dim, 0);
		REGISTER(size_t, get_global_size, 1);
		REGISTER(size_t, get_global_id, 1);
		REGISTER(size_t, get_local_size, 1);
		REGISTER(size_t, get_local_id, 1);
		REGISTER(size_t, get_num_groups, 1);
		REGISTER(size_t, get_group_id, 1);
		REGISTER(size_t, get_global_offset, 1);

		// Sync functions
		REGISTER_VAR(int, CLK_LOCAL_MEM_FENCE);
		REGISTER_VAR(int, CLK_GLOBAL_MEM_FENCE);
		REGISTER(void, barrier, 1);

		// Memory fence function
		REGISTER(void, mem_fence, 1);
		REGISTER(void, read_mem_fence, 1);
		REGISTER(void, write_mem_fence, 1);
#undef REGISTER
#undef REGISTER_VAR
	}
}
