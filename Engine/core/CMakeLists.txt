# Engine/core/cmakelists.txt

include(helpers)

add_library(core INTERFACE)

target_include_directories(core INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}") # platform folder
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}" PLT_SOURCE) # files in the platform folder

target_sources( core PRIVATE

  ${PLT_SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/APP.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DynLib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/stacktrace.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/res.cpp
)

target_sources( core PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        core.hpp
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
