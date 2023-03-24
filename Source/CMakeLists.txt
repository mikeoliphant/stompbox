file(GLOB_RECURSE SOURCES *.cpp *.c *.h)

set(NAM_SOURCES ../Dependencies/NeuralAmpModelerCore/NAM/activations.h
	../Dependencies/NeuralAmpModelerCore/NAM/version.h
	../Dependencies/NeuralAmpModelerCore/NAM/lstm.h
	../Dependencies/NeuralAmpModelerCore/NAM/lstm.cpp
	../Dependencies/NeuralAmpModelerCore/NAM/dsp.h
	../Dependencies/NeuralAmpModelerCore/NAM/dsp.cpp
	../Dependencies/NeuralAmpModelerCore/NAM/get_dsp.cpp
	../Dependencies/NeuralAmpModelerCore/NAM/util.cpp
	../Dependencies/NeuralAmpModelerCore/NAM/util.h
	../Dependencies/NeuralAmpModelerCore/NAM/wavenet.cpp
	../Dependencies/NeuralAmpModelerCore/NAM/wavenet.h)

add_library(stompbox STATIC ${SOURCES} ${NAM_SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(NAM ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NAM_SOURCES})

include_directories(stompbox ../Dependencies/eigen)
include_directories(stompbox ../Dependencies/NeuralAmpModelerCore/NAM)
include_directories(stompbox ../Dependencies/json)
include_directories(stompbox ${CMAKE_CURRENT_SOURCE_DIR} Stomps Stomps/Clipping Stomps/TubeTables )

target_compile_features(stompbox PUBLIC cxx_std_17)

set_target_properties(stompbox
	PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	INTERPROCEDURAL_OPTIMIZATION TRUE
	PREFIX ""
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_compile_definitions(stompbox PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

if (MSVC)
	target_compile_options(stompbox PRIVATE
		"$<$<CONFIG:DEBUG>:/W4>"
		"$<$<CONFIG:RELEASE>:/O2>"
	)
else()
	target_compile_options(stompbox PRIVATE
		-Wall -Wextra -Wpedantic -Wshadow -Wstrict-aliasing
		-Wunreachable-code -Wdouble-promotion -Weffc++ -Wconversion
		-Wsign-conversion
		"$<$<CONFIG:DEBUG>:-Og;-ggdb;-Werror>"
		"$<$<CONFIG:RELEASE>:-Ofast>"
	)
endif()
