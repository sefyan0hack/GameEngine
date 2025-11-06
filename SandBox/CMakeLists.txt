
include(helpers)


add_executable(SandBox)


set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

# Ensure PIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add these compiler/linker flags
add_compile_options(-fPIC -fno-lto)
add_link_options(-fno-lto)

add_library(Game SHARED)

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
