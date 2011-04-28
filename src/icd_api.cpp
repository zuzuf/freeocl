/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#include "freeocl.h"
#include "context.h"
#include "commandqueue.h"
#include "sampler.h"
#include "mem.h"
#include "device.h"
#include "program.h"
#include "kernel.h"
#include "platform.h"
#include "event.h"

#define SET_RET(X)	if (errcode_ret)	*errcode_ret = (X)
extern "C"
{
	cl_command_queue clCreateCommandQueue (cl_context context,
										   cl_device_id device,
										   cl_command_queue_properties properties,
										   cl_int *errcode_ret)
	{
		return context->dispatch->clCreateCommandQueue(context,
													   device,
													   properties,
													   errcode_ret);
	}

	cl_int clRetainCommandQueue (cl_command_queue command_queue)
	{
		return command_queue->dispatch->clRetainCommandQueue(command_queue);
	}

	cl_int clReleaseCommandQueue (cl_command_queue command_queue)
	{
		return command_queue->dispatch->clReleaseCommandQueue(command_queue);
	}

	cl_int clGetCommandQueueInfo (cl_command_queue command_queue,
								  cl_command_queue_info param_name,
								  size_t param_value_size,
								  void *param_value,
								  size_t *param_value_size_ret)
	{
		return command_queue->dispatch->clGetCommandQueueInfo(command_queue,
															  param_name,
															  param_value_size,
															  param_value,
															  param_value_size_ret);
	}

	cl_int clFlush (cl_command_queue command_queue)
	{
		return command_queue->dispatch->clFlush(command_queue);
	}

	cl_int clFinish (cl_command_queue command_queue)
	{
		return command_queue->dispatch->clFinish(command_queue);
	}

	cl_context clCreateContext (const cl_context_properties *properties,
								cl_uint num_devices,
								const cl_device_id *devices,
								void (CL_CALLBACK *pfn_notify)(const char *errinfo,
															   const void *private_info,
															   size_t cb,
															   void *user_data),
								void *user_data,
								cl_int *errcode_ret)
	{
		if (num_devices == 0)
		{
			SET_RET(CL_INVALID_VALUE);
			return 0;
		}
		return (*devices)->dispatch->clCreateContext(properties,
													 num_devices,
													 devices,
													 pfn_notify,
													 user_data,
													 errcode_ret);
	}

	cl_int clRetainContext (cl_context context)
	{
		return context->dispatch->clRetainContext(context);
	}

	cl_int clReleaseContext (cl_context context)
	{
		return context->dispatch->clReleaseContext(context);
	}

	cl_int clGetContextInfo (cl_context context,
							 cl_context_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		return context->dispatch->clGetContextInfo(context,
												   param_name,
												   param_value_size,
												   param_value,
												   param_value_size_ret);
	}

	cl_int clGetDeviceInfo (cl_device_id device,
							cl_device_info param_name,
							size_t param_value_size,
							void *param_value,
							size_t *param_value_size_ret)
	{
		return device->dispatch->clGetDeviceInfo(device,
												 param_name,
												 param_value_size,
												 param_value,
												 param_value_size_ret);
	}

	cl_int clGetDeviceIDs (cl_platform_id platform,
						   cl_device_type device_type,
						   cl_uint num_entries,
						   cl_device_id *devices,
						   cl_uint *num_devices)
	{
		return platform->dispatch->clGetDeviceIDs(platform,
												  device_type,
												  num_entries,
												  devices,
												  num_devices);
	}

	cl_event clCreateUserEvent (cl_context context, cl_int *errcode_ret)
	{
		return context->dispatch->clCreateUserEvent(context,
													errcode_ret);
	}

	cl_int clSetUserEventStatus (cl_event event, cl_int execution_status)
	{
		return event->dispatch->clSetUserEventStatus(event,
													 execution_status);
	}

	cl_int clWaitForEvents (cl_uint num_events, const cl_event *event_list)
	{
		if (num_events == 0 || event_list == NULL)
			return CL_INVALID_VALUE;
		return event_list[0]->dispatch->clWaitForEvents(num_events, event_list);
	}

	cl_int clGetEventInfo (cl_event event,
						   cl_event_info param_name,
						   size_t param_value_size,
						   void *param_value,
						   size_t *param_value_size_ret)
	{
		return event->dispatch->clGetEventInfo(event,
											   param_name,
											   param_value_size,
											   param_value,
											   param_value_size_ret);
	}

	cl_int clSetEventCallback (cl_event event,
							   cl_int command_exec_callback_type,
							   void (CL_CALLBACK *pfn_event_notify)(cl_event event,
																	cl_int event_command_exec_status,
																	void *user_data),
							   void *user_data)
	{
		return event->dispatch->clSetEventCallback(event,
												   command_exec_callback_type,
												   pfn_event_notify,
												   user_data);
	}

	cl_int clRetainEvent (cl_event event)
	{
		return event->dispatch->clRetainEvent(event);
	}

	cl_int clReleaseEvent (cl_event event)
	{
		return event->dispatch->clReleaseEvent(event);
	}

	cl_int clEnqueueMarker (cl_command_queue command_queue,
							cl_event *event)
	{
		return command_queue->dispatch->clEnqueueMarker(command_queue,
														event);
	}

	cl_int clEnqueueBarrier (cl_command_queue command_queue)
	{
		return command_queue->dispatch->clEnqueueBarrier(command_queue);
	}

	cl_int clEnqueueWaitForEvents (cl_command_queue command_queue,
								   cl_uint num_events,
								   const cl_event *event_list)
	{
		return command_queue->dispatch->clEnqueueWaitForEvents(command_queue,
															   num_events,
															   event_list);
	}

	cl_int clGetEventProfilingInfo (cl_event event,
									cl_profiling_info param_name,
									size_t param_value_size,
									void *param_value,
									size_t *param_value_size_ret)
	{
		return event->dispatch->clGetEventProfilingInfo(event,
														param_name,
														param_value_size,
														param_value,
														param_value_size_ret);
	}

	cl_mem clCreateImage2D (cl_context context,
							cl_mem_flags flags,
							const cl_image_format *image_format,
							size_t image_width,
							size_t image_height,
							size_t image_row_pitch,
							void *host_ptr,
							cl_int *errcode_ret)
	{
		return context->dispatch->clCreateImage2D(context,
												  flags,
												  image_format,
												  image_width,
												  image_height,
												  image_row_pitch,
												  host_ptr,
												  errcode_ret);
	}

	cl_mem clCreateImage3D (cl_context context,
							cl_mem_flags flags,
							const cl_image_format *image_format,
							size_t image_width,
							size_t image_height,
							size_t image_depth,
							size_t image_row_pitch,
							size_t image_slice_pitch,
							void *host_ptr,
							cl_int *errcode_ret)
	{
		return context->dispatch->clCreateImage3D(context,
												  flags,
												  image_format,
												  image_width,
												  image_height,
												  image_depth,
												  image_row_pitch,
												  image_slice_pitch,
												  host_ptr,
												  errcode_ret);
	}

	cl_int clGetSupportedImageFormats (cl_context context,
									   cl_mem_flags flags,
									   cl_mem_object_type image_type,
									   cl_uint num_entries,
									   cl_image_format *image_formats,
									   cl_uint *num_image_formats)
	{
		return context->dispatch->clGetSupportedImageFormats(context,
															 flags,
															 image_type,
															 num_entries,
															 image_formats,
															 num_image_formats);
	}

	cl_int clEnqueueReadImage (cl_command_queue command_queue,
							   cl_mem image,
							   cl_bool blocking_read,
							   const size_t origin[3],
							   const size_t region[3],
							   size_t row_pitch,
							   size_t slice_pitch,
							   void *ptr,
							   cl_uint num_events_in_wait_list,
							   const cl_event *event_wait_list,
							   cl_event *event)
	{
		return command_queue->dispatch->clEnqueueReadImage(command_queue,
														   image,
														   blocking_read,
														   origin,
														   region,
														   row_pitch,
														   slice_pitch,
														   ptr,
														   num_events_in_wait_list,
														   event_wait_list,
														   event);
	}

	cl_int clEnqueueWriteImage (cl_command_queue command_queue,
								cl_mem image,
								cl_bool blocking_write,
								const size_t origin[3],
								const size_t region[3],
								size_t input_row_pitch,
								size_t input_slice_pitch,
								const void * ptr,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		return command_queue->dispatch->clEnqueueWriteImage(command_queue,
															image,
															blocking_write,
															origin,
															region,
															input_row_pitch,
															input_slice_pitch,
															ptr,
															num_events_in_wait_list,
															event_wait_list,
															event);
	}

	cl_int clEnqueueCopyImage (cl_command_queue command_queue,
							   cl_mem src_image,
							   cl_mem dst_image,
							   const size_t src_origin[3],
							   const size_t dst_origin[3],
							   const size_t region[3],
							   cl_uint num_events_in_wait_list,
							   const cl_event *event_wait_list,
							   cl_event *event)
	{
		return command_queue->dispatch->clEnqueueCopyImage(command_queue,
														   src_image,
														   dst_image,
														   src_origin,
														   dst_origin,
														   region,
														   num_events_in_wait_list,
														   event_wait_list,
														   event);
	}

	cl_int clEnqueueCopyImageToBuffer (cl_command_queue command_queue,
									   cl_mem src_image,
									   cl_mem dst_buffer,
									   const size_t src_origin[3],
									   const size_t region[3],
									   size_t dst_offset,
									   cl_uint num_events_in_wait_list,
									   const cl_event *event_wait_list,
									   cl_event *event)
	{
		return command_queue->dispatch->clEnqueueCopyImageToBuffer(command_queue,
																   src_image,
																   dst_buffer,
																   src_origin,
																   region,
																   dst_offset,
																   num_events_in_wait_list,
																   event_wait_list,
																   event);
	}

	cl_int clEnqueueCopyBufferToImage (cl_command_queue command_queue,
									   cl_mem src_buffer,
									   cl_mem dst_image,
									   size_t src_offset,
									   const size_t dst_origin[3],
									   const size_t region[3],
									   cl_uint num_events_in_wait_list,
									   const cl_event *event_wait_list,
									   cl_event *event)
	{
		return command_queue->dispatch->clEnqueueCopyBufferToImage(command_queue,
																   src_buffer,
																   dst_image,
																   src_offset,
																   dst_origin,
																   region,
																   num_events_in_wait_list,
																   event_wait_list,
																   event);
	}

	void *clEnqueueMapImage (cl_command_queue command_queue,
							  cl_mem image,
							  cl_bool blocking_map,
							  cl_map_flags map_flags,
							  const size_t origin[3],
							  const size_t region[3],
							  size_t *image_row_pitch,
							  size_t *image_slice_pitch,
							  cl_uint num_events_in_wait_list,
							  const cl_event *event_wait_list,
							  cl_event *event,
							  cl_int *errcode_ret)
	{
		return command_queue->dispatch->clEnqueueMapImage(command_queue,
														  image,
														  blocking_map,
														  map_flags,
														  origin,
														  region,
														  image_row_pitch,
														  image_slice_pitch,
														  num_events_in_wait_list,
														  event_wait_list,
														  event,
														  errcode_ret);
	}

	cl_int clGetImageInfo (cl_mem image,
						   cl_image_info param_name,
						   size_t param_value_size,
						   void *param_value,
						   size_t *param_value_size_ret)
	{
		return image->dispatch->clGetImageInfo(image,
											   param_name,
											   param_value_size,
											   param_value,
											   param_value_size_ret);
	}

	cl_int clEnqueueNativeKernel (cl_command_queue command_queue,
								  void (*user_func)(void *),
								  void *args,
								  size_t cb_args,
								  cl_uint num_mem_objects,
								  const cl_mem *mem_list,
								  const void **args_mem_loc,
								  cl_uint num_events_in_wait_list,
								  const cl_event *event_wait_list,
								  cl_event *event)
	{
		return command_queue->dispatch->clEnqueueNativeKernel(command_queue,
															  user_func,
															  args,
															  cb_args,
															  num_mem_objects,
															  mem_list,
															  args_mem_loc,
															  num_events_in_wait_list,
															  event_wait_list,
															  event);
	}

	cl_kernel clCreateKernel (cl_program program,
							  const char *kernel_name,
							  cl_int *errcode_ret)
	{
		return program->dispatch->clCreateKernel(program,
												 kernel_name,
												 errcode_ret);
	}

	cl_int clCreateKernelsInProgram (cl_program program,
									 cl_uint num_kernels,
									 cl_kernel *kernels,
									 cl_uint *num_kernels_ret)
	{
		return program->dispatch->clCreateKernelsInProgram(program,
														   num_kernels,
														   kernels,
														   num_kernels_ret);
	}

	cl_int clRetainKernel (cl_kernel kernel)
	{
		return kernel->dispatch->clRetainKernel(kernel);
	}

	cl_int clReleaseKernel (cl_kernel kernel)
	{
		return kernel->dispatch->clReleaseKernel(kernel);
	}

	cl_int clSetKernelArg (cl_kernel kernel,
						   cl_uint arg_index,
						   size_t arg_size,
						   const void *arg_value)
	{
		return kernel->dispatch->clSetKernelArg(kernel,
												arg_index,
												arg_size,
												arg_value);
	}

	cl_int clGetKernelInfo (cl_kernel kernel,
							cl_kernel_info param_name,
							size_t param_value_size,
							void *param_value,
							size_t *param_value_size_ret)
	{
		return kernel->dispatch->clGetKernelInfo(kernel,
												 param_name,
												 param_value_size,
												 param_value,
												 param_value_size_ret);
	}

	cl_int clGetKernelWorkGroupInfo (cl_kernel kernel,
									 cl_device_id device,
									 cl_kernel_work_group_info param_name,
									 size_t param_value_size,
									 void *param_value,
									 size_t *param_value_size_ret)
	{
		return kernel->dispatch->clGetKernelWorkGroupInfo(kernel,
														  device,
														  param_name,
														  param_value_size,
														  param_value,
														  param_value_size_ret);
	}

	cl_int clEnqueueNDRangeKernel (cl_command_queue command_queue,
								   cl_kernel kernel,
								   cl_uint work_dim,
								   const size_t *global_work_offset,
								   const size_t *global_work_size,
								   const size_t *local_work_size,
								   cl_uint num_events_in_wait_list,
								   const cl_event *event_wait_list,
								   cl_event *event)
	{
		return command_queue->dispatch->clEnqueueNDRangeKernel(command_queue,
															   kernel,
															   work_dim,
															   global_work_offset,
															   global_work_size,
															   local_work_size,
															   num_events_in_wait_list,
															   event_wait_list,
															   event);
	}

	cl_int clEnqueueTask (cl_command_queue command_queue,
						  cl_kernel kernel,
						  cl_uint num_events_in_wait_list,
						  const cl_event *event_wait_list,
						  cl_event *event)
	{
		return command_queue->dispatch->clEnqueueTask(command_queue,
													  kernel,
													  num_events_in_wait_list,
													  event_wait_list,
													  event);
	}

	cl_mem clCreateBuffer (cl_context context,
						   cl_mem_flags flags,
						   size_t size,
						   void *host_ptr,
						   cl_int *errcode_ret)
	{
		return context->dispatch->clCreateBuffer(context,
												 flags,
												 size,
												 host_ptr,
												 errcode_ret);
	}

	cl_mem clCreateSubBuffer (cl_mem buffer,
							  cl_mem_flags flags,
							  cl_buffer_create_type buffer_create_type,
							  const void *buffer_create_info,
							  cl_int *errcode_ret)
	{
		return buffer->dispatch->clCreateSubBuffer(buffer,
												   flags,
												   buffer_create_type,
												   buffer_create_info,
												   errcode_ret);
	}

	cl_int clRetainMemObject (cl_mem memobj)
	{
		return memobj->dispatch->clRetainMemObject(memobj);
	}

	cl_int clReleaseMemObject (cl_mem memobj)
	{
		return memobj->dispatch->clReleaseMemObject(memobj);
	}

	cl_int clSetMemObjectDestructorCallback (cl_mem memobj,
											 void (CL_CALLBACK *pfn_notify)(cl_mem memobj,
																			void *user_data),
											 void *user_data)
	{
		return memobj->dispatch->clSetMemObjectDestructorCallback(memobj,
																  pfn_notify,
																  user_data);
	}

	cl_int clGetMemObjectInfo (cl_mem memobj,
							   cl_mem_info param_name,
							   size_t param_value_size,
							   void *param_value,
							   size_t *param_value_size_ret)
	{
		return memobj->dispatch->clGetMemObjectInfo(memobj,
													param_name,
													param_value_size,
													param_value,
													param_value_size_ret);
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
		return command_queue->dispatch->clEnqueueReadBuffer(command_queue,
															buffer,
															blocking_read,
															offset,
															cb,
															ptr,
															num_events_in_wait_list,
															event_wait_list,
															event);
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
		return command_queue->dispatch->clEnqueueWriteBuffer(command_queue,
															 buffer,
															 blocking_write,
															 offset,
															 cb,
															 ptr,
															 num_events_in_wait_list,
															 event_wait_list,
															 event);
	}

	cl_int clEnqueueCopyBuffer (cl_command_queue command_queue,
								cl_mem src_buffer,
								cl_mem dst_buffer,
								size_t src_offset,
								size_t dst_offset,
								size_t cb,
								cl_uint num_events_in_wait_list,
								const cl_event *event_wait_list,
								cl_event *event)
	{
		return command_queue->dispatch->clEnqueueCopyBuffer(command_queue,
															src_buffer,
															dst_buffer,
															src_offset,
															dst_offset,
															cb,
															num_events_in_wait_list,
															event_wait_list,
															event);
	}

	void * clEnqueueMapBuffer (cl_command_queue command_queue,
							   cl_mem buffer,
							   cl_bool blocking_map,
							   cl_map_flags map_flags,
							   size_t offset,
							   size_t cb,
							   cl_uint num_events_in_wait_list,
							   const cl_event *event_wait_list,
							   cl_event *event,
							   cl_int *errcode_ret)
	{
		return command_queue->dispatch->clEnqueueMapBuffer(command_queue,
														   buffer,
														   blocking_map,
														   map_flags,
														   offset,
														   cb,
														   num_events_in_wait_list,
														   event_wait_list,
														   event,
														   errcode_ret);
	}

	cl_int clEnqueueUnmapMemObject (cl_command_queue command_queue,
									cl_mem memobj,
									void *mapped_ptr,
									cl_uint num_events_in_wait_list,
									const cl_event *event_wait_list,
									cl_event *event)
	{
		return command_queue->dispatch->clEnqueueUnmapMemObject(command_queue,
																memobj,
																mapped_ptr,
																num_events_in_wait_list,
																event_wait_list,
																event);
	}

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueCopyBufferRect(cl_command_queue     command_queue,
							cl_mem               src_buffer,
							cl_mem               dst_buffer,
							const size_t *       src_origin,
							const size_t *       dst_origin,
							const size_t *       region,
							size_t               src_row_pitch,
							size_t               src_slice_pitch,
							size_t               dst_row_pitch,
							size_t               dst_slice_pitch,
							cl_uint              num_events_in_wait_list,
							const cl_event *     event_wait_list,
							cl_event *           event ) CL_API_SUFFIX__VERSION_1_1
	{
		return command_queue->dispatch->clEnqueueCopyBufferRect(command_queue,
																src_buffer,
																dst_buffer,
																src_origin,
																dst_origin,
																region,
																src_row_pitch,
																src_slice_pitch,
																dst_row_pitch,
																dst_slice_pitch,
																num_events_in_wait_list,
																event_wait_list,
																event);
	}

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueWriteBufferRect(cl_command_queue     command_queue,
							 cl_mem               buffer,
							 cl_bool              blocking_write,
							 const size_t *       buffer_origin,
							 const size_t *       host_origin,
							 const size_t *       region,
							 size_t               buffer_row_pitch,
							 size_t               buffer_slice_pitch,
							 size_t               host_row_pitch,
							 size_t               host_slice_pitch,
							 const void *         ptr,
							 cl_uint              num_events_in_wait_list,
							 const cl_event *     event_wait_list,
							 cl_event *           event) CL_API_SUFFIX__VERSION_1_1
	{
		return command_queue->dispatch->clEnqueueWriteBufferRect(command_queue,
																 buffer,
																 blocking_write,
																 buffer_origin,
																 host_origin,
																 region,
																 buffer_row_pitch,
																 buffer_slice_pitch,
																 host_row_pitch,
																 host_slice_pitch,
																 ptr,
																 num_events_in_wait_list,
																 event_wait_list,
																 event);
	}

	CL_API_ENTRY cl_int CL_API_CALL
	clEnqueueReadBufferRect(cl_command_queue     command_queue,
							cl_mem               buffer,
							cl_bool              blocking_read,
							const size_t *       buffer_origin,
							const size_t *       host_origin,
							const size_t *       region,
							size_t               buffer_row_pitch,
							size_t               buffer_slice_pitch,
							size_t               host_row_pitch,
							size_t               host_slice_pitch,
							void *               ptr,
							cl_uint              num_events_in_wait_list,
							const cl_event *     event_wait_list,
							cl_event *           event) CL_API_SUFFIX__VERSION_1_1
	{
		return command_queue->dispatch->clEnqueueReadBufferRect(command_queue,
																buffer,
																blocking_read,
																buffer_origin,
																host_origin,
																region,
																buffer_row_pitch,
																buffer_slice_pitch,
																host_row_pitch,
																host_slice_pitch,
																ptr,
																num_events_in_wait_list,
																event_wait_list,
																event);
	}

	cl_int clGetPlatformInfo(cl_platform_id platform,
							 cl_platform_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		return platform->dispatch->clGetPlatformInfo(platform,
													 param_name,
													 param_value_size,
													 param_value,
													 param_value_size_ret);
	}

	cl_int clGetPlatformIDs (cl_uint num_entries,
							 cl_platform_id *platforms,
							 cl_uint *num_platforms)
	{
		if ((num_entries != 0 && platforms == NULL)
			|| (platforms == NULL && num_platforms == NULL))
			return CL_INVALID_VALUE;

		if (num_platforms != NULL)
			*num_platforms = 0;

		if (platforms != NULL)
		{
		}

		return CL_SUCCESS;
	}

	cl_program clCreateProgramWithSource (cl_context context,
										  cl_uint count,
										  const char **strings,
										  const size_t *lengths,
										  cl_int *errcode_ret)
	{
		return context->dispatch->clCreateProgramWithSource(context,
															count,
															strings,
															lengths,
															errcode_ret);
	}

	cl_program clCreateProgramWithBinary (cl_context context,
										  cl_uint num_devices,
										  const cl_device_id *device_list,
										  const size_t *lengths,
										  const unsigned char **binaries,
										  cl_int *binary_status,
										  cl_int *errcode_ret)
	{
		return context->dispatch->clCreateProgramWithBinary(context,
															num_devices,
															device_list,
															lengths,
															binaries,
															binary_status,
															errcode_ret);
	}

	cl_int clRetainProgram (cl_program program)
	{
		return program->dispatch->clRetainProgram(program);
	}

	cl_int clReleaseProgram (cl_program program)
	{
		return program->dispatch->clReleaseProgram(program);
	}

	cl_int clBuildProgram (cl_program program,
						   cl_uint num_devices,
						   const cl_device_id *device_list,
						   const char *options,
						   void (CL_CALLBACK *pfn_notify)(cl_program program,
														  void *user_data),
						   void *user_data)
	{
		return program->dispatch->clBuildProgram(program,
												 num_devices,
												 device_list,
												 options,
												 pfn_notify,
												 user_data);
	}

	cl_int clUnloadCompiler (void)
	{
		// Guess what ? We never load the compiler as it is an external program!
		return CL_SUCCESS;
	}

	cl_int clGetProgramInfo (cl_program program,
							 cl_program_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		return program->dispatch->clGetProgramInfo(program,
												   param_name,
												   param_value_size,
												   param_value,
												   param_value_size_ret);
	}

	cl_int clGetProgramBuildInfo (cl_program program,
								  cl_device_id device,
								  cl_program_build_info param_name,
								  size_t param_value_size,
								  void *param_value,
								  size_t *param_value_size_ret)
	{
		return program->dispatch->clGetProgramBuildInfo(program,
														device,
														param_name,
														param_value_size,
														param_value,
														param_value_size_ret);
	}

	cl_sampler clCreateSampler (cl_context context,
								cl_bool normalized_coords,
								cl_addressing_mode addressing_mode,
								cl_filter_mode filter_mode,
								cl_int *errcode_ret)
	{
		return context->dispatch->clCreateSampler(context,
												  normalized_coords,
												  addressing_mode,
												  filter_mode,
												  errcode_ret);
	}

	cl_int clRetainSampler (cl_sampler sampler)
	{
		return sampler->dispatch->clRetainSampler(sampler);
	}

	cl_int clReleaseSampler (cl_sampler sampler)
	{
		return sampler->dispatch->clReleaseSampler(sampler);
	}

	cl_int clGetSamplerInfo (cl_sampler sampler,
							 cl_sampler_info param_name,
							 size_t param_value_size,
							 void *param_value,
							 size_t *param_value_size_ret)
	{
		return sampler->dispatch->clGetSamplerInfo(sampler,
												   param_name,
												   param_value_size,
												   param_value,
												   param_value_size_ret);
	}

}
