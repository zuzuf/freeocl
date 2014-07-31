
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(EXTRA_SOURCES
        ${PROJECT_SOURCE_DIR}/libs/dlfcn-win32/dlfcn.h
        ${PROJECT_SOURCE_DIR}/libs/dlfcn-win32/dlfcn.c
        ${PROJECT_SOURCE_DIR}/libs/win32.64_ucontext/ucontext.h
        ${PROJECT_SOURCE_DIR}/libs/win32.64_ucontext/ucontext.c
        )
    include_directories(../libs/dlfcn-win32/)
    include_directories(../libs/win32.64_ucontext/)
endif(CMAKE_SYSTEM_NAME STREQUAL "Windows")

set(ICD_SOURCES
	icd_api.cpp
	icd_loader.cpp icd_loader.h
	)

add_library(OpenCL SHARED ${ICD_SOURCES} ${EXTRA_SOURCES})
if(NOT CMAKE_HOST_APPLE)
	set_target_properties(OpenCL PROPERTIES LINK_FLAGS "-Wl,--version-script=${FreeOCL_SOURCE_DIR}/OpenCL.map" VERSION 1.2 SOVERSION 1.2 SOVERSION 1 )
endif(NOT CMAKE_HOST_APPLE)

# Installation and packaging process
if(NOT (CMAKE_SYSTEM_NAME STREQUAL "Windows"))
	install(TARGETS OpenCL LIBRARY DESTINATION lib${LIB_SUFFIX})
endif(NOT (CMAKE_SYSTEM_NAME STREQUAL "Windows"))

