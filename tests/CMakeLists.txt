
file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE core gtest gtest_main)

# Register the test with CTest
include(GoogleTest)
gtest_discover_tests(tests)

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

if(MSVC)
  find_program(OPENCPPCOVERAGE_PATH OpenCppCoverage.exe)
  if(OPENCPPCOVERAGE_PATH)
    # Convert paths to native Windows format
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/src" NATIVE_SOURCE_DIR)
    file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage_report" NATIVE_OUTPUT_DIR)
    file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/3party" NATIVE_3PARTY_PATTERN)
    file(TO_NATIVE_PATH "$<TARGET_FILE:tests>" NATIVE_TESTS)
    
    set(EXCLUDED_SOURCES
        --excluded_sources "${NATIVE_3PARTY_PATTERN}\\*"
        --excluded_sources "*.h"
        --excluded_sources "*.hpp"
        --excluded_sources "*.inl"
        --excluded_sources ".*\\\\predefined C\\+\\+ types.*"
        --excluded_sources ".*\\\\Microsoft Visual Studio.*"
        --excluded_modules ".*msvcp.*.dll"
        --excluded_modules ".*vcruntime.*.dll"
    )
    add_custom_target(coverage
      COMMAND ${CMAKE_COMMAND} -E make_directory "${NATIVE_OUTPUT_DIR}"
      COMMAND ${OPENCPPCOVERAGE_PATH}
        --quiet
        --sources ${NATIVE_SOURCE_DIR}
        --export_type html:${NATIVE_OUTPUT_DIR}
        ${EXCLUDED_SOURCES}
        -- ${NATIVE_TESTS}
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Running OpenCppCoverage..."
      DEPENDS core
    )
  endif()
endif()