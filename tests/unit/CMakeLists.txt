
file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

add_executable(unit_tests ${TEST_SOURCES})

apply_all_options(TARGETS unit_tests )

if(ONLINE)
    CPMAddPackage(
        URI "gh:google/googletest@1.17.0" 
        OPTIONS "gtest_force_shared_crt OFF" "INSTALL_GTEST OFF" "gtest_hide_internal_symbols ON"
    )
else()
    add_subdirectory(
        ${CMAKE_SOURCE_DIR}/3party/googletest-1.16.0
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-1.16.0-build
    )
endif()

target_link_libraries(unit_tests PRIVATE Engine glm::glm GTest::gtest GTest::gtest_main )

if(NOT EMSCRIPTEN)
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)
endif()