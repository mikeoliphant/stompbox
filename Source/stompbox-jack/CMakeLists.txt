add_executable(stompbox-jack stompbox-jack.cpp)

# Should remove all of these - PluginProcessor.h pulls them in
include_directories(stompbox ../../Dependencies/eigen)
include_directories(stompbox ../../Dependencies/NeuralAmpModelerCore/NAM)
include_directories(stompbox ../../Dependencies/json)
include_directories(stompbox ../../Dependencies/denormal)
include_directories(stompbox-jack .. ../Stomps Stomps Stomps/Clipping Stomps/TubeTables)

target_compile_features(stompbox-jack PUBLIC cxx_std_17)

set_target_properties(stompbox-jack
	PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	INTERPROCEDURAL_OPTIMIZATION TRUE
	PREFIX ""
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_compile_definitions(stompbox-jack PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

if (MSVC)
	target_compile_options(stompbox-jack PRIVATE
		"$<$<CONFIG:DEBUG>:/W4>"
		"$<$<CONFIG:RELEASE>:/O2>"
	)
else()
	target_compile_options(stompbox-jack PRIVATE
		-Wall -Wextra -Wpedantic -Wshadow -Wstrict-aliasing
		-Wunreachable-code -Wdouble-promotion -Weffc++ -Wconversion
		-Wsign-conversion
		"$<$<CONFIG:DEBUG>:-Og;-ggdb;-Werror>"
		"$<$<CONFIG:RELEASE>:-Ofast>"
	)
endif()
