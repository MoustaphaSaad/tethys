cmake_minimum_required(VERSION 3.9)

# add library target
add_library(libffi SHARED IMPORTED GLOBAL)

if(WIN32)
	set_target_properties(libffi
		PROPERTIES
		IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/win64/bin/libffi-7.dll
		IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/win64/bin/libffi-7.lib
		INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/win64/include
	)
	add_custom_target(deploy_libffi
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/win64/bin/libffi-7.dll
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libffi-7.dll
	)
	add_dependencies(libffi deploy_libffi)
endif()