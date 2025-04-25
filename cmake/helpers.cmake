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
                /bigobj
                # Debug flags
                "$<$<CONFIG:Debug>:/Z7>"
                "$<$<CONFIG:Debug>:/Od>"
                "$<$<CONFIG:Debug>:/RTC1>"
                "$<$<CONFIG:Debug>:/EHsc>"
                "$<$<CONFIG:Debug>:/DYNAMICBASE>"
                "$<$<CONFIG:Debug>:/analyze>"
                "$<$<CONFIG:Debug>:/Oy->"

                # Release flags
                "$<$<CONFIG:Release>:/O2>"
                "$<$<CONFIG:Release>:/Zi>"
                "$<$<CONFIG:Release>:/Zo>"
                "$<$<CONFIG:Release>:/Oy->"
                "$<$<CONFIG:Release>:/DNDEBUG>"
            )
            target_link_options(${target} PRIVATE
                "$<$<CONFIG:Debug>:/DEBUG>"       # Debugger-optimized linking (Debug)
                "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>" # no terminale
                "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>" # entry
                "$<$<CONFIG:Release>:/OPT:REF>"   # Optimize unused functions
                "$<$<CONFIG:Release>:/OPT:ICF>"   # Identical COMDAT folding
                "$<$<CONFIG:RELEASE>:/INCREMENTAL:NO>"  # Disable incremental linking
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wno-cast-function-type -Wfloat-equal -Winit-self -Wcast-qual -Wwrite-strings
                # -Wmissing-declarations
                -fno-exceptions
                -fstack-protector-strong
                -Wlogical-op -Wnull-dereference -Wswitch-enum
                -Wsuggest-final-types -Wsuggest-final-methods
                -fdevirtualize -ftree-vectorize
                -Wuninitialized -Wpointer-arith -Wreturn-type -Winline -Wredundant-decls
                -fno-operator-names
                # Debug flags
                "$<$<CONFIG:Debug>:-ggdb>"
                "$<$<CONFIG:Debug>:-g3>"
                "$<$<CONFIG:Debug>:-O0>"
                "$<$<CONFIG:Debug>:-fno-inline>"
                "$<$<CONFIG:Debug>:-fstack-protector-strong>"
                "$<$<CONFIG:Debug>:-funwind-tables>"
                "$<$<CONFIG:Debug>:-fno-optimize-sibling-calls>"
                "$<$<CONFIG:Debug>:-fno-elide-constructors>"
                "$<$<CONFIG:Debug>:-D_FORTIFY_SOURCE=2>"

                # Release flags
                "$<$<CONFIG:Release>:-O3>"
                "$<$<CONFIG:Release>:-g1>"
                # "$<$<CONFIG:Release>:-march=native>"  #enable when dnt need to send the app
                "$<$<CONFIG:Release>:-DNDEBUG>"
                "$<$<CONFIG:Release>:-fno-omit-frame-pointer>"
                "$<$<CONFIG:Release>:-funwind-tables>"
                "$<$<CONFIG:Release>:-fasynchronous-unwind-tables>"
                "$<$<CONFIG:Release>:-ffunction-sections>"
                "$<$<CONFIG:Release>:-fdata-sections>"
                )
            target_link_options(${target} PRIVATE
                "$<$<CONFIG:Release>:-Wl,--subsystem,windows>"
                "$<$<CONFIG:Release>:-flto>"
                "$<$<CONFIG:Release>:-Wl,--as-needed>"
                "$<$<CONFIG:Release>:-Wl,--gc-sections>"
                "$<$<CONFIG:Release>:-Wl,-Bsymbolic-functions>"
            )
        else()
            target_compile_options(${target} PRIVATE "$<$<CONFIG:Debug>:-ftime-trace>")
        endif()
    endforeach()
endfunction()