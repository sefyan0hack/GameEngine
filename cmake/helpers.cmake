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

function(apply_main_options)
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
                "$<$<CONFIG:Release>:/Oy->"
            )
            target_link_options(${target} PRIVATE
                "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"       # no terminale
                "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>"    # entry
                "$<$<CONFIG:Release>:/OPT:REF>"                 # Optimize unused functions
                "$<$<CONFIG:Release>:/OPT:ICF>"                 # Identical COMDAT folding
                "$<$<CONFIG:Release>:/INCREMENTAL:NO>"          # Disable incremental linking
            )
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
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
        endif()
    endforeach()
endfunction()

function(apply_warning_options)
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

        set(MSVC_WARNING_FLAGS /W4)

        set(GNU_WARNING_FLAGS
            -Wno-cast-function-type -Winit-self -Wcast-qual
            -Wsuggest-final-types -Wsuggest-final-methods
        )

        set(CLANG_WARNING_FLAGS -Wno-language-extension-token)

        set(CLANG_AND_GNU_WARNING_FLAGS
            -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wwrite-strings -Wno-sign-conversion
            -Wnull-dereference -Wswitch-enum
            -Wuninitialized -Wpointer-arith -Wreturn-type -Wredundant-decls
            -fno-operator-names
        )

        target_compile_options(${target} PRIVATE 
            "$<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNING_FLAGS}>"
            "$<$<CXX_COMPILER_ID:GNU>:${GNU_WARNING_FLAGS};${CLANG_AND_GNU_WARNING_FLAGS}>"
            "$<$<CXX_COMPILER_ID:Clang>:${CLANG_WARNING_FLAGS};${CLANG_AND_GNU_WARNING_FLAGS}>"
        )    
    endforeach()
endfunction()

function(apply_coverage_options)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    foreach(target IN LISTS ARG_TARGETS)
        get_target_property(original_target ${target} ALIASED_TARGET)
            
        if (original_target)
            message(STATUS "Applying compile options to alias target '${target}' (original: '${original_target}')")
            set(target ${original_target})
        endif()
        
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            "$<$<BOOL:${ENABLE_COVERAGE}>:--coverage>"
            "$<$<BOOL:${ENABLE_COVERAGE}>:-fno-omit-frame-pointer>"
            "$<$<BOOL:${ENABLE_COVERAGE}>:-fno-inline>"
            "$<$<BOOL:${ENABLE_COVERAGE}>:-pg>"
        )
        target_link_options(${target} PRIVATE
            "$<$<BOOL:${ENABLE_COVERAGE}>:--coverage>"
            "$<$<BOOL:${ENABLE_COVERAGE}>:-pg>"
            "$<$<BOOL:${ENABLE_COVERAGE}>:-no-pie>"
        )
        endif()
    endforeach()
endfunction()

function(apply_sanitizer_options)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    foreach(target IN LISTS ARG_TARGETS)
        get_target_property(original_target ${target} ALIASED_TARGET)
            
        if (original_target)
            message(STATUS "Applying compile options to alias target '${target}' (original: '${original_target}')")
            set(target ${original_target})
        endif()

        if(NOT "${SANITIZER}" STREQUAL "")
            if(MSVC)
                if("${SANITIZER}" MATCHES ".*address.*")
                    target_compile_options(${target} PRIVATE /fsanitize=address)
                endif()
            elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options(${target} PRIVATE -fsanitize=${SANITIZER})
            endif()

            if(NOT "${SANITIZER}" STREQUAL "")
            target_link_options(${target} PRIVATE
                "$<$<CXX_COMPILER_ID:GNU>:-static-libasan -static-libtsan -static-liblsan -static-libubsan>"
                "$<$<CXX_COMPILER_ID:Clang>:-static-libsan>"
            )
            endif()
        endif()
    endforeach()
endfunction()

function(apply_harden_options)
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
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|x64|x86|AMD64")
                target_compile_options(${target} PRIVATE
                    "$<$<BOOL:${HARDEN}>:/sdl>"
                    "$<$<BOOL:${HARDEN}>:/GS>"
                    "$<$<BOOL:${HARDEN}>:/guard:cf>"
                    "$<$<BOOL:${HARDEN}>:/dynamicbase>"
                )
            endif()
            if(HARDEN AND CMAKE_SIZEOF_VOID_P EQUAL 4)  # 32-bit
                    target_compile_options(${target} PRIVATE /SafeSEH)
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                "$<$<BOOL:${HARDEN}>:-fhardened>"
                "$<$<BOOL:${HARDEN}>:-Wno-hardened>"
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            
            target_compile_options(${target} PRIVATE
                "$<$<BOOL:${HARDEN}>:-Wformat;-Wformat-security;-Werror=format-security;-fno-strict-aliasing;-fno-common;-fstack-protector-all>"
            )

            if(HARDEN)
                target_link_options(${target} PRIVATE
                    "$<$<STREQUAL:$<PLATFORM_ID>,Linux>:-Wl,-z,relro;-Wl,-z,now;-Wl,-z,shstk;-Wl,-z,notext>"
                )
            endif()
        endif()
        if(HARDEN)
            if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                set_target_properties(${target} PROPERTIES
                    POSITION_INDEPENDENT_CODE ON
                    INTERPROCEDURAL_OPTIMIZATION $<IF:$<CONFIG:Release>,TRUE,FALSE>
                )

                target_compile_definitions(${target} PRIVATE
                    _FORTIFY_SOURCE=2
                )
            endif()
        endif()

    endforeach()
endfunction()

function(apply_all_options)
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

        apply_main_options(TARGETS ${target})
        apply_warning_options(TARGETS ${target})
        apply_sanitizer_options(TARGETS ${target})
        apply_coverage_options(TARGETS ${target})
        apply_harden_options(TARGETS ${target})
    endforeach()
    pre_include_file("${CMAKE_SOURCE_DIR}/include/core/Global_H.hpp")
endfunction()


function(rtti V)
    if(V)
    add_compile_options(
        #disable rtti
        "$<$<CXX_COMPILER_ID:MSVC>:/GR>"
        "$<$<CXX_COMPILER_ID:Clang,GNU>:-frtti>"
    )
    else()
    add_compile_options(
        #disable rtti
        "$<$<CXX_COMPILER_ID:MSVC>:/GR->"
        "$<$<CXX_COMPILER_ID:Clang,GNU>:-fno-rtti>"
    )
    endif()
endfunction(rtti)

function(pre_include_file file)
    if(MSVC)
        add_definitions(/FI"${file}")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_definitions(-include "${file}")
    endif()
endfunction(pre_include_file)


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