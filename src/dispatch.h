/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef __FREEOCL_DISPATCH_H__
#define __FREEOCL_DISPATCH_H__

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

typedef CL_API_ENTRY cl_int (CL_API_CALL *_stub)(void*);

struct _cl_icd_dispatch
{
/* #0 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetPlatformIDs)(cl_uint          /* num_entries */,
					 cl_platform_id * /* platforms */,
					 cl_uint *        /* num_platforms */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetPlatformInfo)(cl_platform_id   /* platform */,
					  cl_platform_info /* param_name */,
					  size_t           /* param_value_size */,
					  void *           /* param_value */,
					  size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #2 */
	/* Device APIs */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetDeviceIDs)(cl_platform_id   /* platform */,
				   cl_device_type   /* device_type */,
				   cl_uint          /* num_entries */,
				   cl_device_id *   /* devices */,
				   cl_uint *        /* num_devices */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetDeviceInfo)(cl_device_id    /* device */,
					cl_device_info  /* param_name */,
					size_t          /* param_value_size */,
					void *          /* param_value */,
					size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #4 */
	/* Context APIs  */
	CL_API_ENTRY cl_context (CL_API_CALL *
	clCreateContext)(const cl_context_properties * /* properties */,
					cl_uint                       /* num_devices */,
					const cl_device_id *          /* devices */,
					void (CL_CALLBACK * /* pfn_notify */)(const char *, const void *, size_t, void *),
					void *                        /* user_data */,
					cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #5 */
//	_stub	unknown;
	CL_API_ENTRY cl_context (CL_API_CALL *
	clCreateContextFromType)(const cl_context_properties * /* properties */,
							cl_device_type                /* device_type */,
							void (CL_CALLBACK *     /* pfn_notify*/ )(const char *, const void *, size_t, void *),
							void *                        /* user_data */,
							cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainContext)(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseContext)(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetContextInfo)(cl_context         /* context */,
					 cl_context_info    /* param_name */,
					 size_t             /* param_value_size */,
					 void *             /* param_value */,
					 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #9 */
	/* Command Queue APIs */
	CL_API_ENTRY cl_command_queue (CL_API_CALL *
	clCreateCommandQueue)(cl_context                     /* context */,
						 cl_device_id                   /* device */,
						 cl_command_queue_properties    /* properties */,
						 cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainCommandQueue)(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseCommandQueue)(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetCommandQueueInfo)(cl_command_queue      /* command_queue */,
						  cl_command_queue_info /* param_name */,
						  size_t                /* param_value_size */,
						  void *                /* param_value */,
						  size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/*
	 *  WARNING:
	 *     This API introduces mutable state into the OpenCL implementation. It has been REMOVED
	 *  to better facilitate thread safety.  The 1.0 API is not thread safe. It is not tested by the
	 *  OpenCL 1.1 conformance test, and consequently may not work or may not work dependably.
	 *  It is likely to be non-performant. Use of this API is not advised. Use at your own risk.
	 *
	 *  Software developers previously relying on this API are instructed to set the command queue
	 *  properties when creating the queue, instead.
	 */
/* #13 */
	CL_API_ENTRY cl_int (CL_API_CALL*
	clSetCommandQueueProperty)(cl_command_queue              /* command_queue */,
							  cl_command_queue_properties   /* properties */,
							  cl_bool                        /* enable */,
							  cl_command_queue_properties * /* old_properties */) CL_EXT_SUFFIX__VERSION_1_0_DEPRECATED;

/* #14 */
	/* Memory Object APIs */
	CL_API_ENTRY cl_mem (CL_API_CALL *
	clCreateBuffer)(cl_context   /* context */,
				   cl_mem_flags /* flags */,
				   size_t       /* size */,
				   void *       /* host_ptr */,
				   cl_int *     /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_mem (CL_API_CALL *
	clCreateImage2D)(cl_context              /* context */,
					cl_mem_flags            /* flags */,
					const cl_image_format * /* image_format */,
					size_t                  /* image_width */,
					size_t                  /* image_height */,
					size_t                  /* image_row_pitch */,
					void *                  /* host_ptr */,
					cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_mem (CL_API_CALL *
	clCreateImage3D)(cl_context              /* context */,
					cl_mem_flags            /* flags */,
					const cl_image_format * /* image_format */,
					size_t                  /* image_width */,
					size_t                  /* image_height */,
					size_t                  /* image_depth */,
					size_t                  /* image_row_pitch */,
					size_t                  /* image_slice_pitch */,
					void *                  /* host_ptr */,
					cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainMemObject)(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseMemObject)(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetSupportedImageFormats)(cl_context           /* context */,
							   cl_mem_flags         /* flags */,
							   cl_mem_object_type   /* image_type */,
							   cl_uint              /* num_entries */,
							   cl_image_format *    /* image_formats */,
							   cl_uint *            /* num_image_formats */) CL_API_SUFFIX__VERSION_1_0;

/* #20 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetMemObjectInfo)(cl_mem           /* memobj */,
					   cl_mem_info      /* param_name */,
					   size_t           /* param_value_size */,
					   void *           /* param_value */,
					   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetImageInfo)(cl_mem           /* image */,
				   cl_image_info    /* param_name */,
				   size_t           /* param_value_size */,
				   void *           /* param_value */,
				   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #22 */
	/* Sampler APIs  */
	CL_API_ENTRY cl_sampler (CL_API_CALL *
	clCreateSampler)(cl_context          /* context */,
					cl_bool             /* normalized_coords */,
					cl_addressing_mode  /* addressing_mode */,
					cl_filter_mode      /* filter_mode */,
					cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainSampler)(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseSampler)(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetSamplerInfo)(cl_sampler         /* sampler */,
					 cl_sampler_info    /* param_name */,
					 size_t             /* param_value_size */,
					 void *             /* param_value */,
					 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #26 */
	/* Program Object APIs  */
	CL_API_ENTRY cl_program (CL_API_CALL *
	clCreateProgramWithSource)(cl_context        /* context */,
							  cl_uint           /* count */,
							  const char **     /* strings */,
							  const size_t *    /* lengths */,
							  cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_program (CL_API_CALL *
	clCreateProgramWithBinary)(cl_context                     /* context */,
							  cl_uint                        /* num_devices */,
							  const cl_device_id *           /* device_list */,
							  const size_t *                 /* lengths */,
							  const unsigned char **         /* binaries */,
							  cl_int *                       /* binary_status */,
							  cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainProgram)(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseProgram)(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clBuildProgram)(cl_program           /* program */,
				   cl_uint              /* num_devices */,
				   const cl_device_id * /* device_list */,
				   const char *         /* options */,
				   void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
				   void *               /* user_data */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clUnloadCompiler)(void) CL_API_SUFFIX__VERSION_1_0;

/* #32 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetProgramInfo)(cl_program         /* program */,
					 cl_program_info    /* param_name */,
					 size_t             /* param_value_size */,
					 void *             /* param_value */,
					 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetProgramBuildInfo)(cl_program            /* program */,
						  cl_device_id          /* device */,
						  cl_program_build_info /* param_name */,
						  size_t                /* param_value_size */,
						  void *                /* param_value */,
						  size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #34 */
	/* Kernel Object APIs */
	CL_API_ENTRY cl_kernel (CL_API_CALL *
	clCreateKernel)(cl_program      /* program */,
				   const char *    /* kernel_name */,
				   cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clCreateKernelsInProgram)(cl_program     /* program */,
							 cl_uint        /* num_kernels */,
							 cl_kernel *    /* kernels */,
							 cl_uint *      /* num_kernels_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainKernel)(cl_kernel    /* kernel */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseKernel)(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clSetKernelArg)(cl_kernel    /* kernel */,
				   cl_uint      /* arg_index */,
				   size_t       /* arg_size */,
				   const void * /* arg_value */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetKernelInfo)(cl_kernel       /* kernel */,
					cl_kernel_info  /* param_name */,
					size_t          /* param_value_size */,
					void *          /* param_value */,
					size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetKernelWorkGroupInfo)(cl_kernel                  /* kernel */,
							 cl_device_id               /* device */,
							 cl_kernel_work_group_info  /* param_name */,
							 size_t                     /* param_value_size */,
							 void *                     /* param_value */,
							 size_t *                   /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #41 */
	/* Event Object APIs  */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clWaitForEvents)(cl_uint             /* num_events */,
					const cl_event *    /* event_list */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetEventInfo)(cl_event         /* event */,
				   cl_event_info    /* param_name */,
				   size_t           /* param_value_size */,
				   void *           /* param_value */,
				   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainEvent)(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseEvent)(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;

	/* Profiling APIs  */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetEventProfilingInfo)(cl_event            /* event */,
							cl_profiling_info   /* param_name */,
							size_t              /* param_value_size */,
							void *              /* param_value */,
							size_t *            /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #46 */
	/* Flush and Finish APIs */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clFlush)(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clFinish)(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

/* #48 */
	/* Enqueued Commands APIs */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueReadBuffer)(cl_command_queue    /* command_queue */,
						cl_mem              /* buffer */,
						cl_bool             /* blocking_read */,
						size_t              /* offset */,
						size_t              /* cb */,
						void *              /* ptr */,
						cl_uint             /* num_events_in_wait_list */,
						const cl_event *    /* event_wait_list */,
						cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueWriteBuffer)(cl_command_queue   /* command_queue */,
						 cl_mem             /* buffer */,
						 cl_bool            /* blocking_write */,
						 size_t             /* offset */,
						 size_t             /* cb */,
						 const void *       /* ptr */,
						 cl_uint            /* num_events_in_wait_list */,
						 const cl_event *   /* event_wait_list */,
						 cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueCopyBuffer)(cl_command_queue    /* command_queue */,
						cl_mem              /* src_buffer */,
						cl_mem              /* dst_buffer */,
						size_t              /* src_offset */,
						size_t              /* dst_offset */,
						size_t              /* cb */,
						cl_uint             /* num_events_in_wait_list */,
						const cl_event *    /* event_wait_list */,
						cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueReadImage)(cl_command_queue     /* command_queue */,
					   cl_mem               /* image */,
					   cl_bool              /* blocking_read */,
					   const size_t *       /* origin[3] */,
					   const size_t *       /* region[3] */,
					   size_t               /* row_pitch */,
					   size_t               /* slice_pitch */,
					   void *               /* ptr */,
					   cl_uint              /* num_events_in_wait_list */,
					   const cl_event *     /* event_wait_list */,
					   cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueWriteImage)(cl_command_queue    /* command_queue */,
						cl_mem              /* image */,
						cl_bool             /* blocking_write */,
						const size_t *      /* origin[3] */,
						const size_t *      /* region[3] */,
						size_t              /* input_row_pitch */,
						size_t              /* input_slice_pitch */,
						const void *        /* ptr */,
						cl_uint             /* num_events_in_wait_list */,
						const cl_event *    /* event_wait_list */,
						cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

/* #53 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueCopyImage)(cl_command_queue     /* command_queue */,
					   cl_mem               /* src_image */,
					   cl_mem               /* dst_image */,
					   const size_t *       /* src_origin[3] */,
					   const size_t *       /* dst_origin[3] */,
					   const size_t *       /* region[3] */,
					   cl_uint              /* num_events_in_wait_list */,
					   const cl_event *     /* event_wait_list */,
					   cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueCopyImageToBuffer)(cl_command_queue /* command_queue */,
							   cl_mem           /* src_image */,
							   cl_mem           /* dst_buffer */,
							   const size_t *   /* src_origin[3] */,
							   const size_t *   /* region[3] */,
							   size_t           /* dst_offset */,
							   cl_uint          /* num_events_in_wait_list */,
							   const cl_event * /* event_wait_list */,
							   cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueCopyBufferToImage)(cl_command_queue /* command_queue */,
							   cl_mem           /* src_buffer */,
							   cl_mem           /* dst_image */,
							   size_t           /* src_offset */,
							   const size_t *   /* dst_origin[3] */,
							   const size_t *   /* region[3] */,
							   cl_uint          /* num_events_in_wait_list */,
							   const cl_event * /* event_wait_list */,
							   cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY void * (CL_API_CALL *
	clEnqueueMapBuffer)(cl_command_queue /* command_queue */,
					   cl_mem           /* buffer */,
					   cl_bool          /* blocking_map */,
					   cl_map_flags     /* map_flags */,
					   size_t           /* offset */,
					   size_t           /* cb */,
					   cl_uint          /* num_events_in_wait_list */,
					   const cl_event * /* event_wait_list */,
					   cl_event *       /* event */,
					   cl_int *         /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY void * (CL_API_CALL *
	clEnqueueMapImage)(cl_command_queue  /* command_queue */,
					  cl_mem            /* image */,
					  cl_bool           /* blocking_map */,
					  cl_map_flags      /* map_flags */,
					  const size_t *    /* origin[3] */,
					  const size_t *    /* region[3] */,
					  size_t *          /* image_row_pitch */,
					  size_t *          /* image_slice_pitch */,
					  cl_uint           /* num_events_in_wait_list */,
					  const cl_event *  /* event_wait_list */,
					  cl_event *        /* event */,
					  cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

/* #58 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueUnmapMemObject)(cl_command_queue /* command_queue */,
							cl_mem           /* memobj */,
							void *           /* mapped_ptr */,
							cl_uint          /* num_events_in_wait_list */,
							const cl_event *  /* event_wait_list */,
							cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueNDRangeKernel)(cl_command_queue /* command_queue */,
						   cl_kernel        /* kernel */,
						   cl_uint          /* work_dim */,
						   const size_t *   /* global_work_offset */,
						   const size_t *   /* global_work_size */,
						   const size_t *   /* local_work_size */,
						   cl_uint          /* num_events_in_wait_list */,
						   const cl_event * /* event_wait_list */,
						   cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueTask)(cl_command_queue  /* command_queue */,
				  cl_kernel         /* kernel */,
				  cl_uint           /* num_events_in_wait_list */,
				  const cl_event *  /* event_wait_list */,
				  cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueNativeKernel)(cl_command_queue  /* command_queue */,
						  void (*user_func)(void *),
						  void *            /* args */,
						  size_t            /* cb_args */,
						  cl_uint           /* num_mem_objects */,
						  const cl_mem *    /* mem_list */,
						  const void **     /* args_mem_loc */,
						  cl_uint           /* num_events_in_wait_list */,
						  const cl_event *  /* event_wait_list */,
						  cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueMarker)(cl_command_queue    /* command_queue */,
					cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

/* #63 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueWaitForEvents)(cl_command_queue /* command_queue */,
						   cl_uint          /* num_events */,
						   const cl_event * /* event_list */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueBarrier)(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	_stub		empty_after_1_0_API[16];

/* #81 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clSetEventCallback)( cl_event    /* event */,
						cl_int      /* command_exec_callback_type */,
						void (CL_CALLBACK * /* pfn_notify */)(cl_event, cl_int, void *),
						void *      /* user_data */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_mem (CL_API_CALL *
	clCreateSubBuffer)(cl_mem                   /* buffer */,
					  cl_mem_flags             /* flags */,
					  cl_buffer_create_type    /* buffer_create_type */,
					  const void *             /* buffer_create_info */,
					  cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clSetMemObjectDestructorCallback)(  cl_mem /* memobj */,
										void (CL_CALLBACK * /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/),
										void * /*user_data */ )             CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_event (CL_API_CALL *
	clCreateUserEvent)(cl_context    /* context */,
					  cl_int *      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clSetUserEventStatus)(cl_event   /* event */,
						 cl_int     /* execution_status */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueReadBufferRect)(cl_command_queue    /* command_queue */,
							cl_mem              /* buffer */,
							cl_bool             /* blocking_read */,
							const size_t *      /* buffer_origin */,
							const size_t *      /* host_origin */,
							const size_t *      /* region */,
							size_t              /* buffer_row_pitch */,
							size_t              /* buffer_slice_pitch */,
							size_t              /* host_row_pitch */,
							size_t              /* host_slice_pitch */,
							void *              /* ptr */,
							cl_uint             /* num_events_in_wait_list */,
							const cl_event *    /* event_wait_list */,
							cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueWriteBufferRect)(cl_command_queue    /* command_queue */,
							 cl_mem              /* buffer */,
							 cl_bool             /* blocking_write */,
							 const size_t *      /* buffer_origin */,
							 const size_t *      /* host_origin */,
							 const size_t *      /* region */,
							 size_t              /* buffer_row_pitch */,
							 size_t              /* buffer_slice_pitch */,
							 size_t              /* host_row_pitch */,
							 size_t              /* host_slice_pitch */,
							 const void *        /* ptr */,
							 cl_uint             /* num_events_in_wait_list */,
							 const cl_event *    /* event_wait_list */,
							 cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueCopyBufferRect)(cl_command_queue    /* command_queue */,
							cl_mem              /* src_buffer */,
							cl_mem              /* dst_buffer */,
							const size_t *      /* src_origin */,
							const size_t *      /* dst_origin */,
							const size_t *      /* region */,
							size_t              /* src_row_pitch */,
							size_t              /* src_slice_pitch */,
							size_t              /* dst_row_pitch */,
							size_t              /* dst_slice_pitch */,
							cl_uint             /* num_events_in_wait_list */,
							const cl_event *    /* event_wait_list */,
							cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_1;

	_stub		empty_after_1_1_API[72];
/* #161 */
	CL_API_ENTRY cl_int (CL_API_CALL *
	clCreateSubDevices)(cl_device_id                         /* in_device */,
					   const cl_device_partition_property * /* properties */,
					   cl_uint                              /* num_devices */,
					   cl_device_id *                       /* out_devices */,
					   cl_uint *                            /* num_devices_ret */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clRetainDevice)(cl_device_id /* device */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clReleaseDevice)(cl_device_id /* device */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_mem (CL_API_CALL *
	clCreateImage)(cl_context              /* context */,
				  cl_mem_flags            /* flags */,
				  const cl_image_format * /* image_format */,
				  const cl_image_desc *   /* image_desc */,
				  void *                  /* host_ptr */,
				  cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_program (CL_API_CALL *
	clCreateProgramWithBuiltInKernels)(cl_context            /* context */,
									  cl_uint               /* num_devices */,
									  const cl_device_id *  /* device_list */,
									  const char *          /* kernel_names */,
									  cl_int *              /* errcode_ret */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clCompileProgram)(cl_program           /* program */,
					 cl_uint              /* num_devices */,
					 const cl_device_id * /* device_list */,
					 const char *         /* options */,
					 cl_uint              /* num_input_headers */,
					 const cl_program *   /* input_headers */,
					 const char **        /* header_include_names */,
					 void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
					 void *               /* user_data */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_program (CL_API_CALL *
	clLinkProgram)(cl_context           /* context */,
				  cl_uint              /* num_devices */,
				  const cl_device_id * /* device_list */,
				  const char *         /* options */,
				  cl_uint              /* num_input_programs */,
				  const cl_program *   /* input_programs */,
				  void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
				  void *               /* user_data */,
				  cl_int *             /* errcode_ret */ ) CL_API_SUFFIX__VERSION_1_2;


	CL_API_ENTRY cl_int (CL_API_CALL *
	clUnloadPlatformCompiler)(cl_platform_id /* platform */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clGetKernelArgInfo)(cl_kernel       /* kernel */,
					   cl_uint         /* arg_indx */,
					   cl_kernel_arg_info  /* param_name */,
					   size_t          /* param_value_size */,
					   void *          /* param_value */,
					   size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueFillBuffer)(cl_command_queue   /* command_queue */,
						cl_mem             /* buffer */,
						const void *       /* pattern */,
						size_t             /* pattern_size */,
						size_t             /* offset */,
						size_t             /* size */,
						cl_uint            /* num_events_in_wait_list */,
						const cl_event *   /* event_wait_list */,
						cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueFillImage)(cl_command_queue   /* command_queue */,
					   cl_mem             /* image */,
					   const void *       /* fill_color */,
					   const size_t *     /* origin[3] */,
					   const size_t *     /* region[3] */,
					   cl_uint            /* num_events_in_wait_list */,
					   const cl_event *   /* event_wait_list */,
					   cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueMigrateMemObjects)(cl_command_queue       /* command_queue */,
							   cl_uint                /* num_mem_objects */,
							   const cl_mem *         /* mem_objects */,
							   cl_mem_migration_flags /* flags */,
							   cl_uint                /* num_events_in_wait_list */,
							   const cl_event *       /* event_wait_list */,
							   cl_event *             /* event */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueMarkerWithWaitList)(cl_command_queue /* command_queue */,
								cl_uint           /* num_events_in_wait_list */,
								const cl_event *  /* event_wait_list */,
								cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clEnqueueBarrierWithWaitList)(cl_command_queue /* command_queue */,
								 cl_uint           /* num_events_in_wait_list */,
								 const cl_event *  /* event_wait_list */,
								 cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_2;

	CL_API_ENTRY cl_int (CL_API_CALL *
	clSetPrintfCallback)(cl_context          /* context */,
						void (CL_CALLBACK * /* pfn_notify */)(cl_context /* program */,
															  cl_uint /*printf_data_len */,
															  char * /* printf_data_ptr */,
															  void * /* user_data */),
						void *              /* user_data */) CL_API_SUFFIX__VERSION_1_2;



	/* Extension function access
	 *
	 * Returns the extension function address for the given function name,
	 * or NULL if a valid function can not be found.  The client must
	 * check to make sure the address is not NULL, before using or
	 * calling the returned function address.
	 */
	CL_API_ENTRY void * (CL_API_CALL *
	clGetExtensionFunctionAddressForPlatform)(cl_platform_id /* platform */,
											 const char *   /* func_name */) CL_API_SUFFIX__VERSION_1_2;

	_stub		empty_after_1_2_API[64];
};

#endif
