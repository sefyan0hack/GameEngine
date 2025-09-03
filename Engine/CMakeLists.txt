
include(helpers)

add_subdirectory(core)

add_library(Engine INTERFACE)

target_include_directories(Engine INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Engine
    INTERFACE
        core
)

print_target_compile_options(Engine)
