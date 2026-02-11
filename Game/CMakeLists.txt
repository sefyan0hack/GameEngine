
include(Pack)

if(ANDROID)
  add_library(Game)
else()
  add_executable(Game)
endif()

target_sources( Game PRIVATE Game.cpp )

target_link_libraries(Game PRIVATE Engine)

target_pack(Game)