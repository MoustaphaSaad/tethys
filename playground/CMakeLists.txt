cmake_minimum_required(VERSION 3.9)

# list the source files
set(SOURCE_FILES
	main.cpp
)

# add executable
add_executable(playground
	${SOURCE_FILES}
)

target_link_libraries(playground
	PUBLIC
	MoustaphaSaad::mn
	MoustaphaSaad::vm
	MoustaphaSaad::as
	MoustaphaSaad::ffi
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# let's enable warnings as errors
if(WIN32)
	target_compile_options(playground
		PRIVATE
			/WX /W4
	)
	target_link_libraries(playground PUBLIC libffi)
elseif(UNIX)
	target_compile_options(playground
		PRIVATE
			-Wall -Werror
	)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(libffi REQUIRED IMPORTED_TARGET libffi)
	target_link_libraries(playground
		PUBLIC
			PkgConfig::libffi
	)
endif()

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(playground PUBLIC cxx_std_17)
set_target_properties(playground PROPERTIES
	CXX_EXTENSIONS OFF
)

# define debug macro
target_compile_definitions(playground PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")
