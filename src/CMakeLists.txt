
set(PARSER_SOURCES
	parser/grammar
	parser/parser.cpp	parser/parser.h
	parser/macros.h
	parser/lex.cpp
	parser/error.cpp
    parser/pragma.cpp
	parser/node.cpp		parser/node.h
	parser/chunk.cpp	parser/chunk.h
	parser/value.h		parser/value.cpp
	parser/token.h		parser/token.cpp
	parser/struct_literal.h parser/struct_literal.cpp
	parser/type.h			parser/type.cpp
	parser/array_type.h		parser/array_type.cpp
	parser/native_type.h	parser/native_type.cpp
	parser/struct_type.h	parser/struct_type.cpp
	parser/union_type.h		parser/union_type.cpp
	parser/pointer_type.h	parser/pointer_type.cpp
	parser/enum_type.h      parser/enum_type.cpp
	parser/typedef.h		parser/typedef.cpp
	parser/function.h		parser/function.cpp
	parser/printf.h			parser/printf.cpp
	parser/kernel.h			parser/kernel.cpp
	parser/ternary.h		parser/ternary.cpp
	parser/binary.h			parser/binary.cpp
	parser/unary.h			parser/unary.cpp
	parser/index.h			parser/index.cpp
	parser/var.h			parser/var.cpp
	parser/call.h			parser/call.cpp
	parser/expression.h
	parser/callable.h		parser/callable.cpp
	parser/symbol_table.h	parser/symbol_table.cpp
	parser/builtin.h		parser/builtin.cpp
	parser/overloaded_builtin.h		parser/overloaded_builtin.cpp
	parser/overloaded_function.h	parser/overloaded_function.cpp
	parser/member.h			parser/member.cpp
	parser/swizzle.h		parser/swizzle.cpp
	parser/cast.h			parser/cast.cpp
	parser/declarator.h		parser/declarator.cpp
	parser/qualifier.h		parser/qualifier.cpp
	parser/sizeof.h			parser/sizeof.cpp
	parser/if.h				parser/if.cpp
	parser/do.h				parser/do.cpp
	parser/while.h			parser/while.cpp
	)

set(PREPROCESSOR_SOURCES
	preprocessor/preprocessor.cpp	preprocessor/preprocessor.h
	preprocessor/tokenizer.cpp
	preprocessor/expression.cpp
	preprocessor/macro_expansion.cpp
	)

set(UTILS_SOURCES
	utils/map.h
	utils/set.h
	utils/hash.h
	utils/smartptr.h
	utils/string.h
	utils/commandqueue.cpp	utils/commandqueue.h
	utils/mutex.cpp			utils/mutex.h
	utils/condition.cpp		utils/condition.h
	utils/thread.cpp		utils/thread.h
	utils/time.cpp			utils/time.h
	utils/threadpool.cpp	utils/threadpool.h
	)

set(SOURCES
	freeocl.cpp		freeocl.h
	platform.cpp	platform.h
	device.cpp		device.h
	context.cpp		context.h

	kernel.cpp		kernel.h
	program.cpp		program.h
	mem.cpp			mem.h
	event.cpp		event.h

	sampler.cpp		sampler.h
	image.cpp
	dispatch.h
	prototypes.h
	codebuilder.cpp	codebuilder.h
	)

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

# Add FreeOCL headers in order to have them recognized by IDEs
set(FREEOCL_HEADERS
	../include/FreeOCL/asyncmemop.h
	../include/FreeOCL/atomic.h
	../include/FreeOCL/common.h
	../include/FreeOCL/converters.h
	../include/FreeOCL/geometric.h
	../include/FreeOCL/imgreadwrite.h
	../include/FreeOCL/image1d_t.h
	../include/FreeOCL/image1d_buffer_t.h
	../include/FreeOCL/image1d_array_t.h
	../include/FreeOCL/image2d_t.h
	../include/FreeOCL/image2d_array_t.h
	../include/FreeOCL/image3d_t.h
	../include/FreeOCL/integer.h
	../include/FreeOCL/math.h
	../include/FreeOCL/memfence.h
	../include/FreeOCL/opencl_c.h
	../include/FreeOCL/relational.h
	../include/FreeOCL/sync.h
	../include/FreeOCL/vectors.h
	../include/FreeOCL/vloadstore.h
	../include/FreeOCL/vmisc.h
	../include/FreeOCL/workitem.h
	../include/FreeOCL/simdopts.h
	../include/FreeOCL/printf.h
    ../include/FreeOCL/ucontext_inline.h
	)

add_library(FreeOCL SHARED ${SOURCES} ${PARSER_SOURCES} ${UTILS_SOURCES} ${FREEOCL_HEADERS} ${PREPROCESSOR_SOURCES} ${EXTRA_SOURCES})
target_link_libraries(FreeOCL)

# Installation and packaging process
if(NOT (CMAKE_SYSTEM_NAME STREQUAL "Windows"))
	install(TARGETS FreeOCL LIBRARY DESTINATION lib${LIB_SUFFIX})
endif(NOT (CMAKE_SYSTEM_NAME STREQUAL "Windows"))

add_subdirectory(icd)
