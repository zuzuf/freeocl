#ifndef __FREEOCL_CONTEXT_H__
#define __FREEOCL_CONTEXT_H__

#include "freeocl.h"
#include <vector>

struct _cl_context : public FreeOCL::icd_table, public FreeOCL::ref_counter, public FreeOCL::mutex
{
	std::vector<cl_device_id>	devices;
	std::vector<cl_context_properties>	properties;
	void (CL_CALLBACK *pfn_notify)(const char *errinfo,
								   const void *private_info,
								   size_t cb,
								   void *user_data);
	void *user_data;
};

#endif
