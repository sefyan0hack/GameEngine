# Engine/core/cmakelists.txt

include(helpers)

add_library(core INTERFACE)

target_sources( core PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/APP.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DynLib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/res.cpp
)

target_sources( core PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        core.hpp
        Platform.hpp
        Exception.hpp
        Log.hpp
        Utils.hpp
        fmts.hpp
        Event.hpp
        Function.hpp
        APP.hpp
        ResMan.hpp
        DynLib.hpp
        SysInfo.hpp
        res.hpp
)

target_include_directories(core 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Engine
)
