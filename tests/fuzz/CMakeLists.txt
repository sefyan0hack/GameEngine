include(CPM)

CPMAddPackage(
    NAME fuzztest
    GITHUB_REPOSITORY google/fuzztest
    GIT_TAG 2025-07-28
)

if (FUZZTEST_FUZZING_MODE)
    fuzztest_setup_fuzzing_flags()
endif()

add_executable(fuzz_tests Utils.cpp)

target_link_libraries(fuzz_tests PRIVATE Engine)
link_fuzztest(fuzz_tests)
