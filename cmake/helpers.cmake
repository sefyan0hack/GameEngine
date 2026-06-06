

macro(collect_plt_sources tgt)
target_include_directories(${tgt} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/platform/${CMAKE_SYSTEM_NAME}") # platform folder
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/platform/${CMAKE_SYSTEM_NAME}" PLT_SOURCE) # files in the platform folder
endmacro()

####################################################################################################
# Static link std lib
####################################################################################################

macro(target_link_staticly tgt scope)
target_link_options( ${tgt} ${scope}
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:-static>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-static-libstdc++>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-static-libgcc>
)

endmacro()

####################################################################################################
# Print host system info
####################################################################################################

function(print_sys_info key)
    cmake_host_system_information(RESULT ${key} QUERY ${key})
    message(STATUS "${key} : ${${key}}")
endfunction()