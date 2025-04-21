
file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE core gtest gtest_main)

# Register the test with CTest
include(GoogleTest)
gtest_discover_tests(tests)

# set up a helper variable to detect Debug mode in single‐config generators
if(NOT CMAKE_CONFIGURATION_TYPES)
  set(IS_DEBUG   "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
else()
  set(IS_DEBUG   TRUE)
endif()

if(IS_DEBUG)
  find_program(GCOVR_EXECUTABLE gcovr)
  if(GCOVR_EXECUTABLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(core PRIVATE 
      "$<$<CONFIG:Debug>:--coverage>"
      "$<$<CONFIG:Debug>:-fprofile-arcs>"
      "$<$<CONFIG:Debug>:-ftest-coverage>"
      "$<$<CONFIG:Debug>:-fkeep-static-functions>"
      "$<$<CONFIG:Debug>:-fno-inline>"
      "$<$<CONFIG:Debug>:-fno-elide-constructors>"
      "$<$<CONFIG:Debug>:-fno-inline-small-functions>"
      "$<$<CONFIG:Debug>:-fno-default-inline>"
      "$<$<CONFIG:Debug>:-fprofile-abs-path>"
    )
    target_link_options(core PUBLIC 
      "$<$<CONFIG:Debug>:--coverage>"
      "$<$<CONFIG:Debug>:-fprofile-arcs>"
      "$<$<CONFIG:Debug>:-ftest-coverage>"
      "$<$<CONFIG:Debug>:-fkeep-static-functions>"
      "$<$<CONFIG:Debug>:-fno-inline>"
      "$<$<CONFIG:Debug>:-fno-elide-constructors>"
      "$<$<CONFIG:Debug>:-fno-inline-small-functions>"
      "$<$<CONFIG:Debug>:-fno-default-inline>"
      "$<$<CONFIG:Debug>:-fprofile-abs-path>"
    )
    add_custom_target(coverage
      COMMENT "Generating coverage report"
      COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure  # Run tests first
      COMMAND ${GCOVR_EXECUTABLE}
      -r ${CMAKE_SOURCE_DIR}
      --html --html-details --html-title "UnitTest coverage"
      --calls
      --branches
      --exclude ".*/3party/.*"
      --exclude ".*\\.inl"
      --decisions
      --exclude-function-lines
      --exclude-noncode-lines
      --delete
      -o coverage.html
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS core tests
    )
  endif()

  find_program(OPENCPPCOVERAGE_PATH OpenCppCoverage.exe)
  if(OPENCPPCOVERAGE_PATH AND MSVC)
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
endif() #IS_DEBUG
