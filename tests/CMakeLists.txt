enable_testing()

file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

add_executable(tests ${TEST_SOURCES})

target_link_libraries(tests PRIVATE core gtest gtest_main)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(DISCOVERY_MODE  ON CACHE BOOL PRE_TEST FORCE)

if(ONLINE)
        CPMAddPackage("gh:google/googletest@1.17.0")
else()
        add_subdirectory(
            ${CMAKE_SOURCE_DIR}/3party/googletest-1.16.0
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-1.16.0-build
        )
endif()

include(GoogleTest)
gtest_discover_tests(tests
        EXTRA_ARGS --gtest_num_threads=1
        PROPERTIES RUN_SERIAL TRUE
)

if (WIN32)
        # HACK: we can't use TARGET_RUNTIME_DLLS because simbody doesn't package libgfortran etc. correctly
        add_custom_command(
        TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_RUNTIME_DLLS:tests>
                $<TARGET_FILE_DIR:tests>
        VERBATIM
        COMMAND_EXPAND_LISTS
        )
endif()