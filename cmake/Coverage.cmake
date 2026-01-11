if(COVERAGE)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set( CLANG_COVERAGE_FLAGS
            -fprofile-instr-generate
            -fcoverage-mapping
            -fcoverage-mcdc
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
                COMMAND ${GCOVR_PATH} --version
                COMMAND ${GCOV_PATH} --version
                COMMAND ${CMAKE_COMMAND} -E env "TESTING_ENABLED=1"  $<TARGET_FILE:unit_tests>
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
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set( GNU_COVERAGE_FLAGS
            --coverage
            -fno-omit-frame-pointer
            -fno-elide-constructors # Track copy/move constructors
            -fprofile-abs-path # Absolute paths for gcovr
            -fno-inline
        )

        # find_program(LCOV_PATH lcov REQUIRED)
        find_program(LLVM_PROFDATA_PATH llvm-profdata REQUIRED)
        find_program(LLVM_COV_PATH llvm-cov REQUIRED)

        add_custom_target(pre_coverage
            COMMAND ${LLVM_COV_PATH} --version
            COMMAND ${CMAKE_COMMAND} -E remove "default.profraw"
            COMMAND ${CMAKE_COMMAND} -E remove "coverage.profdata"
            COMMAND ${CMAKE_COMMAND} -E env "LLVM_PROFILE_FILE=default.profraw" "TESTING_ENABLED=1" $<TARGET_FILE:unit_tests>
            COMMAND ${LLVM_PROFDATA_PATH} merge -sparse default.profraw -o coverage.profdata
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS Engine unit_tests
        )

        add_custom_target(coverage
            COMMAND ${LLVM_COV_PATH} export $<TARGET_FILE:unit_tests>
                -instr-profile=coverage.profdata
                -format=lcov
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/tests/.*'
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/3party/.*'
                > ${CMAKE_BINARY_DIR}/coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS pre_coverage
            COMMENT "Generating lcov coverage report for Codecov.io"
        )

        add_custom_target(coverage_html
            COMMAND ${LLVM_COV_PATH} show $<TARGET_FILE:unit_tests>
                -instr-profile=coverage.profdata
                -output-dir=${CMAKE_BINARY_DIR}/coverage_html
                -format=html
                -Xdemangler=c++filt -Xdemangler=-n
                -show-line-counts-or-regions
                -show-instantiations
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/tests/.*'
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/3party/.*'
                --show-mcdc-summary
                --show-mcdc
                -show-instantiation-summary
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS pre_coverage
            COMMENT "Generating Clang html coverage reports..."
        )
    endif()

endif()


add_library(project_coverage_flags INTERFACE)

target_compile_options(project_coverage_flags INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:${GNU_COVERAGE_FLAGS}>
    $<$<CXX_COMPILER_ID:Clang>:${CLANG_COVERAGE_FLAGS}>
)