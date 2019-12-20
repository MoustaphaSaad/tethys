cmake_minimum_required(VERSION 3.9)

# list the header files
set(HEADER_FILES
	include/as/Pos.h
	include/as/Rng.h
	include/as/Token_Listing.h
	include/as/Tkn.h
	include/as/Err.h
	include/as/Scan.h
	include/as/Src.h
	include/as/Parse_Tree.h
	include/as/Parse.h
	include/as/Gen.h
)

# list the source files
set(SOURCE_FILES
	src/as/Scan.cpp
	src/as/Src.cpp
	src/as/Parse.cpp
	src/as/Gen.cpp
	src/as/Parse_Tree.cpp
)


# add library target
add_library(as
	${HEADER_FILES}
	${SOURCE_FILES}
)

add_library(MoustaphaSaad::as ALIAS as)

target_link_libraries(as
	PUBLIC
		MoustaphaSaad::mn
		MoustaphaSaad::vm
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})


# Windows specfic flags to enable W variations of winapi
if(WIN32)
	target_compile_options(as
		PRIVATE
			/WX /W4
	)
elseif(UNIX)
	target_compile_options(as
		PRIVATE
			-Wall -Werror
	)
endif()

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(as PUBLIC cxx_std_17)
set_target_properties(as PROPERTIES
	CXX_EXTENSIONS OFF
	DEBUG_POSTFIX d
)

# define debug macro
target_compile_definitions(as PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

# generate exports header file
include(GenerateExportHeader)
generate_export_header(as
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/as/Exports.h
)

# list include directories
target_include_directories(as
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)