
include(helpers)

add_library(input)

target_sources( input PRIVATE
    Keyboard.cpp
    Mouse.cpp
)

target_sources( input PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
      Keyboard.hpp
      Mouse.hpp
)

target_include_directories(input PRIVATE ${CMAKE_SOURCE_DIR}/Engine)

apply_all_options(TARGETS input)
apply_coverage_options(TARGETS input)
apply_harden_options(TARGETS input)
apply_sanitizer_options(TARGETS input)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(NOT WIN32)
    target_link_libraries(input PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(input
        PRIVATE
            Threads::Threads
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
    )
endif()