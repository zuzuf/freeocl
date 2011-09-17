option(BUILD_SAMPLES "build samples")

if(BUILD_SAMPLES)
	set(SAMPLES_COMMON_SOURCES
		errors.cpp errors.h)
	add_library(samples_common STATIC ${SAMPLES_COMMON_SOURCES})

	set(SAMPLES_LIBRARIES samples_common OpenCL)

	set(CLDEMO_SOURCES cldemo.cpp)
	add_executable(cldemo ${CLDEMO_SOURCES})
	target_link_libraries(cldemo ${SAMPLES_LIBRARIES})

	set(CLTEST_SOURCES cltest.cpp)
	add_executable(cltest ${CLTEST_SOURCES})
	target_link_libraries(cltest ${SAMPLES_LIBRARIES})

	set(CLLIST_SOURCES cllist.cpp)
	add_executable(cllist ${CLLIST_SOURCES})
	target_link_libraries(cllist ${SAMPLES_LIBRARIES})

	install(TARGETS cldemo cltest cllist
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
endif(BUILD_SAMPLES)
