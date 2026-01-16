include(CMakeDependentOption)

option(TESTS                "Tests"                                   OFF)
option(DOCS                 "Docs"                                    OFF)
option(COVERAGE             "Code coverage"                           OFF)
option(HARDEN               "Hardening"                               ON)
option(ASAN                 "Address Sanitizer"                       OFF)
option(USAN                 "Undefined Sanitizer"                     OFF)
option(LINK_TIME_OPT        "Link Time opt"                           ON)
option(GAME_HOT_RELOADABLE  "Build Game as Shared and hot reload it"  OFF)
option(RTTI                 "RunTime Type Info"                       ON)
option(OBJ_LIFETIME_TRACE   "Object life time tracing"                ON)
cmake_dependent_option(STATIC_LINK "Static link C++" ON "MINGW" OFF)
cmake_dependent_option(ROBUST_GL_CHECK "Robust GL checks in Debug builds `slow` " ON [[ CMAKE_BUILD_TYPE STREQUAL "Debug" ]] OFF)
cmake_dependent_option(CONSOLE_ATTACHED "Console Attached to Window" OFF [[ CMAKE_BUILD_TYPE STREQUAL "Release" ]] ON)

####################################################################################################
# Print Option Status
####################################################################################################

add_feature_info(TESTS                  TESTS                "[Build the testing suite]")
add_feature_info(DOCS                   DOCS                 "[Build documentation]")
add_feature_info(COVERAGE               COVERAGE             "[Generate coverage reports]")
add_feature_info(HARDEN                 HARDEN               "[Security hardening flags]")
add_feature_info(ASAN                   ASAN                 "[Address Sanitizer]")
add_feature_info(USAN                   USAN                 "[Undefined Sanitizer]")
add_feature_info(LINK_TIME_OPT          LINK_TIME_OPT        "[Link Time optimization]")
add_feature_info(GAME_HOT_RELOADABLE    GAME_HOT_RELOADABLE  "[Game hot reload support]")
add_feature_info(ROBUST_GL_CHECK        ROBUST_GL_CHECK      "[Robust GL checks in Debug]")
add_feature_info(CONSOLE_ATTACHED       CONSOLE_ATTACHED     "[Console Attached  to Window]")
add_feature_info(RTTI                   RTTI                 "[Run Time type information]")
add_feature_info(OBJ_LIFETIME_TRACE     OBJ_LIFETIME_TRACE   "[Object life time tracing]")
add_feature_info(STATIC_LINK            STATIC_LINK          "[Static link C++]")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
