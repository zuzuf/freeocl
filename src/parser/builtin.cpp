#include "builtin.h"
#include "parser.h"
#include "native_type.h"
#include "symbol_table.h"
#include "overloaded_builtin.h"
#include "var.h"
#include "printf.h"

namespace FreeOCL
{
	namespace
	{
		inline std::deque<int> &operator<<(std::deque<int> &container, FreeOCL::native_type::type_id i)
		{
			container.push_back((int)i);
			return container;
		}
	}

	builtin::builtin(const smartptr<type> &return_type, const std::string &name, const size_t num_params)
		: return_type(return_type),
		name(name),
		num_params(num_params)
	{
	}

	builtin::~builtin()
	{
	}

	smartptr<type> builtin::get_return_type(const std::deque<smartptr<type> > &/*arg_types*/) const
	{
		return return_type;
	}

	const std::string &builtin::get_name() const
	{
		return name;
	}

	size_t builtin::get_num_params() const
	{
		return num_params;
	}

	void builtin::write(std::ostream& out) const
	{
		out << name << ' ';
	}

	void parser::register_builtin()
	{
		static bool b_init = true;
		static symbol_table syms;

		if (!b_init)
		{
			*symbols = syms;
			return;
		}

		const int types[] = { native_type::UCHAR,	native_type::UCHAR2,	native_type::UCHAR3,	native_type::UCHAR4,	native_type::UCHAR8,	native_type::UCHAR16,
							  native_type::USHORT,	native_type::USHORT2,	native_type::USHORT3,	native_type::USHORT4,	native_type::USHORT8,	native_type::USHORT16,
							  native_type::UINT,	native_type::UINT2,		native_type::UINT3,		native_type::UINT4,		native_type::UINT8,		native_type::UINT16,
							  native_type::ULONG,	native_type::ULONG2,	native_type::ULONG3,	native_type::ULONG4,	native_type::ULONG8,	native_type::ULONG16,
							  native_type::CHAR,	native_type::CHAR2,		native_type::CHAR3,		native_type::CHAR4,		native_type::CHAR8,		native_type::CHAR16,
							  native_type::SHORT,	native_type::SHORT2,	native_type::SHORT3,	native_type::SHORT4,	native_type::SHORT8,	native_type::SHORT16,
							  native_type::INT,		native_type::INT2,		native_type::INT3,		native_type::INT4,		native_type::INT8,		native_type::INT16,
							  native_type::LONG,	native_type::LONG2,		native_type::LONG3,		native_type::LONG4,		native_type::LONG8,		native_type::LONG16,
							  native_type::FLOAT,	native_type::FLOAT2,	native_type::FLOAT3,	native_type::FLOAT4,	native_type::FLOAT8,	native_type::FLOAT16,
							  native_type::DOUBLE,	native_type::DOUBLE2,	native_type::DOUBLE3,	native_type::DOUBLE4,	native_type::DOUBLE8,	native_type::DOUBLE16 };
		std::deque<int> gentype_all;
		std::deque<int> gentype_scalars;
		std::deque<int> gentype_vectors;
		std::deque<int> gentype_half;
		std::deque<int> gentype_floats;
		std::deque<int> gentype_floats14;
		std::deque<int> gentype_doubles;
		std::deque<int> gentype_doubles14;
		std::deque<int> gentype_double_vectors;
		std::deque<int> gentype_signed;
		std::deque<int> gentype_unsigned;
		std::deque<int> gentype_integers;
		std::deque<int> gentype_single;
		std::deque<int> gentype_vec1;
		std::deque<int> gentype_vec2;
		std::deque<int> gentype_vec3;
		std::deque<int> gentype_vec4;
		std::deque<int> gentype_vec8;
		std::deque<int> gentype_vec16;
		std::deque<int> gentype_size1;
		std::deque<int> gentype_size2;
		std::deque<int> gentype_size3;
		std::deque<int> gentype_size4;
		std::deque<int> gentype_size6;
		std::deque<int> gentype_size8;
		std::deque<int> gentype_size12;
		std::deque<int> gentype_size16;
		std::deque<int> gentype_size24;
		std::deque<int> gentype_size32;
		std::deque<int> gentype_size64;
		std::deque<int> gentype_size128;
		gentype_size1 << native_type::CHAR << native_type::UCHAR;
		gentype_size2 << native_type::CHAR2 << native_type::UCHAR2
					  << native_type::SHORT << native_type::USHORT;
		gentype_size3 << native_type::CHAR3 << native_type::UCHAR3;
		gentype_size4 << native_type::CHAR4 << native_type::UCHAR4
					  << native_type::SHORT2 << native_type::USHORT2
					  << native_type::INT << native_type::UINT
					  << native_type::FLOAT;
		gentype_size6 << native_type::SHORT3 << native_type::USHORT3;
		gentype_size8 << native_type::CHAR8 << native_type::UCHAR8
					  << native_type::SHORT4 << native_type::USHORT4
					  << native_type::INT2 << native_type::UINT2
					  << native_type::LONG << native_type::ULONG
					  << native_type::FLOAT2 << native_type::DOUBLE;
		gentype_size12 << native_type::INT3 << native_type::UINT3 << native_type::FLOAT3;
		gentype_size16 << native_type::CHAR16 << native_type::UCHAR16
					   << native_type::SHORT8 << native_type::USHORT8
					   << native_type::INT4 << native_type::UINT4
					   << native_type::LONG2 << native_type::ULONG2
					   << native_type::FLOAT4 << native_type::DOUBLE2;
		gentype_size24 << native_type::LONG3 << native_type::ULONG3 << native_type::DOUBLE3;
		gentype_size32 << native_type::SHORT16 << native_type::USHORT16
					   << native_type::INT8 << native_type::UINT8
					   << native_type::LONG4 << native_type::ULONG4
					   << native_type::FLOAT8 << native_type::DOUBLE4;
		gentype_size64 << native_type::INT16 << native_type::UINT16
					   << native_type::LONG8 << native_type::ULONG8
					   << native_type::FLOAT16 << native_type::DOUBLE8;
		gentype_size128 << native_type::LONG16 << native_type::ULONG16
						<< native_type::DOUBLE16;
		gentype_all.insert(gentype_all.end(), &(types[0]), &(types[sizeof(types)/sizeof(int)]));
		gentype_floats.insert(gentype_floats.end(), &(types[6 * 8]), &(types[6 * 9]));
		gentype_floats14.insert(gentype_floats14.end(), &(types[6 * 8]), &(types[6 * 8 + 4]));
		gentype_doubles.insert(gentype_doubles.end(), &(types[6 * 9]), &(types[6 * 10]));
		gentype_doubles14.insert(gentype_doubles14.end(), &(types[6 * 9]), &(types[6 * 9 + 4]));
		gentype_double_vectors.insert(gentype_double_vectors.end(), &(types[6 * 9 + 1]), &(types[6 * 10]));
		gentype_signed.insert(gentype_signed.end(), &(types[6 * 4]), &(types[6 * 8]));
		gentype_unsigned.insert(gentype_unsigned.end(), &(types[0]), &(types[6 * 4]));
		for(size_t i = 0 ; i < 10 ; ++i)
		{
			gentype_scalars.push_back(types[i * 6]);
			gentype_vectors.insert(gentype_vectors.end(), &(types[i * 6 + 1]), &(types[i * 6 + 6]));
			gentype_vec1.push_back(types[i * 6]);
			gentype_vec2.push_back(types[i * 6 + 1]);
			gentype_vec3.push_back(types[i * 6 + 2]);
			gentype_vec4.push_back(types[i * 6 + 3]);
			gentype_vec8.push_back(types[i * 6 + 4]);
			gentype_vec16.push_back(types[i * 6 + 5]);
		}
		gentype_single.push_back(0);
		gentype_integers.insert(gentype_integers.end(), gentype_signed.begin(), gentype_signed.end());
		gentype_integers.insert(gentype_integers.end(), gentype_unsigned.begin(), gentype_unsigned.end());
		gentype_half.push_back(native_type::HALF);
		gentype_half.push_back(native_type::HALF2);
		gentype_half.push_back(native_type::HALF3);
		gentype_half.push_back(native_type::HALF4);
		gentype_half.push_back(native_type::HALF8);
		gentype_half.push_back(native_type::HALF16);

#define REGISTER(type, name, num)					symbols->insert(#name, new builtin(native_type::t_##type, #name, num))
#define REGISTER_OVERLOADED(signature, gentype)		do { overloaded_builtin *n = new overloaded_builtin(signature, gentype);	symbols->insert(n->get_name(), n);	} while(false)
#define REGISTER_OVERLOADED_REDIRECT(cl_name, signature, gentype)		do { overloaded_builtin *n = new overloaded_builtin(signature, gentype);	symbols->insert(#cl_name, n);	} while(false)
#define REGISTER_VAR(type, name)					symbols->insert(#name, new var(#name, native_type::t_##type))

		// Printf
		symbols->insert("printf", new printf);

		// Workitem functions
		REGISTER(uint, get_work_dim, 0);
		REGISTER(size_t, get_global_size, 1);
		REGISTER(size_t, get_global_id, 1);
		REGISTER(size_t, get_local_size, 1);
		REGISTER(size_t, get_local_id, 1);
		REGISTER(size_t, get_num_groups, 1);
		REGISTER(size_t, get_group_id, 1);
		REGISTER(size_t, get_global_offset, 1);

		// Math functions
		REGISTER_OVERLOADED("gentype acos(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype acosh(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype acospi(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype asin(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype asinh(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype asinpi(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype atan(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype atan2(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype atanh(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype atanpi(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype atan2pi(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype cbrt(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype ceil(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype copysign(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype cos(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype cosh(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype cospi(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype erf(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype erfc(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype exp(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype exp2(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype exp10(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype expm1(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype fabs(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype fdim(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype floor(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype fma(gentype,gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype fmax(gentype,gentype)|gentype fmax(gentype,float)", gentype_floats);
		REGISTER_OVERLOADED("gentype fmin(gentype,gentype)|gentype fmin(gentype,float)", gentype_floats);
		REGISTER_OVERLOADED("gentype fmod(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype fract(gentype,__global gentype*)|gentype fract(gentype,__local gentype*)|gentype fract(gentype,__private gentype*)", gentype_floats);
		REGISTER_OVERLOADED("gentype frexp(gentype,__global intn*)|gentype frexp(gentype,__local intn*)|gentype frexp(gentype,__private intn*)", gentype_floats);
		REGISTER_OVERLOADED("gentype hypot(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("intn ilogb(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype ldexp(gentype,intn)|gentype ldexp(gentype,int)", gentype_floats);
		REGISTER_OVERLOADED("gentype lgamma(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype lgamma_r(gentype,__global intn*)|gentype lgamma_r(gentype,__local intn*)|gentype lgamma_r(gentype,__private intn*)", gentype_floats);
		REGISTER_OVERLOADED("gentype log(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype log2(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype log10(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype log1p(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype logb(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype mad(gentype,gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype maxmag(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype minmag(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype modf(gentype,__global gentype*)|gentype modf(gentype,__local gentype*)|gentype modf(gentype,__private gentype*)", gentype_floats);
		REGISTER_OVERLOADED("floatn nan(uintn)", gentype_floats);
		REGISTER_OVERLOADED("gentype nextafter(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype pow(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype pown(gentype,intn)", gentype_floats);
		REGISTER_OVERLOADED("gentype powr(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype remainder(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype remquo(gentype,gentype,__global intn*)|gentype remquo(gentype,gentype,__local intn*)|gentype remquo(gentype,gentype,__private intn*)", gentype_floats);
		REGISTER_OVERLOADED("gentype rint(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype rootn(gentype,intn)", gentype_floats);
		REGISTER_OVERLOADED("gentype round(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype rsqrt(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype sin(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype sincos(gentype,__global gentype*)|gentype sincos(gentype,__local gentype*)|gentype sincos(gentype,__private gentype*)", gentype_floats);
		REGISTER_OVERLOADED("gentype sinh(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype sinpi(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype sqrt(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype tan(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype tanh(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype tanpi(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype tgamma(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype trunc(gentype)", gentype_floats);
		// half_ versions
		REGISTER_OVERLOADED("gentype half_cos(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_divide(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_exp(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_exp2(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_exp10(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_log(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_log2(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_log10(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_powr(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_recip(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_rsqrt(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_sin(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_sqrt(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype half_tan(gentype)", gentype_floats);
		// native_ versions
		REGISTER_OVERLOADED("gentype native_cos(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_divide(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_exp(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_exp2(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_exp10(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_log(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_log2(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_log10(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_powr(gentype,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_recip(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_rsqrt(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_sin(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_sqrt(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype native_tan(gentype)", gentype_floats);

		// Integer functions
		REGISTER_OVERLOADED("ugentype abs(gentype)", gentype_integers);
		REGISTER_OVERLOADED("ugentype abs_diff(gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype add_sat(gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype hadd(gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype rhadd(gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype clamp(gentype,gentype,gentype)|gentype clamp(gentype,sgentype,sgentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype clz(gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype mad_hi(gentype,gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype mad_sat(gentype,gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype max(gentype,gentype)|gentype max(gentype,sgentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype min(gentype,gentype)|gentype min(gentype,sgentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype mul_hi(gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype rotate(gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype sub_sat(gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("short upsample(char,uchar)"
							"|ushort upsample(uchar,uchar)"
							"|shortn upsample(charn,ucharn)"
							"|ushortn upsample(ucharn,ucharn)"
							"|int upsample(short,ushort)"
							"|uint upsample(ushort,ushort)"
							"|intn upsample(shortn,ushortn)"
							"|uintn upsample(ushortn,ushortn)"
							"|long upsample(int,uint)"
							"|ulong upsample(uint,uint)"
							"|longn upsample(intn,uintn)"
							"|ulongn upsample(uintn,uintn)", gentype_single);
		REGISTER_OVERLOADED("gentype mad24(gentype,gentype,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype mul24(gentype,gentype,gentype)", gentype_integers);

		// Common functions
		REGISTER_OVERLOADED("gentype clamp(gentype,gentype,gentype)|gentype clamp(gentype,float,float)", gentype_floats);
		REGISTER_OVERLOADED("gentype degrees(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype max(gentype,gentype)|gentype max(gentype,float)", gentype_floats);
		REGISTER_OVERLOADED("gentype min(gentype,gentype)|gentype min(gentype,float)", gentype_floats);
		REGISTER_OVERLOADED("gentype mix(gentype,gentype,gentype)|gentype mix(gentype,gentype,float)", gentype_floats);
		REGISTER_OVERLOADED("gentype radians(gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype step(gentype,gentype)|gentype step(float,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype smoothstep(gentype,gentype,gentype)|gentype step(float,float,gentype)", gentype_floats);
		REGISTER_OVERLOADED("gentype sign(gentype)", gentype_floats);

		// Geometric functions
		REGISTER_OVERLOADED("float4 cross(float4,float4)|float3 cross(float3,float3)", gentype_single);
		REGISTER_OVERLOADED("float dot(floatn,floatn)", gentype_floats14);
		REGISTER_OVERLOADED("float distance(floatn,floatn)", gentype_floats14);
		REGISTER_OVERLOADED("float length(floatn)", gentype_floats14);
		REGISTER_OVERLOADED("floatn normalize(floatn)", gentype_floats14);
		REGISTER_OVERLOADED("float fast_distance(floatn,floatn)", gentype_floats14);
		REGISTER_OVERLOADED("float fast_length(floatn)", gentype_floats14);
		REGISTER_OVERLOADED("floatn fast_normalize(floatn)", gentype_floats14);

		// Relational functions
		REGISTER_OVERLOADED("intn isequal(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isnotequal(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isgreater(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isgreaterequal(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isless(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn islessequal(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn islessgreater(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isfinite(floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isinf(floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isnan(floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isnormal(floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isordered(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn isunordered(floatn,floatn)", gentype_floats);
		REGISTER_OVERLOADED("intn signbit(floatn)", gentype_floats);
		REGISTER_OVERLOADED("int any(gentype)", gentype_signed);
		REGISTER_OVERLOADED("int all(gentype)", gentype_signed);
		REGISTER_OVERLOADED("gentype bitselect(gentype,gentype,gentype)", gentype_all);
		REGISTER_OVERLOADED("gentype select(gentype,gentype,ugentype)|gentype select(gentype,gentype,igentype)", gentype_all);

		// Sync functions
		REGISTER_VAR(int, CLK_LOCAL_MEM_FENCE);
		REGISTER_VAR(int, CLK_GLOBAL_MEM_FENCE);
		REGISTER(void, barrier, 1);

		// Memory fence function
		REGISTER(void, mem_fence, 1);
		REGISTER(void, read_mem_fence, 1);
		REGISTER(void, write_mem_fence, 1);

		// Async Copies from Global to Local Memory, Local to Global Memory, and Prefetch
		REGISTER_OVERLOADED("event_t async_work_group_copy(__global gentype*,const __local gentype*,size_t,event_t)|event_t async_work_group_copy(__local gentype*,const __global gentype*,size_t,event_t)", gentype_all);
		REGISTER_OVERLOADED("event_t async_work_group_strided_copy(__global gentype*,const __local gentype*,size_t,size_t,event_t)|event_t async_work_group_strided_copy(__local gentype*,const __global gentype*,size_t,size_t,event_t)", gentype_all);
		REGISTER_OVERLOADED("void wait_group_events(int num_events,event_t*)", gentype_single);
		REGISTER_OVERLOADED("void prefetch(const __global gentype*,size_t)", gentype_all);

		// Atomic Functions
		REGISTER_OVERLOADED("int atomic_add(__global int*,int)|uint atomic_add(__global uint*,uint)|int atomic_add(__local int*,int)|uint atomic_add(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atomic_sub(__global int*,int)|uint atomic_sub(__global uint*,uint)|int atomic_sub(__local int*,int)|uint atomic_sub(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atomic_xchg(__global int*,int)"
							"|uint atomic_xchg(__global uint*,uint)"
							"|float atomic_xchg(__global float*,float)"
							"|int atomic_xchg(__local int*,int)"
							"|uint atomic_xchg(__local uint*,uint)"
							"|float atomic_xchg(__local float*,float)", gentype_single);
		REGISTER_OVERLOADED("int atomic_inc(__global int*)|uint atomic_inc(__global uint*)|int atomic_inc(__local int*)|uint atomic_inc(__local uint*)", gentype_single);
		REGISTER_OVERLOADED("int atomic_dec(__global int*)|uint atomic_dec(__global uint*)|int atomic_dec(__local int*)|uint atomic_dec(__local uint*)", gentype_single);
		REGISTER_OVERLOADED("int atomic_cmpxchg(__global int*,int,int)"
							"|uint atomic_cmpxchg(__global uint*,uint,uint)"
							"|int atomic_cmpxchg(__local int*,int,int)"
							"|uint atomic_cmpxchg(__local uint*,uint,uint)", gentype_single);
		REGISTER_OVERLOADED("int atomic_min(__global int*,int)|uint atomic_min(__global uint*,uint)|int atomic_min(__local int*,int)|uint atomic_min(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atomic_max(__global int*,int)|uint atomic_max(__global uint*,uint)|int atomic_max(__local int*,int)|uint atomic_max(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atomic_and(__global int*,int)|uint atomic_and(__global uint*,uint)|int atomic_and(__local int*,int)|uint atomic_and(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atomic_or(__global int*,int)|uint atomic_or(__global uint*,uint)|int atomic_or(__local int*,int)|uint atomic_or(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atomic_xor(__global int*,int)|uint atomic_xor(__global uint*,uint)|int atomic_xor(__local int*,int)|uint atomic_xor(__local uint*,uint)", gentype_single);

		// Miscellaneous Vector Functions
		REGISTER_OVERLOADED("int vec_step(gentype)", gentype_all);
		REGISTER_OVERLOADED("int vec_step(gentype)", gentype_half);
//		REGISTER_OVERLOADED("int shuffle(gentype)", gentype_half);
//		REGISTER_OVERLOADED("int shuffle2(gentype)", gentype_half);

		// Image read and write functions
		REGISTER_VAR(uint, CLK_NORMALIZED_COORDS_FALSE);
		REGISTER_VAR(uint, CLK_NORMALIZED_COORDS_TRUE);
		REGISTER_VAR(uint, CLK_ADDRESS_NONE);
		REGISTER_VAR(uint, CLK_ADDRESS_CLAMP_TO_EDGE);
		REGISTER_VAR(uint, CLK_ADDRESS_REPEAT);
		REGISTER_VAR(uint, CLK_ADDRESS_CLAMP);
		REGISTER_VAR(uint, CLK_ADDRESS_MIRRORED_REPEAT);
		REGISTER_VAR(uint, CLK_FILTER_NEAREST);
		REGISTER_VAR(uint, CLK_FILTER_LINEAR);
		REGISTER_VAR(uint, CLK_R);
		REGISTER_VAR(uint, CLK_A);
		REGISTER_VAR(uint, CLK_RG);
		REGISTER_VAR(uint, CLK_RA);
		REGISTER_VAR(uint, CLK_RGB);
		REGISTER_VAR(uint, CLK_RGBA);
		REGISTER_VAR(uint, CLK_BGRA);
		REGISTER_VAR(uint, CLK_ARGB);
		REGISTER_VAR(uint, CLK_INTENSITY);
		REGISTER_VAR(uint, CLK_LUMINANCE);
		REGISTER_VAR(uint, CLK_Rx);
		REGISTER_VAR(uint, CLK_RGx);
		REGISTER_VAR(uint, CLK_RGBx);
		REGISTER_VAR(uint, CLK_SNORM_INT8);
		REGISTER_VAR(uint, CLK_SNORM_INT16);
		REGISTER_VAR(uint, CLK_UNORM_INT8);
		REGISTER_VAR(uint, CLK_UNORM_INT16);
		REGISTER_VAR(uint, CLK_UNORM_SHORT_565);
		REGISTER_VAR(uint, CLK_UNORM_SHORT_555);
		REGISTER_VAR(uint, CLK_UNORM_INT_101010);
		REGISTER_VAR(uint, CLK_SIGNED_INT8);
		REGISTER_VAR(uint, CLK_SIGNED_INT16);
		REGISTER_VAR(uint, CLK_SIGNED_INT32);
		REGISTER_VAR(uint, CLK_UNSIGNED_INT8);
		REGISTER_VAR(uint, CLK_UNSIGNED_INT16);
		REGISTER_VAR(uint, CLK_UNSIGNED_INT32);
		REGISTER_VAR(uint, CLK_HALF_FLOAT);
		REGISTER_VAR(uint, CLK_FLOAT);
		REGISTER_OVERLOADED("float4 read_imagef(image2d_t,sampler_t,int2)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image2d_t,sampler_t,float2)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image2d_t,sampler_t,int2)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image2d_t,sampler_t,float2)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image2d_t,sampler_t,int2)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image2d_t,sampler_t,float2)", gentype_single);
		REGISTER_OVERLOADED("void write_imagef(image2d_t,int2,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagei(image2d_t,int2,int4)", gentype_single);
		REGISTER_OVERLOADED("void write_imageui(image2d_t,int2,uint4)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image3d_t,sampler_t,int4)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image3d_t,sampler_t,float4)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image3d_t,sampler_t,int4)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image3d_t,sampler_t,float4)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image3d_t,sampler_t,int4)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image3d_t,sampler_t,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagef(image3d_t,int4,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagei(image3d_t,int4,int4)", gentype_single);
		REGISTER_OVERLOADED("void write_imageui(image3d_t,int4,uint4)", gentype_single);
		REGISTER_OVERLOADED("int get_image_width(image2d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_width(image3d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_height(image2d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_height(image3d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_depth(image3d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_channel_data_type(image2d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_channel_data_type(image3d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_channel_order(image2d_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_channel_order(image3d_t)", gentype_single);
		REGISTER_OVERLOADED("int2 get_image_dim(image2d_t)", gentype_single);
		REGISTER_OVERLOADED("int4 get_image_dim(image3d_t)", gentype_single);

		// as_type(n) functions
		REGISTER_OVERLOADED("char as_char(gentype)", gentype_size1);
		REGISTER_OVERLOADED("uchar as_uchar(gentype)", gentype_size1);
		REGISTER_OVERLOADED("short as_short(gentype)", gentype_size2);
		REGISTER_OVERLOADED("ushort as_ushort(gentype)", gentype_size2);
		REGISTER_OVERLOADED("int as_int(gentype)", gentype_size4);
		REGISTER_OVERLOADED("uint as_uint(gentype)", gentype_size4);
		REGISTER_OVERLOADED("long as_long(gentype)", gentype_size8);
		REGISTER_OVERLOADED("ulong as_ulong(gentype)", gentype_size8);
		REGISTER_OVERLOADED("float as_float(gentype)", gentype_size4);
		REGISTER_OVERLOADED("double as_double(gentype)", gentype_size8);
		switch(sizeof(void*))
		{
		case 4:
			REGISTER_OVERLOADED("size_t as_size_t(gentype)", gentype_size4);
			break;
		case 8:
			REGISTER_OVERLOADED("size_t as_size_t(gentype)", gentype_size8);
			break;
		}
		REGISTER_OVERLOADED("char2 as_char2(gentype)", gentype_size2);
		REGISTER_OVERLOADED("uchar2 as_uchar2(gentype)", gentype_size2);
		REGISTER_OVERLOADED("short2 as_short2(gentype)", gentype_size4);
		REGISTER_OVERLOADED("ushort2 as_ushort2(gentype)", gentype_size4);
		REGISTER_OVERLOADED("int2 as_int2(gentype)", gentype_size8);
		REGISTER_OVERLOADED("uint2 as_uint2(gentype)", gentype_size8);
		REGISTER_OVERLOADED("long2 as_long2(gentype)", gentype_size16);
		REGISTER_OVERLOADED("ulong2 as_ulong2(gentype)", gentype_size16);
		REGISTER_OVERLOADED("float2 as_float2(gentype)", gentype_size8);
		REGISTER_OVERLOADED("double2 as_double2(gentype)", gentype_size16);
		REGISTER_OVERLOADED("char3 as_char3(gentype)", gentype_size3);
		REGISTER_OVERLOADED("uchar3 as_uchar3(gentype)", gentype_size3);
		REGISTER_OVERLOADED("short3 as_short3(gentype)", gentype_size6);
		REGISTER_OVERLOADED("ushort3 as_ushort3(gentype)", gentype_size6);
		REGISTER_OVERLOADED("int3 as_int3(gentype)", gentype_size12);
		REGISTER_OVERLOADED("uint3 as_uint3(gentype)", gentype_size12);
		REGISTER_OVERLOADED("long3 as_long3(gentype)", gentype_size24);
		REGISTER_OVERLOADED("ulong3 as_ulong3(gentype)", gentype_size24);
		REGISTER_OVERLOADED("float3 as_float3(gentype)", gentype_size12);
		REGISTER_OVERLOADED("double3 as_double3(gentype)", gentype_size24);
		REGISTER_OVERLOADED("char3 as_char3(gentype)", gentype_size4);
		REGISTER_OVERLOADED("uchar3 as_uchar3(gentype)", gentype_size4);
		REGISTER_OVERLOADED("short3 as_short3(gentype)", gentype_size8);
		REGISTER_OVERLOADED("ushort3 as_ushort3(gentype)", gentype_size8);
		REGISTER_OVERLOADED("int3 as_int3(gentype)", gentype_size16);
		REGISTER_OVERLOADED("uint3 as_uint3(gentype)", gentype_size16);
		REGISTER_OVERLOADED("long3 as_long3(gentype)", gentype_size32);
		REGISTER_OVERLOADED("ulong3 as_ulong3(gentype)", gentype_size32);
		REGISTER_OVERLOADED("float3 as_float3(gentype)", gentype_size16);
		REGISTER_OVERLOADED("double3 as_double3(gentype)", gentype_size32);
		REGISTER_OVERLOADED("char4 as_char4(gentype)", gentype_size4);
		REGISTER_OVERLOADED("uchar4 as_uchar4(gentype)", gentype_size4);
		REGISTER_OVERLOADED("short4 as_short4(gentype)", gentype_size8);
		REGISTER_OVERLOADED("ushort4 as_ushort4(gentype)", gentype_size8);
		REGISTER_OVERLOADED("int4 as_int4(gentype)", gentype_size16);
		REGISTER_OVERLOADED("uint4 as_uint4(gentype)", gentype_size16);
		REGISTER_OVERLOADED("long4 as_long4(gentype)", gentype_size32);
		REGISTER_OVERLOADED("ulong4 as_ulong4(gentype)", gentype_size32);
		REGISTER_OVERLOADED("float4 as_float4(gentype)", gentype_size16);
		REGISTER_OVERLOADED("double4 as_double4(gentype)", gentype_size32);
		REGISTER_OVERLOADED("char8 as_char8(gentype)", gentype_size8);
		REGISTER_OVERLOADED("uchar8 as_uchar8(gentype)", gentype_size8);
		REGISTER_OVERLOADED("short8 as_short8(gentype)", gentype_size16);
		REGISTER_OVERLOADED("ushort8 as_ushort8(gentype)", gentype_size16);
		REGISTER_OVERLOADED("int8 as_int8(gentype)", gentype_size32);
		REGISTER_OVERLOADED("uint8 as_uint8(gentype)", gentype_size32);
		REGISTER_OVERLOADED("long8 as_long8(gentype)", gentype_size64);
		REGISTER_OVERLOADED("ulong8 as_ulong8(gentype)", gentype_size64);
		REGISTER_OVERLOADED("float8 as_float8(gentype)", gentype_size32);
		REGISTER_OVERLOADED("double8 as_double8(gentype)", gentype_size64);
		REGISTER_OVERLOADED("char16 as_char16(gentype)", gentype_size16);
		REGISTER_OVERLOADED("uchar16 as_uchar16(gentype)", gentype_size16);
		REGISTER_OVERLOADED("short16 as_short16(gentype)", gentype_size32);
		REGISTER_OVERLOADED("ushort16 as_ushort16(gentype)", gentype_size32);
		REGISTER_OVERLOADED("int16 as_int16(gentype)", gentype_size64);
		REGISTER_OVERLOADED("uint16 as_uint16(gentype)", gentype_size64);
		REGISTER_OVERLOADED("long16 as_long16(gentype)", gentype_size128);
		REGISTER_OVERLOADED("ulong16 as_ulong16(gentype)", gentype_size128);
		REGISTER_OVERLOADED("float16 as_float16(gentype)", gentype_size64);
		REGISTER_OVERLOADED("double16 as_double16(gentype)", gentype_size128);

		// vector data load and store functions
#define REGISTER_VLOAD_VSTOR(T,N)\
		REGISTER_OVERLOADED(#T #N " vload"#N"(size_t, const __global "#T" *)", gentype_single);\
		REGISTER_OVERLOADED(#T #N " vload"#N"(size_t, const __constant "#T" *)", gentype_single);\
		REGISTER_OVERLOADED(#T #N " vload"#N"(size_t, const __local "#T" *)", gentype_single);\
		REGISTER_OVERLOADED(#T #N " vload"#N"(size_t, const __private "#T" *)", gentype_single);\
		REGISTER_OVERLOADED("void vstore"#N"("#T #N", size_t, __global "#T" *)", gentype_single);\
		REGISTER_OVERLOADED("void vstore"#N"("#T #N", size_t, __local "#T" *)", gentype_single);\
		REGISTER_OVERLOADED("void vstore"#N"("#T #N", size_t, __private "#T" *)", gentype_single)

#define REGISTER_VLOAD_VSTOR_ALL(T)\
		REGISTER_VLOAD_VSTOR(T, 2);\
		REGISTER_VLOAD_VSTOR(T, 3);\
		REGISTER_VLOAD_VSTOR(T, 4);\
		REGISTER_VLOAD_VSTOR(T, 8);\
		REGISTER_VLOAD_VSTOR(T, 16)

		REGISTER_VLOAD_VSTOR_ALL(char);
		REGISTER_VLOAD_VSTOR_ALL(uchar);
		REGISTER_VLOAD_VSTOR_ALL(short);
		REGISTER_VLOAD_VSTOR_ALL(ushort);
		REGISTER_VLOAD_VSTOR_ALL(int);
		REGISTER_VLOAD_VSTOR_ALL(uint);
		REGISTER_VLOAD_VSTOR_ALL(long);
		REGISTER_VLOAD_VSTOR_ALL(ulong);
		REGISTER_VLOAD_VSTOR_ALL(float);
		REGISTER_VLOAD_VSTOR_ALL(double);

#undef REGISTER_VLOAD_VSTOR_ALL
#undef REGISTER_VLOAD_VSTOR

		// Conversion functions
		// Default versions
		REGISTER_OVERLOADED("char convert_char(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("uchar convert_uchar(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("short convert_short(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("ushort convert_ushort(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("int convert_int(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("uint convert_uint(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("long convert_long(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("ulong convert_ulong(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("float convert_float(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("double convert_double(gentype)", gentype_vec1);

		REGISTER_OVERLOADED("char2 convert_char2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("uchar2 convert_uchar2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("short2 convert_short2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("ushort2 convert_ushort2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("int2 convert_int2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("uint2 convert_uint2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("long2 convert_long2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("ulong2 convert_ulong2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("float2 convert_float2(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("double2 convert_double2(gentype)", gentype_vec2);

		REGISTER_OVERLOADED("char3 convert_char3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("uchar3 convert_uchar3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("short3 convert_short3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("ushort3 convert_ushort3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("int3 convert_int3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("uint3 convert_uint3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("long3 convert_long3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("ulong3 convert_ulong3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("float3 convert_float3(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("double3 convert_double3(gentype)", gentype_vec3);

		REGISTER_OVERLOADED("char4 convert_char4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("uchar4 convert_uchar4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("short4 convert_short4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("ushort4 convert_ushort4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("int4 convert_int4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("uint4 convert_uint4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("long4 convert_long4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("ulong4 convert_ulong4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("float4 convert_float4(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("double4 convert_double4(gentype)", gentype_vec4);

		REGISTER_OVERLOADED("char8 convert_char8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("uchar8 convert_uchar8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("short8 convert_short8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("ushort8 convert_ushort8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("int8 convert_int8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("uint8 convert_uint8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("long8 convert_long8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("ulong8 convert_ulong8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("float8 convert_float8(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("double8 convert_double8(gentype)", gentype_vec8);

		REGISTER_OVERLOADED("char16 convert_char16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("uchar16 convert_uchar16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("short16 convert_short16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("ushort16 convert_ushort16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("int16 convert_int16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("uint16 convert_uint16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("long16 convert_long16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("ulong16 convert_ulong16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("float16 convert_float16(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("double16 convert_double16(gentype)", gentype_vec16);

		// Saturated versions
		// Default versions
		REGISTER_OVERLOADED("char convert_char_sat(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("uchar convert_uchar_sat(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("short convert_short_sat(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("ushort convert_ushort_sat(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("int convert_int_sat(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("uint convert_uint_sat(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("long convert_long_sat(gentype)", gentype_vec1);
		REGISTER_OVERLOADED("ulong convert_ulong_sat(gentype)", gentype_vec1);

		REGISTER_OVERLOADED("char2 convert_char2_sat(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("uchar2 convert_uchar2_sat(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("short2 convert_short2_sat(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("ushort2 convert_ushort2_sat(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("int2 convert_int2_sat(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("uint2 convert_uint2_sat(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("long2 convert_long2_sat(gentype)", gentype_vec2);
		REGISTER_OVERLOADED("ulong2 convert_ulong2_sat(gentype)", gentype_vec2);

		REGISTER_OVERLOADED("char3 convert_char3_sat(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("uchar3 convert_uchar3_sat(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("short3 convert_short3_sat(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("ushort3 convert_ushort3_sat(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("int3 convert_int3_sat(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("uint3 convert_uint3_sat(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("long3 convert_long3_sat(gentype)", gentype_vec3);
		REGISTER_OVERLOADED("ulong3 convert_ulong3_sat(gentype)", gentype_vec3);

		REGISTER_OVERLOADED("char4 convert_char4_sat(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("uchar4 convert_uchar4_sat(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("short4 convert_short4_sat(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("ushort4 convert_ushort4_sat(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("int4 convert_int4_sat(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("uint4 convert_uint4_sat(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("long4 convert_long4_sat(gentype)", gentype_vec4);
		REGISTER_OVERLOADED("ulong4 convert_ulong4_sat(gentype)", gentype_vec4);

		REGISTER_OVERLOADED("char8 convert_char8_sat(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("uchar8 convert_uchar8_sat(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("short8 convert_short8_sat(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("ushort8 convert_ushort8_sat(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("int8 convert_int8_sat(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("uint8 convert_uint8_sat(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("long8 convert_long8_sat(gentype)", gentype_vec8);
		REGISTER_OVERLOADED("ulong8 convert_ulong8_sat(gentype)", gentype_vec8);

		REGISTER_OVERLOADED("char16 convert_char16_sat(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("uchar16 convert_uchar16_sat(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("short16 convert_short16_sat(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("ushort16 convert_ushort16_sat(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("int16 convert_int16_sat(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("uint16 convert_uint16_sat(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("long16 convert_long16_sat(gentype)", gentype_vec16);
		REGISTER_OVERLOADED("ulong16 convert_ulong16_sat(gentype)", gentype_vec16);

		// Functions provided by the cl_khr_fp64 extension (double support)
		// Math functions
		REGISTER_OVERLOADED("gentype acos(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype acosh(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype acospi(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype asin(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype asinh(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype asinpi(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype atan(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype atan2(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype atanh(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype atanpi(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype atan2pi(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype cbrt(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype ceil(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype copysign(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype cos(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype cosh(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype cospi(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype erf(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype erfc(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype exp(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype exp2(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype exp10(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype expm1(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype fabs(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype fdim(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype floor(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype fma(gentype,gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype fmax(gentype,gentype)|gentype fmax(gentype,double)", gentype_doubles);
		REGISTER_OVERLOADED("gentype fmin(gentype,gentype)|gentype fmin(gentype,double)", gentype_doubles);
		REGISTER_OVERLOADED("gentype fmod(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype fract(gentype,__global gentype*)|gentype fract(gentype,__local gentype*)|gentype fract(gentype,__private gentype*)", gentype_doubles);
		REGISTER_OVERLOADED("gentype frexp(gentype,__global intn*)|gentype frexp(gentype,__local intn*)|gentype frexp(gentype,__private intn*)", gentype_doubles);
		REGISTER_OVERLOADED("gentype hypot(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("intn ilogb(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype ldexp(gentype,intn)|gentype ldexp(gentype,int)", gentype_doubles);
		REGISTER_OVERLOADED("gentype lgamma(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype lgamma_r(gentype,__global intn*)|gentype lgamma_r(gentype,__local intn*)|gentype lgamma_r(gentype,__private intn*)", gentype_doubles);
		REGISTER_OVERLOADED("gentype log(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype log2(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype log10(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype log1p(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype logb(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype mad(gentype,gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype maxmag(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype minmag(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype modf(gentype,__global gentype*)|gentype modf(gentype,__local gentype*)|gentype modf(gentype,__private gentype*)", gentype_doubles);
		REGISTER_OVERLOADED("floatn nan(uintn)", gentype_doubles);
		REGISTER_OVERLOADED("gentype nextafter(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype pow(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype pown(gentype,intn)", gentype_doubles);
		REGISTER_OVERLOADED("gentype powr(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype remainder(gentype,gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype remquo(gentype,gentype,__global intn*)|gentype remquo(gentype,gentype,__local intn*)|gentype remquo(gentype,gentype,__private intn*)", gentype_doubles);
		REGISTER_OVERLOADED("gentype rint(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype rootn(gentype,intn)", gentype_doubles);
		REGISTER_OVERLOADED("gentype round(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype rsqrt(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype sin(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype sincos(gentype,__global gentype*)|gentype sincos(gentype,__local gentype*)|gentype sincos(gentype,__private gentype*)", gentype_doubles);
		REGISTER_OVERLOADED("gentype sinh(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype sinpi(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype sqrt(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype tan(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype tanh(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype tanpi(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype tgamma(gentype)", gentype_doubles);
		REGISTER_OVERLOADED("gentype trunc(gentype)", gentype_doubles);

		// Common functions
		REGISTER_OVERLOADED("gentype clamp(gentype,gentype,gentype)|gentype clamp(gentype,double,double)", gentype_doubles);
		REGISTER_OVERLOADED("gentype degrees(gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype max(gentype,gentype)|gentype max(gentype,double)", gentype_doubles);
		REGISTER_OVERLOADED("gentype min(gentype,gentype)|gentype min(gentype,double)", gentype_doubles);
		REGISTER_OVERLOADED("gentype mix(gentype,gentype,gentype)|gentype mix(gentype,gentype,double)", gentype_doubles);
		REGISTER_OVERLOADED("gentype radians(gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype step(gentype,gentype)|gentype step(double,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype smoothstep(gentype,gentype,gentype)|gentype step(double,double,gentype)", gentype_integers);
		REGISTER_OVERLOADED("gentype sign(gentype)", gentype_integers);

		// Geometric functions
		REGISTER_OVERLOADED("double4 cross(double4,double4)|double3 cross(double3,double3)", gentype_single);
		REGISTER_OVERLOADED("double dot(doublen,doublen)", gentype_doubles14);
		REGISTER_OVERLOADED("double distance(doublen,doublen)", gentype_doubles14);
		REGISTER_OVERLOADED("double length(doublen)", gentype_doubles14);
		REGISTER_OVERLOADED("doublen normalize(doublen)", gentype_doubles14);

		// Relational functions
		REGISTER_OVERLOADED("longn isequal(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isnotequal(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isgreater(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isgreaterequal(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isless(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn islessequal(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn islessgreater(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isfinite(doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isinf(doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isnan(doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isnormal(doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isordered(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn isunordered(doublen,doublen)", gentype_double_vectors);
		REGISTER_OVERLOADED("longn signbit(doublen)", gentype_double_vectors);

		REGISTER_OVERLOADED("int isequal(double,double)", gentype_single);
		REGISTER_OVERLOADED("int isnotequal(double,double)", gentype_single);
		REGISTER_OVERLOADED("int isgreater(double,double)", gentype_single);
		REGISTER_OVERLOADED("int isgreaterequal(double,double)", gentype_single);
		REGISTER_OVERLOADED("int isless(double,double)", gentype_single);
		REGISTER_OVERLOADED("int islessequal(double,double)", gentype_single);
		REGISTER_OVERLOADED("int islessgreater(double,double)", gentype_single);
		REGISTER_OVERLOADED("int isfinite(double)", gentype_single);
		REGISTER_OVERLOADED("int isinf(double)", gentype_single);
		REGISTER_OVERLOADED("int isnan(double)", gentype_single);
		REGISTER_OVERLOADED("int isnormal(double)", gentype_single);
		REGISTER_OVERLOADED("int isordered(double,double)", gentype_single);
		REGISTER_OVERLOADED("int isunordered(double,double)", gentype_single);
		REGISTER_OVERLOADED("int signbit(double)", gentype_single);

		// Functions provided by the cl_*_int32_*_atomics extensions
		// Atomic Functions
		REGISTER_OVERLOADED("int atom_add(__global int*,int)|uint atom_add(__global uint*,uint)", gentype_single);
		REGISTER_OVERLOADED_REDIRECT(atom_add, "int __atom_add_local(__local int*,int)|uint __atom_add_local(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_sub(__global int*,int)|uint atom_sub(__global uint*,uint)|int atom_sub(__local int*,int)|uint atom_sub(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_xchg(__global int*,int)"
							"|uint atom_xchg(__global uint*,uint)"
							"|int atom_xchg(__local int*,int)"
							"|uint atom_xchg(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_inc(__global int*)|uint atom_inc(__global uint*)|int atom_inc(__local int*)|uint atom_inc(__local uint*)", gentype_single);
		REGISTER_OVERLOADED("int atom_dec(__global int*)|uint atom_dec(__global uint*)|int atom_dec(__local int*)|uint atom_dec(__local uint*)", gentype_single);
		REGISTER_OVERLOADED("int atom_cmpxchg(__global int*,int,int)"
							"|uint atom_cmpxchg(__global uint*,uint,uint)"
							"|int atom_cmpxchg(__local int*,int,int)"
							"|uint atom_cmpxchg(__local uint*,uint,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_min(__global int*,int)|uint atom_min(__global uint*,uint)|int atom_min(__local int*,int)|uint atom_min(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_max(__global int*,int)|uint atom_max(__global uint*,uint)|int atom_max(__local int*,int)|uint atom_max(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_and(__global int*,int)|uint atom_and(__global uint*,uint)|int atom_and(__local int*,int)|uint atom_and(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_or(__global int*,int)|uint atom_or(__global uint*,uint)|int atom_or(__local int*,int)|uint atom_or(__local uint*,uint)", gentype_single);
		REGISTER_OVERLOADED("int atom_xor(__global int*,int)|uint atom_xor(__global uint*,uint)|int atom_xor(__local int*,int)|uint atom_xor(__local uint*,uint)", gentype_single);

		// OpenCL 1.2 image functions
		// Integer functions
		REGISTER_OVERLOADED("gentype popcount(gentype)", gentype_integers);

		// Image functions
		REGISTER_OVERLOADED("float4 read_imagef(image1d_t,sampler_t,int)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image1d_t,sampler_t,float)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image1d_t,sampler_t,int)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image1d_t,sampler_t,float)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image1d_t,sampler_t,int)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image1d_t,sampler_t,float)", gentype_single);
		REGISTER_OVERLOADED("void write_imagef(image1d_t,int,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagei(image1d_t,int,int4)", gentype_single);
		REGISTER_OVERLOADED("void write_imageui(image1d_t,int,uint4)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image1d_t,int)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image1d_t,int)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image1d_t,int)", gentype_single);
		REGISTER_OVERLOADED("int get_image_width(image1d_t)", gentype_single);

		REGISTER_OVERLOADED("float4 read_imagef(image1d_buffer_t,int)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image1d_buffer_t,int)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image1d_buffer_t,int)", gentype_single);
		REGISTER_OVERLOADED("void write_imagef(image1d_buffer_t,int,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagei(image1d_buffer_t,int,int4)", gentype_single);
		REGISTER_OVERLOADED("void write_imageui(image1d_buffer_t,int,uint4)", gentype_single);
		REGISTER_OVERLOADED("int get_image_width(image1d_buffer_t)", gentype_single);

		REGISTER_OVERLOADED("float4 read_imagef(image1d_array_t,sampler_t,int2)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image1d_array_t,sampler_t,float2)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image1d_array_t,sampler_t,int2)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image1d_array_t,sampler_t,float2)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image1d_array_t,sampler_t,int2)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image1d_array_t,sampler_t,float2)", gentype_single);
		REGISTER_OVERLOADED("void write_imagef(image1d_array_t,int2,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagei(image1d_array_t,int2,int4)", gentype_single);
		REGISTER_OVERLOADED("void write_imageui(image1d_array_t,int2,uint4)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image1d_array_t,int2)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image1d_array_t,int2)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image1d_array_t,int2)", gentype_single);
		REGISTER_OVERLOADED("int get_image_width(image1d_array_t)", gentype_single);
		REGISTER_OVERLOADED("size_t get_image_array_size(image1d_array_t)", gentype_single);

		REGISTER_OVERLOADED("float4 read_imagef(image2d_array_t,sampler_t,int4)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image2d_array_t,sampler_t,float4)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image2d_array_t,sampler_t,int4)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image2d_array_t,sampler_t,float4)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image2d_array_t,sampler_t,int4)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image2d_array_t,sampler_t,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagef(image2d_array_t,int4,float4)", gentype_single);
		REGISTER_OVERLOADED("void write_imagei(image2d_array_t,int4,int4)", gentype_single);
		REGISTER_OVERLOADED("void write_imageui(image2d_array_t,int4,uint4)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image2d_array_t,int4)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image2d_array_t,int4)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image2d_array_t,int4)", gentype_single);
		REGISTER_OVERLOADED("int get_image_width(image2d_array_t)", gentype_single);
		REGISTER_OVERLOADED("int get_image_height(image2d_array_t)", gentype_single);
		REGISTER_OVERLOADED("size_t get_image_array_size(image2d_array_t)", gentype_single);

		REGISTER_OVERLOADED("float4 read_imagef(image2d_t,int2)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image2d_t,int2)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image2d_t,int2)", gentype_single);
		REGISTER_OVERLOADED("float4 read_imagef(image3d_t,int)", gentype_single);
		REGISTER_OVERLOADED("int4 read_imagei(image3d_t,int)", gentype_single);
		REGISTER_OVERLOADED("uint4 read_imageui(image3d_t,int)", gentype_single);

#undef REGISTER
#undef REGISTER_OVERLOADED
#undef REGISTER_VAR

		b_init = false;
		syms = *symbols;
	}

	bool builtin::has_references_to(const std::string &function_name) const
	{
		return name == function_name;
	}
}
