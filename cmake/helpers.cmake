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

        set(MSVC_WARNING_FLAGS /W4 /permissive-)

        set(GNU_WARNING_FLAGS
            -pedantic -Wno-cast-function-type -Winit-self -Wcast-qual
            -Wsuggest-final-types -Wsuggest-final-methods -Wno-changes-meaning
        )

        set(CLANG_WARNING_FLAGS -Wno-language-extension-token)

        set(CLANG_AND_GNU_WARNING_FLAGS
            -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wwrite-strings -Wno-sign-conversion -Werror=null-dereference 
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
        
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${target} PRIVATE
                "$<$<BOOL:${ENABLE_COVERAGE}>:-fprofile-instr-generate>"
                "$<$<BOOL:${ENABLE_COVERAGE}>:-fcoverage-mapping>"
                "$<$<BOOL:${ENABLE_COVERAGE}>:-fcoverage-mcdc>"
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                "$<$<BOOL:${ENABLE_COVERAGE}>:--coverage>"
                "$<$<BOOL:${ENABLE_COVERAGE}>:-fno-omit-frame-pointer>"
                "$<$<BOOL:${ENABLE_COVERAGE}>:-fno-elide-constructors>" # Track copy/move constructors
                "$<$<BOOL:${ENABLE_COVERAGE}>:-fprofile-abs-path>" # Absolute paths for gcovr
                "$<$<BOOL:${ENABLE_COVERAGE}>:-fno-inline>"
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
        if(MSVC)
            if(ASAN)
                target_compile_options(${target} PRIVATE /fsanitize=address)
            endif()

            if(USAN OR TSAN)
                message(STATUS "USAN OR TSAN NOT WORKING IN `MSVC` !!")
            endif()
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target} PRIVATE
                "$<$<BOOL:${ASAN}>:-fsanitize=address>"
                "$<$<BOOL:${USAN}>:-fsanitize=undefined>"
                "$<$<BOOL:${TSAN}>:-fsanitize=thread>"
            )
            target_link_options(${target} PUBLIC
                "$<$<BOOL:${ASAN}>:-fsanitize=address>"
                "$<$<BOOL:${USAN}>:-fsanitize=undefined>"
                "$<$<BOOL:${TSAN}>:-fsanitize=thread>"
            )
        endif()
        if(ASAN OR USAN OR TSAN AND NOT WIN32 )
            target_link_options(${target} PUBLIC
                "$<$<CXX_COMPILER_ID:GNU>:-static-libasan;-static-libubsan;-static-liblsan>"
                "$<$<CXX_COMPILER_ID:Clang>:-static-libsan>"
            )
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
                )
            endif()
            if(HARDEN AND CMAKE_SIZEOF_VOID_P EQUAL 4)  # 32-bit
                    target_compile_options(${target} PRIVATE /SafeSEH)
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            if(NOT WIN32)
                if(COMPILER_MAJOR_VERSION STREQUAL "14")
                    target_compile_options(${target} PRIVATE
                        "$<$<BOOL:${HARDEN}>:-fhardened>"
                        "$<$<BOOL:${HARDEN}>:-Wno-hardened>"
                    )
                endif()
                target_compile_options(${target} PRIVATE
                    "$<$<BOOL:${HARDEN}>:-fharden-compares>"
                    "$<$<BOOL:${HARDEN}>:-fharden-conditional-branches>"
                )
            endif()
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

        apply_warning_options(TARGETS ${target})
    endforeach()
    pre_include_file("${CMAKE_SOURCE_DIR}/Engine/core/Platform.hpp")
endfunction()


function(rtti V)
    if(V)
    add_compile_options(
        "$<$<CXX_COMPILER_ID:MSVC>:/GR>"
        "$<$<CXX_COMPILER_ID:Clang,GNU>:-frtti>"
    )
    else()
    add_compile_options(
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

# Remove -nostdlib and -nostartfiles from compile options
macro(remove_global_flags target)
    get_target_property(TARGET_OPTS ${target} COMPILE_OPTIONS)
    if(TARGET_OPTS)
        list(FILTER TARGET_OPTS EXCLUDE REGEX "^-nostdlib$")
        list(FILTER TARGET_OPTS EXCLUDE REGEX "^-nostartfiles$")
        set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "${TARGET_OPTS}")
    endif()
endmacro()

#
macro(fetch_and_include_file name url)
    CPMAddPackage(
        NAME ${name}
        URL ${url}
        DOWNLOAD_ONLY YES
        DOWNLOAD_NO_EXTRACT TRUE
        DOWNLOAD_NAME ${name}.h
    )
    include_directories(${${name}_SOURCE_DIR})
endmacro()

function(no_console)
    if(MSVC)
    add_link_options(
        "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
        "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>"
    )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_link_options(
        "$<$<AND:$<CONFIG:Release>,$<STREQUAL:$<PLATFORM_ID>,Windows>>:-Wl,--subsystem,windows>"
    )
    endif()
endfunction()


function(embed_resource resource_file source_file variable_name)
    # Check if source is up-to-date
    if(EXISTS "${source_file}" AND
       NOT "${resource_file}" IS_NEWER_THAN "${source_file}")
        return()
    endif()

    # Read binary file as hex
    file(READ "${resource_file}" hex_content HEX)

    # Calculate total bytes and handle empty files
    string(LENGTH "${hex_content}" hex_length)
    math(EXPR byte_count "${hex_length} / 2")

    # Format hex values with 16 bytes per line
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " formatted_content "${hex_content}")
    string(REGEX REPLACE ", $" "" formatted_content "${formatted_content}")  # Remove trailing comma

    # Split into lines of 16 bytes
    string(REGEX REPLACE "(0x[0-9a-f][0-9a-f], ){16}" "\\0\n" formatted_content "${formatted_content}")

    # Generate source content
    set(array_definition "static constexpr std::array<unsigned char, ${byte_count}> ${variable_name} = {{\n${formatted_content}\n}};")
    set(source_content "// Auto-generated from ${resource_file}\n#include <array>\n\n${array_definition}\n")

    # Write output file
    file(WRITE "${source_file}" "${source_content}")
endfunction()