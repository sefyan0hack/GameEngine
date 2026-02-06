
add_library(ui INTERFACE)

target_include_directories(ui INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}") # platform folder
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}" PLT_SOURCE) # files in the platform folder

target_sources( ui
    PRIVATE
        ${PLT_SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/Text.cpp
    PUBLIC
    FILE_SET headers
      TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
      FILES
        Text.hpp
)

target_link_libraries(ui INTERFACE res)
target_link_libraries(ui INTERFACE glm::glm)
target_link_libraries(ui INTERFACE freetype)
