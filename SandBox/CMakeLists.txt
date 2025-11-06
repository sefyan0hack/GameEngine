
include(helpers)

add_executable(SandBox)
add_library(Game SHARED)

target_sources( SandBox PRIVATE
    main.cpp 
)
target_sources( Game PRIVATE
    Game.cpp 
)

target_link_libraries(Game PRIVATE glm::glm Engine )

apply_all_options(TARGETS SandBox)
apply_coverage_options(TARGETS SandBox)
apply_harden_options(TARGETS SandBox)
apply_sanitizer_options(TARGETS SandBox)

print_target_compile_options(SandBox)
