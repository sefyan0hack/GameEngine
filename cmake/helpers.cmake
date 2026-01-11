
function(pre_include_file file)
    if(MSVC)
        add_definitions(/FI"${file}")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_definitions(-include "${file}")
    endif()
endfunction(pre_include_file)

function(target_pre_include_file target file)
    if(MSVC)
        target_compile_definitions(${target} PRIVATE /FI"${file}")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_definitions(${target} PRIVATE -include "${file}")
    endif()
endfunction(target_pre_include_file)

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