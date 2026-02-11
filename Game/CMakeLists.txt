
include(GenerateExportHeader)
include(Pack)

add_executable(Game)

generate_export_header(Game)

target_sources( Game PRIVATE Game.cpp )

target_include_directories(Game PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Game 
  PRIVATE
    project_warnings_flags
    project_hardening_flags
    Engine
)

target_pack(Game)