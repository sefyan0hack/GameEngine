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
            message(STATUS "++++++[MSVC]++++++ flags")
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
            )
            target_link_options(${target} PRIVATE
                "$<$<CONFIG:Debug>:/DEBUG>"                     # Debugger-optimized linking (Debug)
                "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"       # no terminale
                "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>"    # entry
                "$<$<CONFIG:Release>:/OPT:REF>"                 # Optimize unused functions
                "$<$<CONFIG:Release>:/OPT:ICF>"                 # Identical COMDAT folding
                "$<$<CONFIG:RELEASE>:/INCREMENTAL:NO>"          # Disable incremental linking
            )
            add_definitions(/FI"${CMAKE_SOURCE_DIR}/include/core/Global_H.hpp")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            message(STATUS "++++++[GNU]++++++ flags")
            target_compile_options(${target} PRIVATE
                -Wno-cast-function-type -Winit-self -Wcast-qual
                -Wsuggest-final-types -Wsuggest-final-methods
                -fdevirtualize -ftree-vectorize
                -static-libstdc++
            )
        elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
            message(STATUS "++++++[Clang]++++++ flags")
            target_compile_options(${target} PRIVATE -Wno-language-extension-token
            )
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|.*Clang.*")
        message(STATUS "++++++[GNU|Clang]++++++ flags")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wwrite-strings -Wno-sign-conversion
            -fstack-protector-strong
            -Wnull-dereference -Wswitch-enum
            -Wuninitialized -Wpointer-arith -Wreturn-type -Wredundant-decls
            -fno-operator-names
            -frtti
            
            # Debug flags
            "$<$<CONFIG:Debug>:-ggdb>"
            "$<$<CONFIG:Debug>:-g3>"
            "$<$<CONFIG:Debug>:-O0>"
            "$<$<CONFIG:Debug>:-fno-inline>"
            # Release flags
            "$<$<CONFIG:Release>:-O3>"
            "$<$<CONFIG:Release>:-g1>"
            # "$<$<CONFIG:Release>:-march=native>"  #enable when dnt need to send the app
            "$<$<CONFIG:Release>:-funwind-tables>"
            "$<$<CONFIG:Release>:-fasynchronous-unwind-tables>"
            "$<$<CONFIG:Release>:-ffunction-sections>"
            "$<$<CONFIG:Release>:-fdata-sections>"
        )
        target_link_options(${target} PRIVATE
            -Wl,--noinhibit-exec
            "$<$<AND:$<CONFIG:Release>,$<STREQUAL:$<PLATFORM_ID>,Windows>>:-Wl,--subsystem,windows>"
        )
        
        add_definitions(-include "${CMAKE_SOURCE_DIR}/include/core/Global_H.hpp")
        endif()
    endforeach()
endfunction()

function(delete_files_by_extension DIR EXTENSION)
    file(GLOB_RECURSE FILES_TO_DELETE 
        LIST_DIRECTORIES false
        "${DIR}/**.${EXTENSION}"
        "${DIR}/**/.*.${EXTENSION}"
    )

    if(NOT FILES_TO_DELETE)
        message(STATUS "No *.${EXTENSION} files found in ${DIR}")
        return()
    endif()

    list(LENGTH FILES_TO_DELETE NUM_FILES)
    message(STATUS "Deleting ${NUM_FILES} *.${EXTENSION} files...")

    foreach(FILE IN LISTS FILES_TO_DELETE)
        if(EXISTS "${FILE}")
            file(REMOVE "${FILE}")
            if(VERBOSE)
                message(STATUS "Deleted: ${FILE}")
            endif()
        else()
            if(WARN_ON_MISSING)
                message(WARNING "File not found: ${FILE}")
            endif()
        endif()
    endforeach()
endfunction()