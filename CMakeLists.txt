project (FreeOCL)
cmake_minimum_required (VERSION 2.8)

set(FREEOCL_VERSION_HI		0)
set(FREEOCL_VERSION_LO		0)
set(FREEOCL_VERSION_PATCH	0)
set(FREEOCL_VERSION_STRING	"${FREEOCL_VERSION_HI}.${FREEOCL_VERSION_LO}.${FREEOCL_VERSION_PATCH}")
set(FREEOCL_VENDOR	"Roland Brochard")

set(CMAKE_THREAD_PREFER_PTHREAD pthread)
include(FindThreads)
if(NOT CMAKE_THREAD_LIBS_INIT)
	message(FATAL_ERROR "thread library not found")
endif(NOT CMAKE_THREAD_LIBS_INIT)

include(FindOpenMP)
if(NOT OPENMP_FOUND)
	message(FATAL_ERROR "OpenMP not found, OpenMP is required")
endif(NOT OPENMP_FOUND)

set(SOURCES
	include/CL/cl_d3d10.h
	include/CL/cl_ext.h
	include/CL/cl_gl_ext.h
	include/CL/cl_gl.h
	include/CL/cl.h
	include/CL/cl.hpp
	include/CL/cl_platform.h
	include/CL/opencl.h

	include/FreeOCL/opencl_c.h
	include/FreeOCL/asyncmemop.h
	include/FreeOCL/atomic.h
	include/FreeOCL/common.h
	include/FreeOCL/geometric.h
	include/FreeOCL/imgreadwrite.h
	include/FreeOCL/integer.h
	include/FreeOCL/math.h
	include/FreeOCL/memfence.h
	include/FreeOCL/relational.h
	include/FreeOCL/sync.h
	include/FreeOCL/vloadstore.h
	include/FreeOCL/vmisc.h
	include/FreeOCL/workitem.h
	include/FreeOCL/vectors.h

	src/freeocl.cpp		src/freeocl.h
	src/platform.cpp	src/platform.h
	src/device.cpp		src/device.h
	src/context.cpp		src/context.h
	src/commandqueue.cpp	src/commandqueue.h
	src/kernel.cpp		src/kernel.h
	src/program.cpp		src/program.h
	src/mem.cpp			src/mem.h
	src/event.cpp		src/event.h
	src/mutex.cpp		src/mutex.h
	src/condition.cpp	src/condition.h
	src/thread.cpp		src/thread.h
	src/sampler.cpp		src/sampler.h
	src/image.cpp
	src/dispatch.h
	src/prototypes.h
	src/codebuilder.cpp	src/codebuilder.h

	src/parser/grammar
	src/parser/parser.cpp	src/parser/parser.h
	src/parser/macros.h
	src/parser/lex.cpp
	src/parser/error.cpp
	src/parser/node.cpp		src/parser/node.h
	src/parser/chunk.cpp	src/parser/chunk.h
	src/parser/value.h		src/parser/value.cpp
	src/parser/token.h		src/parser/token.cpp
	src/parser/type.h			src/parser/type.cpp
	src/parser/native_type.h	src/parser/native_type.cpp
	src/parser/struct_type.h	src/parser/struct_type.cpp
	src/parser/union_type.h		src/parser/union_type.cpp
	src/parser/pointer_type.h	src/parser/pointer_type.cpp
	src/parser/typedef.h		src/parser/typedef.cpp
	src/parser/function.h		src/parser/function.cpp
	src/parser/kernel.h			src/parser/kernel.cpp
	src/parser/binary.h			src/parser/binary.cpp
	src/parser/unary.h			src/parser/unary.cpp
	src/parser/index.h			src/parser/index.cpp
	src/parser/var.h			src/parser/var.cpp
	src/parser/call.h			src/parser/call.cpp
	src/parser/expression.h
	src/parser/callable.h
	src/parser/symbol_table.h	src/parser/symbol_table.cpp
	src/parser/builtin.h		src/parser/builtin.cpp
	src/parser/overloaded_builtin.h		src/parser/overloaded_builtin.cpp
	src/parser/member.h			src/parser/member.cpp
	src/parser/swizzle.h		src/parser/swizzle.cpp
	src/parser/cast.h			src/parser/cast.cpp

	src/utils/smartptr.h
	src/utils/string.h
	)

set(API_SOURCES
	src/icd_api.cpp
	src/icd_loader.cpp src/icd_loader.h
	)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -std=c++0x")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ffast-math -std=c++0x")

configure_file(config.h.cmake ${FreeOCL_SOURCE_DIR}/config.h)

include_directories(include)
link_libraries(${CMAKE_THREAD_LIBS_INIT})
link_libraries(-ldl)
add_library(FreeOCL SHARED ${SOURCES})
add_library(OpenCL SHARED ${API_SOURCES})
set_target_properties(OpenCL PROPERTIES VERSION	1.1 SOVERSION 1.1)

option(BUILD_SAMPLES "build samples")

if(BUILD_SAMPLES)
	link_libraries(OpenCL)
	set(CLDEMO_SOURCES
		samples/cldemo.cpp
		samples/errors.cpp samples/errors.h)
	add_executable(cldemo ${CLDEMO_SOURCES})
	set(CLTEST_SOURCES
		samples/cltest.cpp
		samples/errors.cpp samples/errors.h)
	add_executable(cltest ${CLTEST_SOURCES})
	set(CLLIST_SOURCES
		samples/cllist.cpp
		samples/errors.cpp samples/errors.h)
	add_executable(cllist ${CLLIST_SOURCES})
endif(BUILD_SAMPLES)
