function(print_target_compile_options target)
    foreach(config IN ITEMS ${CMAKE_BUILD_TYPE})
        get_target_property(flags ${target} COMPILE_OPTIONS)
        if(flags)
            string(REPLACE ";" " " flags_with_spaces "${flags}")
            message(STATUS "  + ${target} (${config}): ${flags_with_spaces}\n")
        else()
            message(STATUS "  + ${target} (${config}): No flags set\n")
        endif()
    endforeach()
endfunction()

include(ProcessorCount)
ProcessorCount(NUMBER_OF_PROCESSORS)
if(NOT NUMBER_OF_PROCESSORS EQUAL 0)
	set(MAKEFLAGS "-j${NUMBER_OF_PROCESSORS}")
	set(ENV{MAKEFLAGS} "${MAKEFLAGS}")
	MESSAGE(STATUS "Set MAKEFLAGS for MinGW Makefiles to ${MAKEFLAGS}")
endif()


MARK_AS_ADVANCED(MULTITHREADED_BUILD)
SET(MULTITHREADED_BUILD ON CACHE BOOL "Parallel build with as many threads as possible!")
if(MULTITHREADED_BUILD)
    if(${CMAKE_GENERATOR} MATCHES "MinGW Makefiles")
        # Use MAKEFLAGS to specify the number of jobs for parallel build
        set(MAKEFLAGS "-j${NUMBER_OF_PROCESSORS}")
        set(ENV{MAKEFLAGS} "${MAKEFLAGS}")
        MESSAGE(STATUS "Set MAKEFLAGS for MinGW Makefiles to ${MAKEFLAGS}")
    elseif(MSVC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        MESSAGE(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    endif()
endif()

function(apply_compile_options)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    foreach(target IN LISTS ARG_TARGETS)
        # check if the target is an ALIAS
        get_target_property(original_target ${target} ALIASED_TARGET)
            
        if (original_target)
            message(STATUS "Applying compile options to alias target '${target}' (original: '${original_target}')")
            set(target ${original_target})
        endif()
        
        if(MSVC)
            target_compile_options(${target} PRIVATE
                /W4

                # Debug flags
                "$<$<CONFIG:Debug>:/Zi>"
                "$<$<CONFIG:Debug>:/Od>"
                "$<$<CONFIG:Debug>:/RTC1>"
                "$<$<CONFIG:Debug>:/MDd>"
                "$<$<CONFIG:Debug>:/EHsc>"
                "$<$<CONFIG:Debug>:/DEBUG>"
                "$<$<CONFIG:Debug>:/DYNAMICBASE>"
                "$<$<CONFIG:Debug>:/analyze>"
                "$<$<CONFIG:Debug>:/Oy->"

                # Release flags
                "$<$<CONFIG:Release>:/O2>"
                "$<$<CONFIG:Release>:/Zi>"
                "$<$<CONFIG:Release>:/Zo>"
                "$<$<CONFIG:Release>:/Oy->"
                "$<$<CONFIG:Release>:/MT>"
                "$<$<CONFIG:Release>:/DNDEBUG>"
            )
            target_link_options(${target} PRIVATE
                "$<$<CONFIG:Debug>:/DEBUG>"       # Debugger-optimized linking (Debug)
                "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>" # no terminale
                "$<$<CONFIG:Release>:/DEBUG>"     # Include debug symbols in Release
                "$<$<CONFIG:Release>:/OPT:REF>"   # Optimize unused functions
                "$<$<CONFIG:Release>:/OPT:ICF>"   # Identical COMDAT folding
                "$<$<CONFIG:RELEASE>:/INCREMENTAL:NO>"  # Disable incremental linking
            )
        else()
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wno-cast-function-type
                -fno-exceptions
                
                # Debug flags
                "$<$<CONFIG:Debug>:-g3>"
                "$<$<CONFIG:Debug>:-ggdb>"
                "$<$<CONFIG:Debug>:-O0>"
                "$<$<CONFIG:Debug>:-fstack-protector-strong>"
                "$<$<CONFIG:Debug>:-funwind-tables>"

                # Release flags
                "$<$<CONFIG:Release>:-O3>"
                "$<$<CONFIG:Release>:-g>"
                "$<$<CONFIG:Release>:-DNDEBUG>"
                "$<$<CONFIG:Release>:-march=native>"
                "$<$<CONFIG:Release>:-mwindows>"
                "$<$<CONFIG:Release>:-DNDEBUG>"
                "$<$<CONFIG:Release>:-fno-omit-frame-pointer>"
            )
        endif()
    endforeach()
endfunction()