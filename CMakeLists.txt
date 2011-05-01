cmake_minimum_required (VERSION 2.8)
project (FreeOCL)

SET(FREEOCL_VERSION_HI		0)
SET(FREEOCL_VERSION_LO		0)
SET(FREEOCL_VERSION_PATCH	0)
SET(FREEOCL_VERSION_STRING	"${FREEOCL_VERSION_HI}.${FREEOCL_VERSION_LO}.${FREEOCL_VERSION_PATCH}")
SET(FREEOCL_VENDOR	"Roland Brochard")

SET(CMAKE_THREAD_PREFER_PTHREAD pthread)
include(FindThreads)
if(NOT CMAKE_THREAD_LIBS_INIT)
	message(FATAL_ERROR "thread library not found")
endif(NOT CMAKE_THREAD_LIBS_INIT)

include(FindOpenMP)
if(NOT OPENMP_FOUND)
	message(FATAL_ERROR "OpenMP not found, OpenMP is required")
endif(NOT OPENMP_FOUND)

SET(SOURCES
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
	src/parser/parse.cc
	src/parser/Parserbase.h
	src/parser/Parser.h
	src/parser/Parser.ih
	src/parser/macros.h
	src/parser/lex.cpp
	src/parser/error.cpp
	src/parser/node.cpp	src/parser/node.h

	src/utils/smartptr.h
	)

SET(API_SOURCES
	src/icd_api.cpp
	src/icd_loader.cpp src/icd_loader.h
	)

SET(DEMO_SOURCES
	samples/demo.cpp
	samples/errors.cpp samples/errors.h)

SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")

configure_file(config.h.cmake ${FreeOCL_SOURCE_DIR}/config.h)

include_directories(include)
LINK_LIBRARIES(${CMAKE_THREAD_LIBS_INIT})
LINK_LIBRARIES(-ldl)
add_library(FreeOCL SHARED ${SOURCES})
add_library(OpenCL SHARED ${API_SOURCES})

IF(BUILD_DEMO)
	LINK_LIBRARIES(OpenCL)
	add_executable(demo ${DEMO_SOURCES})
ENDIF(BUILD_DEMO)
