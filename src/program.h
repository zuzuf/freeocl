#ifndef __FREEOCL_PROGRAM_H__
#define __FREEOCL_PROGRAM_H__

#include "freeocl.h"
#include <vector>

struct _cl_program : public FreeOCL::icd_table, public FreeOCL::ref_counter
{
};

#endif
