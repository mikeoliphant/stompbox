cmake_minimum_required(VERSION 3.10)

project(StompBox VERSION 0.0.1)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	include_directories(SYSTEM /usr/local/include)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
else()
	message(FATAL_ERROR "Unrecognized Platform!")
endif()

set(STOMPBOX_DEPS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies")

link_libraries(stdc++fs pthread)

add_subdirectory(stompbox)

file(MAKE_DIRECTORY build/stompbox)

add_custom_target(copy_library ALL
	${CMAKE_COMMAND} -E copy "$<TARGET_FILE:stompbox>" stompbox/
	DEPENDS stompbox
)

find_library(JACK_LIB libjack.so)

if(JACK_LIB)
	add_subdirectory(stompbox-jack)

	file(MAKE_DIRECTORY build/stompbox-jack)

	add_custom_target(copy_binaries ALL
		${CMAKE_COMMAND} -E copy "$<TARGET_FILE:stompbox-jack>" stompbox-jack/
		DEPENDS stompbox-jack
	)
else()
        message("Jack libraries not found - stombox-jack will not be built")
endif()
