
include(helpers)
include(Pack)

if(ANDROID)
  add_library(Game SHARED)
else()
  add_executable(Game)
endif()

target_sources(Game PRIVATE Game.cpp )

target_link_libraries(Game PRIVATE Engine)
target_link_staticly(Game PRIVATE)

if(NOT EMBED_RES)
add_custom_command(TARGET Game POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_newer
        "${CMAKE_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:Game>/res"
)
endif()

target_pack(Game)