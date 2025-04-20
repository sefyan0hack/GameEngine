

find_program(GCOVR_EXECUTABLE gcovr)
if(GCOVR_EXECUTABLE AND NOT MSVC)
  add_custom_target(coverage
    COMMENT "Generating coverage report"
    COMMAND ${GCOVR_EXECUTABLE}
      -r ${CMAKE_SOURCE_DIR}
      --html --html-details
      --exclude ".*/3party/.*"
      --exclude ".*\\.h"
      --exclude ".*\\.hpp"
      --exclude ".*\\.inl"
      -o coverage.html
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${PROJECT_NAME}
  )

endif()

if(NOT MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(core PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(core PUBLIC --coverage -fprofile-arcs -ftest-coverage)
endif()

file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")
set(ALL_TEST_TARGETS "")

foreach(src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${src} NAME_WE)
    list(APPEND ALL_TEST_TARGETS ${test_name})
    add_executable(${test_name} ${src})
    target_link_libraries(${test_name} PRIVATE core)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

if(MSVC)
  find_program(OPENCPPCOVERAGE_PATH OpenCppCoverage.exe)
  if(OPENCPPCOVERAGE_PATH)
    # Convert paths to native Windows format
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}" NATIVE_SOURCE_DIR)
    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage_report" NATIVE_OUTPUT_DIR)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/3party" NATIVE_3PARTY_PATTERN)
    
    set(EXCLUDED_SOURCES
      --excluded_sources "${NATIVE_3PARTY_PATTERN}\\*"
      --excluded_sources "*.h"
      --excluded_sources "*.hpp"
      --excluded_sources "*.inl"
    )

    # Initialize coverage commands
    set(COVERAGE_COMMANDS 
      COMMAND ${CMAKE_COMMAND} -E make_directory "${NATIVE_OUTPUT_DIR}"
    )

    # Generate coverage files for each test executable
    set(COVERAGE_FILES "")
    foreach(TEST_TARGET ${ALL_TEST_TARGETS})
        # Generate coverage file path
        set(COV_FILE "${NATIVE_OUTPUT_DIR}/coverage_${TEST_TARGET}.cov")
        file(TO_NATIVE_PATH "${COV_FILE}" NATIVE_COV_FILE)
        list(APPEND COVERAGE_FILES "${NATIVE_COV_FILE}")

        # Add coverage command using generator expressions
        list(APPEND COVERAGE_COMMANDS
          COMMAND ${OPENCPPCOVERAGE_PATH}
            --sources "${NATIVE_SOURCE_DIR}"
            ${EXCLUDED_SOURCES}
            --export_type binary:${NATIVE_COV_FILE}
            -- "$<TARGET_FILE:${TEST_TARGET}>"
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TEST_TARGET}>"
        )
    endforeach()

    # Build merge command with multiple --input_coverage args
    set(MERGE_COMMAND ${OPENCPPCOVERAGE_PATH})
    foreach(COV_FILE ${COVERAGE_FILES})
        list(APPEND MERGE_COMMAND --input_coverage "${COV_FILE}")
    endforeach()
    list(APPEND MERGE_COMMAND
        --export_type binary:${NATIVE_OUTPUT_DIR}/merged.cov
        --
    )

    # Add commands to coverage target
    list(APPEND COVERAGE_COMMANDS
        # Merge coverage files
        COMMAND ${MERGE_COMMAND}
        
        # Generate HTML report
        COMMAND ${OPENCPPCOVERAGE_PATH}
            --input_coverage ${NATIVE_OUTPUT_DIR}/merged.cov
            --export_type html:${NATIVE_OUTPUT_DIR}
            --
        
        # Cleanup intermediate files
        COMMAND ${CMAKE_COMMAND} -E remove ${COVERAGE_FILES}
        COMMAND ${CMAKE_COMMAND} -E remove ${NATIVE_OUTPUT_DIR}/merged.cov
    )

    # Create custom coverage target
    add_custom_target(coverage
      ${COVERAGE_COMMANDS}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Running OpenCppCoverage for all tests and generating merged report..."
      DEPENDS ${ALL_TEST_TARGETS} core
    )
  endif()
endif()