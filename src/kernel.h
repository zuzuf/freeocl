#ifndef __FREEOCL_KERNEL_H__
#define __FREEOCL_KERNEL_H__

#include "freeocl.h"
#include <vector>

struct _cl_kernel : public FreeOCL::icd_table, public FreeOCL::ref_counter
{
};

#endif
