#include "mem.h"
#include "context.h"
#include "commandqueue.h"
#include <cstring>
#include <iostream>

#define SET_VAR(X)	FreeOCL::copyMemoryWithinLimits(&(X), sizeof(X), param_value_size, param_value, param_value_size_ret)

extern "C"
{
	cl_mem clCreateBuffer (cl_context context,
						   cl_mem_flags flags,
						   size_t size,
						   void *host_ptr,
						   cl_int *errcode_ret)
	{
		if (size == 0)
		{
			if (errcode_ret)
				*errcode_ret = CL_INVALID_BUFFER_SIZE;
			return 0;
		}
		if (((flags & (CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR))
			&& host_ptr == NULL)
			|| (host_ptr != NULL && !(flags & (CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR))))
		{
			if (errcode_ret)
				*errcode_ret = CL_INVALID_HOST_PTR;
			return 0;
		}

		if ((flags & CL_MEM_USE_HOST_PTR) && (flags & (CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR)))
		{
			if (errcode_ret)
				*errcode_ret = CL_INVALID_VALUE;
			return 0;
		}

		if (!FreeOCL::isValid(context))
		{
			if (errcode_ret)
				*errcode_ret = CL_INVALID_CONTEXT;
			return 0;
		}

		cl_mem mem = new _cl_mem;
		mem->context = context;
		mem->flags = flags;
		mem->size = size;
		mem->mem_type = CL_MEM_OBJECT_BUFFER;
		mem->host_ptr = host_ptr;
		mem->parent = NULL;
		if (flags & CL_MEM_USE_HOST_PTR)
			mem->ptr = host_ptr;
		else if (posix_memalign(&(mem->ptr), 256, size) == ENOMEM)
		{
			if (errcode_ret)
				*errcode_ret = CL_OUT_OF_RESOURCES;
			delete mem;
			return 0;
		}

		if (flags & CL_MEM_COPY_HOST_PTR)
			memcpy(mem->ptr, host_ptr, size);

		context->unlock();

		return mem;
	}

	cl_mem clCreateSubBuffer (cl_mem buffer,
							  cl_mem_flags flags,
							  cl_buffer_create_type buffer_create_type,
							  const void *buffer_create_info,
							  cl_int *errcode_ret)
	{
		std::cerr << "Stub : clCreateSubBuffer" << std::endl;
		if (errcode_ret)
			*errcode_ret = CL_INVALID_VALUE;
		return 0;
	}

	cl_int clRetainMemObject (cl_mem memobj)
	{
		memobj->lock();
		memobj->retain();
		memobj->unlock();
		return CL_SUCCESS;
	}

	cl_int clReleaseMemObject (cl_mem memobj)
	{
		memobj->lock();
		memobj->release();
		if (memobj->get_ref_count() == 0)
			delete memobj;
		else
			memobj->unlock();
		return CL_SUCCESS;
	}

	cl_int clSetMemObjectDestructorCallback (cl_mem memobj,
											 void (CL_CALLBACK *pfn_notify)(cl_mem memobj,
																			void *user_data),
											 void *user_data)
	{
		if (pfn_notify)
			return CL_INVALID_VALUE;

		memobj->lock();
		FreeOCL::mem_call_back call_back = { pfn_notify, user_data };
		memobj->call_backs.push_back(call_back);
		memobj->unlock();
		return CL_SUCCESS;
	}

	cl_int clGetMemObjectInfo (cl_mem memobj,
							   cl_mem_info param_name,
							   size_t param_value_size,
							   void *param_value,
							   size_t *param_value_size_ret)
	{
		memobj->lock();
		bool bTooSmall = false;
		switch(param_name)
		{
		case CL_MEM_TYPE:					bTooSmall = SET_VAR(memobj->mem_type);	break;
		case CL_MEM_FLAGS:					bTooSmall = SET_VAR(memobj->flags);	break;
		case CL_MEM_SIZE:					bTooSmall = SET_VAR(memobj->size);	break;
		case CL_MEM_HOST_PTR:				bTooSmall = SET_VAR(memobj->host_ptr);	break;
		case CL_MEM_MAP_COUNT:
		case CL_MEM_REFERENCE_COUNT:		bTooSmall = SET_VAR(memobj->get_ref_count());	break;
		case CL_MEM_CONTEXT:				bTooSmall = SET_VAR(memobj->context);	break;
		case CL_MEM_ASSOCIATED_MEMOBJECT:	bTooSmall = SET_VAR(memobj->parent);	break;
		case CL_MEM_OFFSET:
		default:
			memobj->unlock();
			return CL_INVALID_VALUE;
		}

		memobj->unlock();
		if (bTooSmall && param_value != NULL)
			return CL_INVALID_VALUE;

		return CL_SUCCESS;
	}

	cl_int clEnqueueReadBuffer (cl_command_queue command_queue,
								cl_mem buffer,
								cl_bool blocking_read,
								size_t offset,
								size_t cb,
								void *ptr,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		command_queue->lock();
		command_queue->unlock();
	}

	cl_int clEnqueueWriteBuffer (cl_command_queue command_queue,
								 cl_mem buffer,
								 cl_bool blocking_write,
								 size_t offset,
								 size_t cb,
								 const void *ptr,
								 cl_uint num_events_in_wait_list,
								 const cl_event *event_wait_list,
								 cl_event *event)
	{

	}
}

_cl_mem::_cl_mem()
{
	FreeOCL::global_mutex.lock();
	FreeOCL::valid_mem.insert(this);
	FreeOCL::global_mutex.unlock();
}

_cl_mem::~_cl_mem()
{
	std::deque<FreeOCL::mem_call_back> call_backs;
	call_backs.swap(this->call_backs);
	unlock();

	for(std::deque<FreeOCL::mem_call_back>::const_iterator i = call_backs.begin() ; i != call_backs.end() ; ++i)
		i->pfn_notify(this, i->user_data);

	FreeOCL::global_mutex.lock();
	FreeOCL::valid_mem.erase(this);
	FreeOCL::global_mutex.unlock();
}
