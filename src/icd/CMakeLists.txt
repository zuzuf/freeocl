
set(ICD_SOURCES
	icd_api.cpp
	icd_loader.cpp icd_loader.h
	)

add_library(OpenCL SHARED ${ICD_SOURCES})
if(NOT CMAKE_HOST_APPLE)
	set_target_properties(OpenCL PROPERTIES LINK_FLAGS "-Wl,--version-script=${FreeOCL_SOURCE_DIR}/OpenCL.map" VERSION 1.2 SOVERSION 1.2 SOVERSION 1 )
endif(NOT CMAKE_HOST_APPLE)

# Installation and packaging process
install(TARGETS OpenCL LIBRARY DESTINATION lib)
