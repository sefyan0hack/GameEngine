# Engine/core/cmakelists.txt

include(helpers)

add_library(core INTERFACE)

target_sources( core PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/APP.cpp
)

target_sources( core PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        core.hpp
        Platform.hpp
        Log.hpp
        Utils.hpp
        fmts.hpp
        Event.hpp
        EventQueue.hpp
        Function.hpp
        ResourceManager.hpp
        APP.hpp
        ResourceManager.hpp
        DynLib.hpp
)

target_include_directories(core 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Engine
)
