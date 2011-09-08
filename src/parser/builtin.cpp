#include "builtin.h"
#include "parser.h"
#include "native_type.h"
#include "symbol_table.h"
#include "overloaded_builtin.h"
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
		const int types[] = { NativeType::UCHAR,	NativeType::UCHAR2,		NativeType::UCHAR3,		NativeType::UCHAR4,		NativeType::UCHAR8,		NativeType::UCHAR16,
							  NativeType::USHORT,	NativeType::USHORT2,	NativeType::USHORT3,	NativeType::USHORT4,	NativeType::USHORT8,	NativeType::USHORT16,
							  NativeType::UINT,		NativeType::UINT2,		NativeType::UINT3,		NativeType::UINT4,		NativeType::UINT8,		NativeType::UINT16,
							  NativeType::ULONG,	NativeType::ULONG2,		NativeType::ULONG3,		NativeType::ULONG4,		NativeType::ULONG8,		NativeType::ULONG16,
							  NativeType::CHAR,		NativeType::CHAR2,		NativeType::CHAR3,		NativeType::CHAR4,		NativeType::CHAR8,		NativeType::CHAR16,
							  NativeType::SHORT,	NativeType::SHORT2,		NativeType::SHORT3,		NativeType::SHORT4,		NativeType::SHORT8,		NativeType::SHORT16,
							  NativeType::INT,		NativeType::INT2,		NativeType::INT3,		NativeType::INT4,		NativeType::INT8,		NativeType::INT16,
							  NativeType::LONG,		NativeType::LONG2,		NativeType::LONG3,		NativeType::LONG4,		NativeType::LONG8,		NativeType::LONG16,
							  NativeType::FLOAT,	NativeType::FLOAT2,		NativeType::FLOAT3,		NativeType::FLOAT4,		NativeType::FLOAT8,		NativeType::FLOAT16,
							  NativeType::DOUBLE,	NativeType::DOUBLE2,	NativeType::DOUBLE3,	NativeType::DOUBLE4,	NativeType::DOUBLE8,	NativeType::DOUBLE16 };
		std::deque<int> gentype_all;
		std::deque<int> gentype_scalars;
		std::deque<int> gentype_vectors;
		std::deque<int> gentype_floats;
		gentype_all.insert(gentype_all.end(), &(types[0]), &(types[sizeof(types)/sizeof(int)]));
		gentype_floats.insert(gentype_floats.end(), &(types[6 * 8]), &(types[6 * 9]));
		for(size_t i = 0 ; i < 10 ; ++i)
		{
			gentype_scalars.push_back(types[i * 6]);
			gentype_vectors.insert(gentype_vectors.end(), &(types[i * 6 + 1]), &(types[i * 6 + 6]));
		}

#define REGISTER(type, name, num)					symbols->insert(#name, new Builtin(NativeType::t_##type, #name, num))
#define REGISTER_OVERLOADED(signature, gentype)		do { OverloadedBuiltin *n = new OverloadedBuiltin(signature, gentype);	symbols->insert(n->getName(), n);	} while(false)
#define REGISTER_VAR(type, name)					symbols->insert(#name, new Var(#name, NativeType::t_##type))

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

		// Relational functions
		REGISTER_OVERLOADED("intn isequal(gentype,gentype)", gentype_all);
		REGISTER_OVERLOADED("float dot(floatn,floatn)", gentype_floats);
#undef REGISTER
#undef REGISTER_OVERLOADED
#undef REGISTER_VAR
	}
}
