
include(helpers)

add_executable(SandBox main.cpp)

target_link_libraries(SandBox PRIVATE glm::glm Engine)

apply_all_options(TARGETS SandBox)
apply_coverage_options(TARGETS SandBox)
apply_harden_options(TARGETS Engine)

print_target_compile_options(SandBox)
