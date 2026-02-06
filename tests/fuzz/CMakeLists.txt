
add_executable(fuzz_tests 
    Utils.cpp
)

target_link_libraries(fuzz_tests PRIVATE Engine)

target_compile_options(fuzz_tests PRIVATE -fsanitize=fuzzer)
target_link_options(fuzz_tests PRIVATE -fsanitize=fuzzer)

add_test(
    NAME fuzz_utils
    COMMAND fuzz_tests
        -runs=1000
        -max_total_time=30
)
