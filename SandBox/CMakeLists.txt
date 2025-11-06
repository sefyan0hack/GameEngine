
include(helpers)


add_executable(SandBox)



add_library(Game SHARED)

set_target_properties(Game PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION OFF
    CMAKE_POSITION_INDEPENDENT_CODE ON
    # CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF
)

target_sources( SandBox PRIVATE
    main.cpp 
)
target_sources( Game PRIVATE
    Game.cpp 
)

target_link_libraries(SandBox PRIVATE Engine )
target_link_libraries(Game PRIVATE glm::glm Engine )

apply_all_options(TARGETS SandBox Game)
apply_coverage_options(TARGETS SandBox Game)
apply_harden_options(TARGETS SandBox Game)
apply_sanitizer_options(TARGETS SandBox Game)

print_target_compile_options(SandBox)
print_target_compile_options(Game)
