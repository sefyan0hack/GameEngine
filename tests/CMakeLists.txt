
file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE core gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

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
else()
  include(CodeCoverage)
  append_coverage_compiler_flags()
  append_coverage_compiler_flags_to_target(core)
  set(GCOVR_ADDITIONAL_ARGS "--calls;--branches;--decisions;--exclude-function-lines;--exclude-noncode-lines;--exclude-unreachable-branches;--exclude-throw-branches;--delete")
  set(COVERAGE_EXCLUDES "${CMAKE_SOURCE_DIR}/3party/*" "${CMAKE_SOURCE_DIR}/tests/*")
  setup_target_for_coverage_gcovr_html(
    NAME coverage
    EXECUTABLE ctest
    DEPENDS core tests
  )
endif()
