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
#ifndef __FREEOCL_PROTOTYPES_H__
#define __FREEOCL_PROTOTYPES_H__

#include "freeocl.h"

extern "C"
{

	/* Platform API */
	CL_API_ENTRY cl_int CL_API_CALL
	clGetPlatformIDsFCL(cl_uint          /* num_entries */,
					 cl_platform_id * /* platforms */,
					 cl_uint *        /* num_platforms */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetPlatformInfoFCL(cl_platform_id   /* platform */,
					  cl_platform_info /* param_name */,
					  size_t           /* param_value_size */,
					  void *           /* param_value */,
					  size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Device APIs */
	CL_API_ENTRY cl_int CL_API_CALL
	clGetDeviceIDsFCL(cl_platform_id   /* platform */,
				   cl_device_type   /* device_type */,
				   cl_uint          /* num_entries */,
				   cl_device_id *   /* devices */,
				   cl_uint *        /* num_devices */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetDeviceInfoFCL(cl_device_id    /* device */,
					cl_device_info  /* param_name */,
					size_t          /* param_value_size */,
					void *          /* param_value */,
					size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Context APIs  */
	CL_API_ENTRY cl_context CL_API_CALL
	clCreateContextFCL(const cl_context_properties * /* properties */,
					cl_uint                       /* num_devices */,
					const cl_device_id *          /* devices */,
					void (CL_CALLBACK * /* pfn_notify */)(const char *, const void *, size_t, void *),
					void *                        /* user_data */,
					cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_context CL_API_CALL
	clCreateContextFromTypeFCL(const cl_context_properties * /* properties */,
							cl_device_type                /* device_type */,
							void (CL_CALLBACK *     /* pfn_notify*/ )(const char *, const void *, size_t, void *),
							void *                        /* user_data */,
							cl_int *                      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clRetainContextFCL(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clReleaseContextFCL(cl_context /* context */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetContextInfoFCL(cl_context         /* context */,
					 cl_context_info    /* param_name */,
					 size_t             /* param_value_size */,
					 void *             /* param_value */,
					 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Command Queue APIs */
	CL_API_ENTRY cl_command_queue CL_API_CALL
	clCreateCommandQueueFCL(cl_context                     /* context */,
						 cl_device_id                   /* device */,
						 cl_command_queue_properties    /* properties */,
						 cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clRetainCommandQueueFCL(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clReleaseCommandQueueFCL(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetCommandQueueInfoFCL(cl_command_queue      /* command_queue */,
						  cl_command_queue_info /* param_name */,
						  size_t                /* param_value_size */,
						  void *                /* param_value */,
						  size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Memory Object APIs */
	CL_API_ENTRY cl_mem CL_API_CALL
	clCreateBufferFCL(cl_context   /* context */,
				   cl_mem_flags /* flags */,
				   size_t       /* size */,
				   void *       /* host_ptr */,
				   cl_int *     /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_mem CL_API_CALL
	clCreateSubBufferFCL(cl_mem                   /* buffer */,
					  cl_mem_flags             /* flags */,
					  cl_buffer_create_type    /* buffer_create_type */,
					  const void *             /* buffer_create_info */,
					  cl_int *                 /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_mem CL_API_CALL
	clCreateImage2DFCL(cl_context              /* context */,
					cl_mem_flags            /* flags */,
					const cl_image_format * /* image_format */,
					size_t                  /* image_width */,
					size_t                  /* image_height */,
					size_t                  /* image_row_pitch */,
					void *                  /* host_ptr */,
					cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_mem CL_API_CALL
	clCreateImage3DFCL(cl_context              /* context */,
					cl_mem_flags            /* flags */,
					const cl_image_format * /* image_format */,
					size_t                  /* image_width */,
					size_t                  /* image_height */,
					size_t                  /* image_depth */,
					size_t                  /* image_row_pitch */,
					size_t                  /* image_slice_pitch */,
					void *                  /* host_ptr */,
					cl_int *                /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clRetainMemObjectFCL(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clReleaseMemObjectFCL(cl_mem /* memobj */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetSupportedImageFormatsFCL(cl_context           /* context */,
							   cl_mem_flags         /* flags */,
							   cl_mem_object_type   /* image_type */,
							   cl_uint              /* num_entries */,
							   cl_image_format *    /* image_formats */,
							   cl_uint *            /* num_image_formats */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetMemObjectInfoFCL(cl_mem           /* memobj */,
					   cl_mem_info      /* param_name */,
					   size_t           /* param_value_size */,
					   void *           /* param_value */,
					   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetImageInfoFCL(cl_mem           /* image */,
				   cl_image_info    /* param_name */,
				   size_t           /* param_value_size */,
				   void *           /* param_value */,
				   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clSetMemObjectDestructorCallbackFCL(  cl_mem /* memobj */,
										void (CL_CALLBACK * /*pfn_notify*/)( cl_mem /* memobj */, void* /*user_data*/),
										void * /*user_data */ )             CL_API_SUFFIX__VERSION_1_1;

	/* Sampler APIs  */
	CL_API_ENTRY cl_sampler CL_API_CALL
	clCreateSamplerFCL(cl_context          /* context */,
					cl_bool             /* normalized_coords */,
					cl_addressing_mode  /* addressing_mode */,
					cl_filter_mode      /* filter_mode */,
					cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clRetainSamplerFCL(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clReleaseSamplerFCL(cl_sampler /* sampler */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetSamplerInfoFCL(cl_sampler         /* sampler */,
					 cl_sampler_info    /* param_name */,
					 size_t             /* param_value_size */,
					 void *             /* param_value */,
					 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Program Object APIs  */
	CL_API_ENTRY cl_program CL_API_CALL
	clCreateProgramWithSourceFCL(cl_context        /* context */,
							  cl_uint           /* count */,
							  const char **     /* strings */,
							  const size_t *    /* lengths */,
							  cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_program CL_API_CALL
	clCreateProgramWithBinaryFCL(cl_context                     /* context */,
							  cl_uint                        /* num_devices */,
							  const cl_device_id *           /* device_list */,
							  const size_t *                 /* lengths */,
							  const unsigned char **         /* binaries */,
							  cl_int *                       /* binary_status */,
							  cl_int *                       /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clRetainProgramFCL(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clReleaseProgramFCL(cl_program /* program */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clBuildProgramFCL(cl_program           /* program */,
				   cl_uint              /* num_devices */,
				   const cl_device_id * /* device_list */,
				   const char *         /* options */,
				   void (CL_CALLBACK *  /* pfn_notify */)(cl_program /* program */, void * /* user_data */),
				   void *               /* user_data */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetProgramInfoFCL(cl_program         /* program */,
					 cl_program_info    /* param_name */,
					 size_t             /* param_value_size */,
					 void *             /* param_value */,
					 size_t *           /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetProgramBuildInfoFCL(cl_program            /* program */,
						  cl_device_id          /* device */,
						  cl_program_build_info /* param_name */,
						  size_t                /* param_value_size */,
						  void *                /* param_value */,
						  size_t *              /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Kernel Object APIs */
	CL_API_ENTRY cl_kernel CL_API_CALL
	clCreateKernelFCL(cl_program      /* program */,
				   const char *    /* kernel_name */,
				   cl_int *        /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clCreateKernelsInProgramFCL(cl_program     /* program */,
							 cl_uint        /* num_kernels */,
							 cl_kernel *    /* kernels */,
							 cl_uint *      /* num_kernels_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clRetainKernelFCL(cl_kernel    /* kernel */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clReleaseKernelFCL(cl_kernel   /* kernel */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clSetKernelArgFCL(cl_kernel    /* kernel */,
				   cl_uint      /* arg_index */,
				   size_t       /* arg_size */,
				   const void * /* arg_value */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetKernelInfoFCL(cl_kernel       /* kernel */,
					cl_kernel_info  /* param_name */,
					size_t          /* param_value_size */,
					void *          /* param_value */,
					size_t *        /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetKernelWorkGroupInfoFCL(cl_kernel                  /* kernel */,
							 cl_device_id               /* device */,
							 cl_kernel_work_group_info  /* param_name */,
							 size_t                     /* param_value_size */,
							 void *                     /* param_value */,
							 size_t *                   /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Event Object APIs  */
	CL_API_ENTRY cl_int CL_API_CALL
	clWaitForEventsFCL(cl_uint             /* num_events */,
					const cl_event *    /* event_list */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clGetEventInfoFCL(cl_event         /* event */,
				   cl_event_info    /* param_name */,
				   size_t           /* param_value_size */,
				   void *           /* param_value */,
				   size_t *         /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_event CL_API_CALL
	clCreateUserEventFCL(cl_context    /* context */,
					  cl_int *      /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_int CL_API_CALL
	clRetainEventFCL(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clReleaseEventFCL(cl_event /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clSetUserEventStatusFCL(cl_event   /* event */,
						 cl_int     /* execution_status */) CL_API_SUFFIX__VERSION_1_1;

	CL_API_ENTRY cl_int CL_API_CALL
	clSetEventCallbackFCL( cl_event    /* event */,
						cl_int      /* command_exec_callback_type */,
						void (CL_CALLBACK * /* pfn_notify */)(cl_event, cl_int, void *),
						void *      /* user_data */) CL_API_SUFFIX__VERSION_1_1;

	/* Profiling APIs  */
	CL_API_ENTRY cl_int CL_API_CALL
	clGetEventProfilingInfoFCL(cl_event            /* event */,
							cl_profiling_info   /* param_name */,
							size_t              /* param_value_size */,
							void *              /* param_value */,
							size_t *            /* param_value_size_ret */) CL_API_SUFFIX__VERSION_1_0;

	/* Flush and Finish APIs */
	CL_API_ENTRY cl_int CL_API_CALL
	clFlushFCL(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clFinishFCL(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	/* Enqueued Commands APIs */
	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueReadBufferFCL(cl_command_queue    /* command_queue */,
						cl_mem              /* buffer */,
						cl_bool             /* blocking_read */,
						size_t              /* offset */,
						size_t              /* cb */,
						void *              /* ptr */,
						cl_uint             /* num_events_in_wait_list */,
						const cl_event *    /* event_wait_list */,
						cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueReadBufferRectFCL(cl_command_queue    /* command_queue */,
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

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueWriteBufferFCL(cl_command_queue   /* command_queue */,
						 cl_mem             /* buffer */,
						 cl_bool            /* blocking_write */,
						 size_t             /* offset */,
						 size_t             /* cb */,
						 const void *       /* ptr */,
						 cl_uint            /* num_events_in_wait_list */,
						 const cl_event *   /* event_wait_list */,
						 cl_event *         /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueWriteBufferRectFCL(cl_command_queue    /* command_queue */,
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

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueCopyBufferFCL(cl_command_queue    /* command_queue */,
						cl_mem              /* src_buffer */,
						cl_mem              /* dst_buffer */,
						size_t              /* src_offset */,
						size_t              /* dst_offset */,
						size_t              /* cb */,
						cl_uint             /* num_events_in_wait_list */,
						const cl_event *    /* event_wait_list */,
						cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueCopyBufferRectFCL(cl_command_queue    /* command_queue */,
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

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueReadImageFCL(cl_command_queue     /* command_queue */,
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

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueWriteImageFCL(cl_command_queue    /* command_queue */,
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

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueCopyImageFCL(cl_command_queue     /* command_queue */,
					   cl_mem               /* src_image */,
					   cl_mem               /* dst_image */,
					   const size_t *       /* src_origin[3] */,
					   const size_t *       /* dst_origin[3] */,
					   const size_t *       /* region[3] */,
					   cl_uint              /* num_events_in_wait_list */,
					   const cl_event *     /* event_wait_list */,
					   cl_event *           /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueCopyImageToBufferFCL(cl_command_queue /* command_queue */,
							   cl_mem           /* src_image */,
							   cl_mem           /* dst_buffer */,
							   const size_t *   /* src_origin[3] */,
							   const size_t *   /* region[3] */,
							   size_t           /* dst_offset */,
							   cl_uint          /* num_events_in_wait_list */,
							   const cl_event * /* event_wait_list */,
							   cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueCopyBufferToImageFCL(cl_command_queue /* command_queue */,
							   cl_mem           /* src_buffer */,
							   cl_mem           /* dst_image */,
							   size_t           /* src_offset */,
							   const size_t *   /* dst_origin[3] */,
							   const size_t *   /* region[3] */,
							   cl_uint          /* num_events_in_wait_list */,
							   const cl_event * /* event_wait_list */,
							   cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY void * CL_API_CALL
	clEnqueueMapBufferFCL(cl_command_queue /* command_queue */,
					   cl_mem           /* buffer */,
					   cl_bool          /* blocking_map */,
					   cl_map_flags     /* map_flags */,
					   size_t           /* offset */,
					   size_t           /* cb */,
					   cl_uint          /* num_events_in_wait_list */,
					   const cl_event * /* event_wait_list */,
					   cl_event *       /* event */,
					   cl_int *         /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY void * CL_API_CALL
	clEnqueueMapImageFCL(cl_command_queue  /* command_queue */,
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

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueUnmapMemObjectFCL(cl_command_queue /* command_queue */,
							cl_mem           /* memobj */,
							void *           /* mapped_ptr */,
							cl_uint          /* num_events_in_wait_list */,
							const cl_event *  /* event_wait_list */,
							cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueNDRangeKernelFCL(cl_command_queue /* command_queue */,
						   cl_kernel        /* kernel */,
						   cl_uint          /* work_dim */,
						   const size_t *   /* global_work_offset */,
						   const size_t *   /* global_work_size */,
						   const size_t *   /* local_work_size */,
						   cl_uint          /* num_events_in_wait_list */,
						   const cl_event * /* event_wait_list */,
						   cl_event *       /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueTaskFCL(cl_command_queue  /* command_queue */,
				  cl_kernel         /* kernel */,
				  cl_uint           /* num_events_in_wait_list */,
				  const cl_event *  /* event_wait_list */,
				  cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueNativeKernelFCL(cl_command_queue  /* command_queue */,
						  void (*user_func)(void *),
						  void *            /* args */,
						  size_t            /* cb_args */,
						  cl_uint           /* num_mem_objects */,
						  const cl_mem *    /* mem_list */,
						  const void **     /* args_mem_loc */,
						  cl_uint           /* num_events_in_wait_list */,
						  const cl_event *  /* event_wait_list */,
						  cl_event *        /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueMarkerFCL(cl_command_queue    /* command_queue */,
					cl_event *          /* event */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueWaitForEventsFCL(cl_command_queue /* command_queue */,
						   cl_uint          /* num_events */,
						   const cl_event * /* event_list */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueBarrierFCL(cl_command_queue /* command_queue */) CL_API_SUFFIX__VERSION_1_0;

	CL_API_ENTRY cl_int CL_API_CALL clIcdGetPlatformIDsKHR (cl_uint /*num_entries*/,
															cl_platform_id */*platforms*/,
															cl_uint */*num_platforms*/) CL_API_SUFFIX__VERSION_1_1;
}

#endif
