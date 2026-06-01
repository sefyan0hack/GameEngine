
add_library(ui INTERFACE)

collect_plt_sources(ui)

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

target_link_libraries(ui INTERFACE emath)
target_link_libraries(ui INTERFACE freetype)
if(EMBED_RES)
target_link_libraries(ui INTERFACE res)
endif()