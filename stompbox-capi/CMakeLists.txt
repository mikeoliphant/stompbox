add_library(stompbox-capi SHARED StompboxCAPI.h StompboxCAPI.cpp)

include_directories(stompbox-capi ../stompbox)

target_link_libraries(stompbox-capi PRIVATE "$<LINK_LIBRARY:WHOLE_ARCHIVE,stompbox>")

target_compile_features(stompbox-capi PUBLIC cxx_std_17)

if (MSVC)
	target_compile_options(stompbox-capi PRIVATE
		"$<$<CONFIG:DEBUG>:/W4>"
		"$<$<CONFIG:RELEASE>:/O2>"
	)
else()
	target_compile_options(stompbox-capi PRIVATE
		-Wall -Wextra -Wpedantic -Wshadow -Wstrict-aliasing
		-Wunreachable-code -Wdouble-promotion -Weffc++ -Wconversion
		-Wsign-conversion
		"$<$<CONFIG:DEBUG>:-Og;-ggdb;-Werror>"
		"$<$<CONFIG:RELEASE>:-Ofast>"
	)
endif()
