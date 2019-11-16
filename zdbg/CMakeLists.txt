cmake_minimum_required(VERSION 3.9)

# list the header files
set(HEADER_FILES
	include/zdbg/Driver.h
)

# list the source files
set(SOURCE_FILES
	src/zdbg/main.cpp
	src/zdbg/Driver.cpp
)

# list resources files
set(RESOURCES_FILES
	qml/qml.qrc
	UI/UI.qrc
	fonts/fonts.qrc
)


# link with qt5
find_package(Qt5 COMPONENTS Core Quick REQUIRED)
qt5_wrap_cpp(HEADER_MOC ${HEADER_FILES})
qt5_add_resources(RCC_SOURCES ${RESOURCES_FILES})


# add library target
add_executable(zdbg
	${HEADER_FILES}
	${SOURCE_FILES}
	${HEADER_MOC}
	${RCC_SOURCES}
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${UI_FILES})

target_link_libraries(zdbg
	PUBLIC
	Qt5::Core
	Qt5::Quick
)

# qml import dir
set(QML_IMPORT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "" FORCE)

# Windows specfic flags to enable W variations of winapi
if(WIN32)
	target_compile_definitions(zdbg
		PRIVATE
			OS_WINDOWS=1
	)
	target_compile_options(zdbg
		PRIVATE
			/WX
	)
elseif(UNIX)
	target_compile_definitions(zdbg
		PRIVATE
			OS_LINUX=1
	)
	target_compile_options(zdbg
		PRIVATE
			-Werror
	)
endif()


# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(zdbg PUBLIC cxx_std_17)
set_target_properties(zdbg PROPERTIES
	CXX_EXTENSIONS OFF
)

# define debug macro
target_compile_definitions(zdbg PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

# list include directories
target_include_directories(zdbg
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
