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

	set(CLBUILTINS_SOURCES clbuiltins.cpp)
	add_executable(clbuiltins ${CLBUILTINS_SOURCES})
	target_link_libraries(clbuiltins ${SAMPLES_LIBRARIES})

	include(FindSDL)
	include(FindOpenGL)
	if(SDL_FOUND AND OPENGL_FOUND)
		set(CLGALAXY_SOURCES clgalaxy.cpp)
		add_executable(clgalaxy ${CLGALAXY_SOURCES})
		include_directories(${SDL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
		target_link_libraries(clgalaxy ${SAMPLES_LIBRARIES} ${SDL_LIBRARY} ${OPENGL_LIBRARIES})
	endif(SDL_FOUND AND OPENGL_FOUND)

	install(TARGETS cldemo cltest cllist clbuiltins
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
endif(BUILD_SAMPLES)
