add_library(project_coverage_flags INTERFACE)

if(COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        find_program(GCOVR_PATH gcovr)
        find_program(GCOV_PATH NAMES "gcov-${COMPILER_MAJOR_VERSION}" "gcov")

        if(GCOVR_PATH)
            set(COMMEN_GCOVR_FLAGS
                --root ${CMAKE_SOURCE_DIR}
                --filter ${CMAKE_SOURCE_DIR}/Engine/.*
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

            add_custom_target(coverage
                COMMAND ${CMAKE_CTEST_COMMAND}
                COMMAND ${GCOVR_PATH} ${COMMEN_GCOVR_FLAGS} --xml -o coverage
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS unit_tests
                COMMENT "Generating GCC xml coverage report with gcovr..."
            )

            add_custom_target(coverage_html
                COMMAND ${CMAKE_CTEST_COMMAND}
                COMMAND ${GCOVR_PATH} ${COMMEN_GCOVR_FLAGS} --html-details -o coverage.html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS unit_tests
                COMMENT "Generating GCC html coverage report with gcovr..."
            )

        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

        find_program(LLVM_PROFDATA llvm-profdata)
        find_program(LLVM_COV llvm-cov)

        set(COVE_PROFRAW_FILE "coverage.profraw")
        set(COVE_PROFDATA_FILE "coverage.profdata")

        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E remove -f "${COVE_PROFRAW_FILE}" "${COVE_PROFDATA_FILE}"
            COMMAND ${CMAKE_COMMAND} -E env "LLVM_PROFILE_FILE=${COVE_PROFRAW_FILE}" $<TARGET_FILE:unit_tests>

            COMMAND ${LLVM_PROFDATA} merge -sparse ${COVE_PROFRAW_FILE} -o ${COVE_PROFDATA_FILE}

            COMMAND ${LLVM_COV} export $<TARGET_FILE:unit_tests>
                -instr-profile=${COVE_PROFDATA_FILE}
                -format=lcov
                ${CMAKE_SOURCE_DIR}/Engine
                > "${CMAKE_BINARY_DIR}/coverage.lcov"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS unit_tests
        )

    endif()

    target_compile_options(project_coverage_flags INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>
        $<$<CXX_COMPILER_ID:Clang>:-fprofile-instr-generate -fcoverage-mapping>
    )

    target_link_options(project_coverage_flags INTERFACE
        $<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>
        $<$<CXX_COMPILER_ID:Clang>:-fprofile-instr-generate -fcoverage-mapping>
    )

endif()