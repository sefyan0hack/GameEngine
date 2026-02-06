
add_executable(fuzz_tests 
    Utils.cpp
)

target_link_libraries(fuzz_tests 
    PRIVATE
        Engine
)