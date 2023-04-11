file(GLOB_RECURSE SOURCES *.cpp *.c *.h)

set(NAM_SOURCES "${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/activations.h"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/activations.cpp"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/version.h"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/lstm.h"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/lstm.cpp"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/dsp.h"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/dsp.cpp"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/get_dsp.cpp"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/util.cpp"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/util.h"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/convnet.h"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/convnet.cpp"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/wavenet.cpp"
	"${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM/wavenet.h")

add_library(stompbox STATIC ${SOURCES} ${NAM_SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(NAM ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NAM_SOURCES})

include_directories(stompbox ${STOMPBOX_DEPS_PATH}/eigen)
include_directories(stompbox ${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM)
include_directories(stompbox ${STOMPBOX_DEPS_PATH}/json)
include_directories(stompbox ${STOMPBOX_DEPS_PATH}/denormal)
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
		-Wall -Wextra -Wpedantic -Wstrict-aliasing -Wunreachable-code -Weffc++ -Wno-unused-parameter
		"$<$<CONFIG:DEBUG>:-Og;-ggdb;-Werror>"
		"$<$<CONFIG:RELEASE>:-Ofast>"
	)
endif()
