
file(GLOB TEST_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*_test.cpp")

foreach(src IN LISTS TEST_SOURCES)
    add_executable(${src} "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    target_link_libraries(${src} PRIVATE core)
    add_test(NAME ${src} COMMAND ${src})
endforeach()