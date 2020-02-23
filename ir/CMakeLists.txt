cmake_minimum_required(VERSION 3.9)

# list the header files
set(HEADER_FILES
	include/ir/Bag.h
)

# list the source files
set(SOURCE_FILES
)


# add library target
add_library(ir
	${HEADER_FILES}
	${SOURCE_FILES}
)

add_library(MoustaphaSaad::ir ALIAS ir)

target_link_libraries(ir
	PUBLIC
		MoustaphaSaad::mn
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})


# Windows specfic flags to enable W variations of winapi
if(WIN32)
	target_compile_options(ir
		PRIVATE
			/WX /W4
	)
elseif(UNIX)
	target_compile_options(ir
		PRIVATE
			-Wall -Werror
	)
endif()

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(ir PUBLIC cxx_std_17)
set_target_properties(ir PROPERTIES
	CXX_EXTENSIONS OFF
	DEBUG_POSTFIX d
)

# define debug macro
target_compile_definitions(ir PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

# generate exports header file
include(GenerateExportHeader)
generate_export_header(ir
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/ir/Exports.h
)

# list include directories
target_include_directories(ir
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)