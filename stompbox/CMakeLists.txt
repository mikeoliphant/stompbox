if (MSVC)
	add_compile_options(
		"$<$<CONFIG:DEBUG>:/W4>"
		"$<$<CONFIG:RELEASE>:/O2>"
	)
else()
	add_compile_options(
		-Wall 
		# -Wpedantic -Wextra -Wstrict-aliasing -Wunreachable-code -Weffc++ -Wno-unused-parameter
		"$<$<CONFIG:DEBUG>:-Og;-ggdb>"
		"$<$<CONFIG:RELWITHDEBINFO>:-Ofast>"
		"$<$<CONFIG:RELEASE>:-Ofast>"
	)
endif()

add_subdirectory(${STOMPBOX_DEPS_PATH}/NeuralAudio NeuralAudio)

include_directories(stompbox ${STOMPBOX_DEPS_PATH}/NeuralAudio)
include_directories(stompbox ${STOMPBOX_DEPS_PATH}/denormal)
include_directories(stompbox ${STOMPBOX_DEPS_PATH}/WDL/WDL)
include_directories(stompbox ${STOMPBOX_DEPS_PATH}/dr_libs)
include_directories(stompbox ${STOMPBOX_DEPS_PATH}/r8brain-free-src)
include_directories(stompbox ${CMAKE_CURRENT_SOURCE_DIR} Stomps)

file(GLOB_RECURSE SOURCES *.cpp *.c *.h)

set(NA_SOURCES ${STOMPBOX_DEPS_PATH}/NeuralAudio/NeuralAudio/NeuralModel.h)

set(OTHER_SOURCES "${STOMPBOX_DEPS_PATH}/r8brain-free-src/r8bbase.cpp"
	"${STOMPBOX_DEPS_PATH}/r8brain-free-src/pffft.cpp"
	"${STOMPBOX_DEPS_PATH}/WDL/WDL/convoengine.cpp"
	"${STOMPBOX_DEPS_PATH}/WDL/WDL/fft.c")

add_library(stompbox STATIC ${SOURCES} ${NA_SOURCES} ${OTHER_SOURCES})

target_link_libraries(stompbox PRIVATE NeuralAudio)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(NeuralAudio ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NA_SOURCES})

add_definitions(-DWDL_FFT_REALSIZE=8)
add_definitions(-DWDL_CONVO_THREAD)
add_definitions(-DWDL_ALLOW_UNSIGNED_DEFAULT_CHAR)

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
