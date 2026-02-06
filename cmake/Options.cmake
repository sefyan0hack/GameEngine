include(CMakeDependentOption)

option(WITH_TESTS           "Tests"                                   OFF)
option(WITH_DOCS            "Docs"                                    OFF)
option(COVERAGE             "Code coverage"                           OFF)
option(HARDEN               "Hardening"                               ON)
option(LINK_TIME_OPT        "Link Time opt"                           ON)
option(GAME_HOT_RELOADABLE  "Build Game as Shared and hot reload it"  OFF)
option(OBJ_LIFETIME_TRACE   "Object life time tracing"                ON)
option(ROBUST_GL_CHECK      "Robust GL checks in Debug builds `slow`" OFF)
cmake_dependent_option(CONSOLE_ATTACHED "Console Attached to Window"  OFF [[ CMAKE_BUILD_TYPE STREQUAL "Release" ]] ON)

####################################################################################################
# Print Option Status
####################################################################################################

add_feature_info(WITH_TESTS             WITH_TESTS           "[Build the testing suite]")
add_feature_info(WITH_DOCS              WITH_DOCS            "[Build Documentation]")
add_feature_info(COVERAGE               COVERAGE             "[Generate coverage reports]")
add_feature_info(HARDEN                 HARDEN               "[Security hardening flags]")
add_feature_info(LINK_TIME_OPT          LINK_TIME_OPT        "[Link Time optimization]")
add_feature_info(GAME_HOT_RELOADABLE    GAME_HOT_RELOADABLE  "[Game hot reload support]")
add_feature_info(ROBUST_GL_CHECK        ROBUST_GL_CHECK      "[Robust GL checks in Debug]")
add_feature_info(CONSOLE_ATTACHED       CONSOLE_ATTACHED     "[Console Attached  to Window]")
add_feature_info(OBJ_LIFETIME_TRACE     OBJ_LIFETIME_TRACE   "[Object life time tracing]")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
