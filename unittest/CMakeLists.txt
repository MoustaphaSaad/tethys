cmake_minimum_required(VERSION 3.9)

# list source files
set(SOURCE_FILES
	unittest_tas.cpp
	unittest_main.cpp
)

# add executable target
add_executable(tethys_unittest
	${SOURCE_FILES}
)

target_link_libraries(tethys_unittest
	PRIVATE
		MoustaphaSaad::mn
		MoustaphaSaad::as
)

# add doctest folder
target_include_directories(tethys_unittest
	PRIVATE
		${CMAKE_SOURCE_DIR}/external/doctest
)

target_compile_definitions(tethys_unittest
	PRIVATE
		TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test"
)