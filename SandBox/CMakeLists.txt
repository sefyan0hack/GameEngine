# sandbox/cmakelists.txt

include(helpers)
include(GenerateExportHeader)

add_library(Game SHARED)

set_target_properties(Game PROPERTIES PREFIX "lib")

generate_export_header(Game
    BASE_NAME GAME
    EXPORT_MACRO_NAME GAME_API
    EXPORT_FILE_NAME game_export.h
    STATIC_DEFINE GAME_STATIC
)

target_sources( Game PRIVATE
    Game.cpp 
)

target_include_directories(Game PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(Game PRIVATE glm::glm Engine)

add_executable(SandBox)

target_sources( SandBox PRIVATE
    main.cpp 
)

target_link_libraries(SandBox PRIVATE Engine)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(NOT WIN32)
    target_link_libraries(SandBox PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(SandBox PRIVATE $<$<CXX_COMPILER_ID:GNU>:stdc++exp>)
endif()

add_custom_command(TARGET SandBox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Engine>
    ${CMAKE_CURRENT_BINARY_DIR} 
)
apply_all_options(TARGETS SandBox Game)
apply_coverage_options(TARGETS SandBox Game)
apply_harden_options(TARGETS SandBox Game)
apply_sanitizer_options(TARGETS SandBox Game)

print_target_compile_options(SandBox)
print_target_compile_options(Game)
