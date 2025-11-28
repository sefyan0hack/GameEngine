# sandbox/cmakelists.txt

include(helpers)
include(GenerateExportHeader)

add_library(Game SHARED)

generate_export_header(Game
    BASE_NAME GAME
    EXPORT_MACRO_NAME GAME_API
    EXPORT_FILE_NAME game_export.h
    STATIC_DEFINE GAME_STATIC
)

target_sources( Game PRIVATE
    Game.cpp 
)
if(EMSCRIPTEN)
  target_link_options(Game PRIVATE -sSIDE_MODULE=1)
endif()
target_include_directories(Game PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(Game PRIVATE glm::glm Engine)


add_custom_command(TARGET Game POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Game> $<TARGET_FILE_DIR:Luncher>
)


apply_all_options(TARGETS Game)
apply_coverage_options(TARGETS Game)
# apply_harden_options(TARGETS Game) # it applys the stack protector and craches du to `detached threads`
apply_sanitizer_options(TARGETS Game)

print_target_compile_options(Game)
