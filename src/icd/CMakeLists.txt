
set(ICD_SOURCES
	icd_api.cpp
	icd_loader.cpp icd_loader.h
	)

add_library(OpenCL SHARED ${ICD_SOURCES})
set_target_properties(OpenCL PROPERTIES LINK_FLAGS "-Wl,--version-script=${FreeOCL_SOURCE_DIR}/OpenCL.map" VERSION 1.1 SOVERSION 1.1)

# Installation and packaging process
install(TARGETS OpenCL LIBRARY DESTINATION lib)
