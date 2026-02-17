
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

macro(collect_plt_sources tgt)
target_include_directories(${tgt} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/platform/${CMAKE_SYSTEM_NAME}") # platform folder
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/platform/${CMAKE_SYSTEM_NAME}" PLT_SOURCE) # files in the platform folder

if(UNIX)
  target_include_directories(${tgt} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/platform/Unix")
  aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/platform/Unix" UNIX_SOURCE)
  list(APPEND PLT_SOURCE ${UNIX_SOURCE})
endif()
endmacro()


####################################################################################################
# Static link std lib
####################################################################################################

macro(target_link_libstdxx_staticly tgt scope)
target_link_options( ${tgt} ${scope}
    $<$<CXX_COMPILER_ID:GNU,Clang>:-static-libstdc++>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-static-libgcc>
)
endmacro()
