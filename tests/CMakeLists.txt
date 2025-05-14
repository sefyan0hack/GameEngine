
file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

add_executable(tests ${TEST_SOURCES})

if(ONLINE)
  CPMAddPackage("gh:google/googletest@1.16.0")
else()
  add_subdirectory(
    ${CMAKE_SOURCE_DIR}/3party/googletest-1.16.0
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-1.16.0-build
  )
endif()

target_link_libraries(tests PRIVATE core gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

include(CodeCoverage)

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
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  if(WIN32)
    find_program(GCOVR_PATH gcovr.exe)
  elseif(UNIX)
    find_program(GCOVR_PATH gcovr)
  endif()
  if(GCOVR_PATH)
    set(GCOVR_ADDITIONAL_ARGS "--calls;--branches;--decisions;--exclude-function-lines;--exclude-noncode-lines;--exclude-unreachable-branches;--exclude-throw-branches;--delete")
    set(COVERAGE_EXCLUDES "${CMAKE_SOURCE_DIR}/3party/*" "${CMAKE_SOURCE_DIR}/tests/*")
    append_coverage_compiler_flags()
    append_coverage_compiler_flags_to_target(core)
    message(STATUS "[Code Coverage `ON`]")
    setup_target_for_coverage_gcovr_html(
      NAME coverage_html
      EXECUTABLE ctest
      DEPENDS core tests
    )
    setup_target_for_coverage_gcovr_xml(
      NAME coverage_xml
      EXECUTABLE ctest
      DEPENDS core tests
    )
    # delete_files_by_extension("${CMAKE_BINARY_DIR}" "gcda")
  else()
  message(STATUS "conseder doing `pip install gcovr` if you want code covegare")
  endif()
endif()
