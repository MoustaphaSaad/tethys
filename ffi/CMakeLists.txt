cmake_minimum_required(VERSION 3.9)

# list the header files
set(HEADER_FILES
	include/ffi/FFI.h
)

# list the source files
set(SOURCE_FILES
	src/ffi/FFI.cpp
)

set(ASM_FILES
)

if (WIN32)
	enable_language(ASM_MASM)
	if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		message(FATAL_ERROR "you platform is not supported")
	elseif ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		set(ASM_FILES ${ASM_FILES}
			src/ffi/win64/call.asm
		)
	endif ()
else()
	enable_language(ASM)
	message(FATAL_ERROR "you platform is not supported")
endif()


# add library target
add_library(ffi
	${HEADER_FILES}
	${SOURCE_FILES}
	${ASM_FILES}
)

add_library(MoustaphaSaad::ffi ALIAS ffi)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ASM_FILES})


# Windows specfic flags to enable W variations of winapi
if(WIN32)
	target_compile_options(ffi
		PRIVATE
			/WX /W4
	)
elseif(UNIX)
	target_compile_options(ffi
		PRIVATE
			-Wall -Werror
	)
endif()

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(ffi PUBLIC cxx_std_17)
set_target_properties(ffi PROPERTIES
	CXX_EXTENSIONS OFF
	DEBUG_POSTFIX d
)

# define debug macro
target_compile_definitions(ffi PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

# generate exports header file
include(GenerateExportHeader)
generate_export_header(ffi
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/ffi/Exports.h
)

# list include directories
target_include_directories(ffi
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)