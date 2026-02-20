include(GoogleTest)
include(helpers)

add_executable(unit_tests)

target_sources(unit_tests 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Function.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils.cpp
)

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

target_link_libraries(unit_tests 
    PRIVATE
        Engine
        GTest::gtest GTest::gtest_main
)

target_link_staticly(unit_tests PRIVATE)
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)
