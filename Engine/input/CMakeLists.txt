# Engine/input/cmakelists.txt

include(helpers)

add_library(input INTERFACE)

target_sources( input PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Keyboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Mouse.cpp
)

target_sources( input PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
      Keyboard.hpp
      Mouse.hpp
)

target_include_directories(input 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Engine
)
