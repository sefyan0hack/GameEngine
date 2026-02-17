
include(helpers)
include(Pack)

if(ANDROID)
  add_library(Game SHARED)
else()
  add_executable(Game)
endif()

target_sources( Game PRIVATE Game.cpp )

target_link_libraries(Game PRIVATE Engine)
target_link_libstdxx_staticly(Game PRIVATE)

target_pack(Game)