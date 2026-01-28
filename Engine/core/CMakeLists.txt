
add_library(core INTERFACE)

target_include_directories(core INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}") # platform folder
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/${PLT_DIR_PATH_SUFFIX}" PLT_SOURCE) # files in the platform folder

target_sources( core
  PRIVATE
    ${PLT_SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/APP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DynLib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stacktrace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/res.cpp
  PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/core.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Exception.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/stacktrace.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Log.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Utils.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fmts.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Function.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/APP.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DynLib.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/res.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/format.hpp
)
