# sandbox/cmakelists.txt

include(helpers)
include(GenerateExportHeader)

if(EMSCRIPTEN)
  add_executable(Game)
else()
  add_library(Game SHARED)
endif()

target_sources( Game PRIVATE
    Game.cpp 
)

if(EMSCRIPTEN)
  target_compile_options(Game PRIVATE 
    -sSIDE_MODULE=2
    -sWASM=1
    -sEXPORT_ALL=1
  )
  target_link_options(Game PRIVATE 
    -sSIDE_MODULE=2
    -sWASM=1
    -sEXPORT_ALL=1
  )
  set_target_properties(Game PROPERTIES PREFIX "lib")
  set_target_properties(Game PROPERTIES SUFFIX ".so")
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
