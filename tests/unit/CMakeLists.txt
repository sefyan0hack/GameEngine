include(GoogleTest)
include(helpers)

add_executable(unit_tests)

target_sources(unit_tests 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Function.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.cpp
)

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/3party/googletest
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
)

target_link_libraries(unit_tests 
    PRIVATE
        Engine
        GTest::gtest GTest::gmock GTest::gtest_main
)

target_link_staticly(unit_tests PRIVATE)
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)
