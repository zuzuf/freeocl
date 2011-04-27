cmake_minimum_required (VERSION 2.8)
project (FreeOCL)

SET(FREEOCL_VERSION_HI		0)
SET(FREEOCL_VERSION_LO		0)
SET(FREEOCL_VERSION_PATCH	0)
SET(FREEOCL_VERSION_STRING	"${FREEOCL_VERSION_HI}.${FREEOCL_VERSION_LO}.${FREEOCL_VERSION_PATCH}")
SET(FREEOCL_VENDOR	"Roland Brochard")

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
	)

SET(DEMO_SOURCES
	samples/demo.cpp)

SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")

configure_file(config.h.cmake ${FreeOCL_SOURCE_DIR}/config.h)

include_directories(include)
add_library(FreeOCL SHARED ${SOURCES})

IF(BUILD_DEMO)
	LINK_LIBRARIES(FreeOCL)
	add_executable(demo ${DEMO_SOURCES})
ENDIF(BUILD_DEMO)
