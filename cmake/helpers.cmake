
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

    include_directories( SYSTEM "${INCLUDE_PATH_}")
endfunction()