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
                "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"       # no terminale
                "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>"    # entry
                "$<$<CONFIG:Release>:/OPT:REF>"                 # Optimize unused functions
                "$<$<CONFIG:Release>:/OPT:ICF>"                 # Identical COMDAT folding
                "$<$<CONFIG:Release>:/INCREMENTAL:NO>"          # Disable incremental linking
            )
            add_definitions(/FI"${CMAKE_SOURCE_DIR}/include/core/Global_H.hpp")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -Wno-cast-function-type -Winit-self -Wcast-qual
                -Wsuggest-final-types -Wsuggest-final-methods
            )
            if(NOT "${SANITIZER}" STREQUAL "")
                add_link_options(
                    -static-libasan -static-libtsan -static-liblsan -static-libubsan 
                )
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${target} PRIVATE -Wno-language-extension-token)
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wwrite-strings -Wno-sign-conversion
            -Wnull-dereference -Wswitch-enum
            -Wuninitialized -Wpointer-arith -Wreturn-type -Wredundant-decls
            -fno-operator-names
            -fno-rtti #disable rtti

            # Debug flags
            "$<$<CONFIG:Debug>:-g>"
            "$<$<CONFIG:Debug>:-ggdb>"
            "$<$<CONFIG:Debug>:-O0>"
            # Release flags
            "$<$<CONFIG:Release>:-O3>"
        )
        target_link_options(${target} PRIVATE
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


function(apply_harden_options)
    if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    set(options)
    set(oneValueArgs)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    foreach(target IN LISTS ARG_TARGETS)
        # check if the target is an ALIAS
        get_target_property(original_target ${target} ALIASED_TARGET)
        get_target_property(type ${target} TYPE)

        if (original_target)
            message(STATUS "Applying compile options to alias target '${target}' (original: '${original_target}')")
            set(target ${original_target})
        endif()
        
        if(MSVC)
            if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64|x64|x86|AMD64")
                target_compile_options(${target} PRIVATE
                    /sdl /GS /SafeSEH /guard:cf /dynamicbase
                )
                target_link_options(${target} PRIVATE
                    /guard:cf
                )
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -fhardened -Wno-hardened
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) 
            
            target_compile_options(${target} PRIVATE
                -Wformat -Wformat-security -Werror=format-security -fno-strict-aliasing -fno-common
                -fstack-protector-all
            )
            target_link_options(${target} PRIVATE
                "$<$<STREQUAL:$<PLATFORM_ID>,Linux>:-Wl,-z,relro;-Wl,-z,now;-Wl,-z,shstk;-Wl,-z,notext>"
            )
        endif()
        set_target_properties(${target}  PROPERTIES POSITION_INDEPENDENT_CODE ON)

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            add_definitions(-D_FORTIFY_SOURCE=2)
            add_definitions(-D_GLIBCXX_ASSERTIONS)
        endif()

    endforeach()
    endif()
endfunction()