
include(GenerateExportHeader)

if(GAME_HOT_RELOADABLE)
  add_library(Game MODULE )
else()
  add_library(Game)
endif()

generate_export_header(Game)

target_sources( Game PRIVATE Game.cpp )

target_include_directories(Game PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Game 
  PRIVATE
    project_warnings_flags
    project_coverage_flags
    project_sanitizer_flags
    project_hardening_flags
    glm::glm
    Engine
)

if(NOT GAME_HOT_RELOADABLE)
  target_link_libraries(Launcher PRIVATE Game)
else()
  add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Game> $<TARGET_FILE_DIR:Launcher>
  )
endif()
