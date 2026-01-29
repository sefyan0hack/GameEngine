
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


function(fetch_and_include_file name url)
    if(${ARGC} GREATER 2)
        set(sub_dir "${ARGV2}")
        set(dest_dir "${CMAKE_BINARY_DIR}/${sub_dir}")
        
        if(NOT EXISTS "${dest_dir}")
            file(MAKE_DIRECTORY "${dest_dir}")
        endif()
        
        set(SOURCE_DIR_ARG SOURCE_DIR "${dest_dir}")
        
        set(INCLUDE_PATH_ "${CMAKE_BINARY_DIR}")
    else()
        set(SOURCE_DIR_ARG "")
        set(INCLUDE_PATH_ "${${name}_SOURCE_DIR}")
    endif()

    CPMAddPackage(
        NAME ${name}
        URL ${url}
        ${SOURCE_DIR_ARG}
        DOWNLOAD_ONLY YES
        DOWNLOAD_NO_EXTRACT TRUE
        DOWNLOAD_NAME ${name}.h
    )

    include_directories("${INCLUDE_PATH_}")
endfunction()