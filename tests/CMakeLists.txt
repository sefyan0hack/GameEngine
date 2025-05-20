
file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

add_executable(tests ${TEST_SOURCES})

if(ONLINE)
  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    VERSION 1.16.0
    OPTIONS "BUILD_GMOCK OFF"
            "INSTALL_GTEST OFF"
            "BUILD_SHARED_LIBS ON"
  )
else()
  set(BUILD_GMOCK OFF CACHE BOOL   "Disable GMock build" FORCE)
  set(INSTALL_GTEST OFF CACHE BOOL "Disable GTEST INSTALL" FORCE)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "build Shared gtest" FORCE)

  add_subdirectory(
    ${CMAKE_SOURCE_DIR}/3party/googletest-1.16.0
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-1.16.0-build
  )
endif()

target_link_libraries(tests PRIVATE core gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)