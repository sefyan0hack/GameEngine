
add_library(inputs INTERFACE)

collect_plt_sources(inputs)

target_sources( inputs
    PRIVATE
        ${PLT_SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/Keyboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Mouse.cpp
    PUBLIC
    FILE_SET headers
      TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
      FILES
      Keyboard.hpp
      Mouse.hpp
)