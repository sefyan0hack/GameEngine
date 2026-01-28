if(COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC Flags
        set(GNU_COVERAGE_FLAGS
            --coverage
            -fno-inline
            -fno-inline-small-functions
            -fno-default-inline
        )

        find_program(GCOVR_PATH gcovr REQUIRED)
        find_program(GCOV_PATH NAMES "gcov-${COMPILER_MAJOR_VERSION}" "gcov" REQUIRED)

        if(GCOVR_PATH)
            set(COMMEN_GCOVR_FLAGS
                --root ${CMAKE_SOURCE_DIR}
                --exclude '${CMAKE_SOURCE_DIR}/tests/.*'
                --exclude '${CMAKE_SOURCE_DIR}/3party/.*'
                --exclude '${CMAKE_BINARY_DIR}/.*'
                --exclude '/usr/.*'
                --exclude '[A-Z]:/msys64/.*'
                --exclude '[A-Z]:/mingw64/.*'
                --exclude '.*/include/c++/.*'
                --gcov-executable ${GCOV_PATH}
                --delete
                --exclude-function-lines
                --exclude-unreachable-branches
                --exclude-throw-branches
                --exclude-noncode-lines
                --sort=uncovered-percent
                --merge-mode-functions=merge-use-line-min
                --sort-reverse
                --decisions
                --calls
                -s
            )

            add_custom_target(pre_coverage
                COMMAND ${CMAKE_COMMAND} $<TARGET_FILE:unit_tests>
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS Engine unit_tests
            )

            add_custom_target(coverage
                COMMAND ${GCOVR_PATH} ${COMMEN_GCOVR_FLAGS} --xml -o coverage
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS pre_coverage
                COMMENT "Generating GCC xml coverage report with gcovr..."
            )

            add_custom_target(coverage_html
                COMMAND ${GCOVR_PATH} ${COMMEN_GCOVR_FLAGS} --html-details -o coverage.html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS pre_coverage
                COMMENT "Generating GCC html coverage report with gcovr..."
            )
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang Flags (Source-based coverage)
        set(CLANG_COVERAGE_FLAGS
            -fprofile-instr-generate
            -fcoverage-mapping
            -fcoverage-mcdc
            -fno-inline
        )

        find_program(LLVM_PROFDATA_PATH llvm-profdata REQUIRED)
        find_program(LLVM_COV_PATH llvm-cov REQUIRED)

        add_custom_target(pre_coverage
            COMMAND ${CMAKE_COMMAND} -E remove -f "default.profraw" "coverage.profdata"
            COMMAND ${CMAKE_COMMAND} -E env "LLVM_PROFILE_FILE=default.profraw" "TESTING_ENABLED=1" $<TARGET_FILE:unit_tests>
            COMMAND ${LLVM_PROFDATA_PATH} merge -sparse default.profraw -o coverage.profdata
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS Engine unit_tests
        )

        add_custom_target(coverage
            COMMAND ${LLVM_COV_PATH} export $<TARGET_FILE:unit_tests>
                -instr-profile=coverage.profdata
                -format=lcov
                "-ignore-filename-regex=(tests/|3party/)"
                -output-file=${CMAKE_BINARY_DIR}/coverage.lcov
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS pre_coverage
        )

    endif()
endif()

add_library(project_coverage_flags INTERFACE)

target_compile_options(project_coverage_flags INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:${GNU_COVERAGE_FLAGS}>
    $<$<CXX_COMPILER_ID:Clang>:${CLANG_COVERAGE_FLAGS}>
)

target_link_options(project_coverage_flags INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:--coverage>
    $<$<CXX_COMPILER_ID:Clang>:-fprofile-instr-generate>
)