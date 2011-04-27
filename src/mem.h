#ifndef __FREEOCL_MEM_H__
#define __FREEOCL_MEM_H__

#include "freeocl.h"
#include <vector>
#include <deque>

namespace FreeOCL
{
	struct mem_call_back
	{
		void (CL_CALLBACK *pfn_notify)(cl_mem memobj,
									   void *user_data);
		void *user_data;
	};
}

struct _cl_mem : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::mutex
{
	void *ptr;
	size_t size;
	cl_mem_flags flags;
	cl_context context;
	cl_mem_object_type mem_type;
	cl_mem parent;
	void *host_ptr;

	~_cl_mem();
	std::deque<FreeOCL::mem_call_back> call_backs;
};

#endif
