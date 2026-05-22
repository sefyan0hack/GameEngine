
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/3party/fuzztest
    ${CMAKE_CURRENT_BINARY_DIR}/fuzztest-build
)

if (FUZZTEST_FUZZING_MODE)
    fuzztest_setup_fuzzing_flags()
endif()

add_executable(fuzz_tests Utils.cpp)

target_link_libraries(fuzz_tests PRIVATE Engine)
link_fuzztest(fuzz_tests)
