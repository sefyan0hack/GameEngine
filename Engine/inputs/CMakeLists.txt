
add_library(inputs INTERFACE)

target_include_directories(inputs INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}") # platform folder
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}" PLT_SOURCE) # files in the platform folder

target_sources( inputs
    PRIVATE
        ${PLT_SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/Keyboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Mouse.cpp
    PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
      Keyboard.hpp
      Mouse.hpp
)