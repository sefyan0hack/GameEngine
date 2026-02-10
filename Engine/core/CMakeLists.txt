
add_library(core INTERFACE)

collect_plt_sources(core)

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
  FILE_SET headers
    TYPE HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/core.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Exception.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/stacktrace.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Log.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Utils.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/Function.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/APP.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/DynLib.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/SysInfo.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/res.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/format.hpp
)
