cmake_minimum_required(VERSION 3.9)

# list the header files
set(HEADER_FILES
	include/vm/Reg.h
	include/vm/Op.h
	include/vm/Util.h
	include/vm/Core.h
	include/vm/Pkg.h
	include/vm/C.h
)

# list the source files
set(SOURCE_FILES
	src/vm/Core.cpp
	src/vm/Pkg.cpp
	src/vm/C.cpp
)


# add library target
add_library(vm
	${HEADER_FILES}
	${SOURCE_FILES}
)

add_library(MoustaphaSaad::vm ALIAS vm)

target_link_libraries(vm
	PUBLIC
		MoustaphaSaad::mn
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})


# Windows specfic flags to enable W variations of winapi
if(WIN32)
	target_compile_options(vm
		PRIVATE
			/WX /W4
	)
	target_link_libraries(vm PUBLIC libffi)
elseif(UNIX)
	target_compile_options(vm
		PRIVATE
			-Wall -Werror
	)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(libffi REQUIRED IMPORTED_TARGET libffi)
	target_link_libraries(vm
		PUBLIC
			PkgConfig::libffi
	)
endif()

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(vm PUBLIC cxx_std_17)
set_target_properties(vm PROPERTIES
	CXX_EXTENSIONS OFF
	DEBUG_POSTFIX d
)

# define debug macro
target_compile_definitions(vm PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

# generate exports header file
include(GenerateExportHeader)
generate_export_header(vm
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/vm/Exports.h
)

# list include directories
target_include_directories(vm
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)