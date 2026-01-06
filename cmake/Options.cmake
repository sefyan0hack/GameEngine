include(CMakeDependentOption)

option(TESTS                "Tests"                                   OFF)
option(DOCS                 "Docs"                                    OFF)
option(ENABLE_COVERAGE      "Code coverage"                           OFF)
option(HARDEN               "Hardening"                               OFF)
option(ASAN                 "Address Sanitizer"                       OFF)
option(USAN                 "Undefined Sanitizer"                     OFF)
option(TSAN                 "Thread Sanitizer"                        OFF)
option(LINK_TIME_OPT        "Link Time opt"                           ON)
option(GAME_HOT_RELOADABLE  "Build Game as Shared and hot reload it"  OFF)

cmake_dependent_option(ROBUST_GL_CHECK "Robust GL checks in Debug builds `slow` " ON [[ CMAKE_BUILD_TYPE STREQUAL "Debug" ]] OFF)
cmake_dependent_option(NO_CONSOLE "No Console with Window" ON [[ CMAKE_BUILD_TYPE STREQUAL "Release" ]] OFF)
cmake_dependent_option(RTTI "RunTime Type Info" ON "NOT MINGW" OFF) # disable RTTI becaus of bug in mingw redifinition when static link libstdc++ // TODO: chek if rtti on woks on CI


####################################################################################################
# Print Option Status
####################################################################################################

add_feature_info(TESTS                  TESTS                "[Build the testing suite]")
add_feature_info(DOCS                   DOCS                 "[Build documentation]")
add_feature_info(ENABLE_COVERAGE        ENABLE_COVERAGE      "[Generate coverage reports]")
add_feature_info(HARDEN                 HARDEN               "[Security hardening flags]")
add_feature_info(ASAN                   ASAN                 "[Address Sanitizer]")
add_feature_info(USAN                   USAN                 "[Undefined Sanitizer]")
add_feature_info(LINK_TIME_OPTO         LINK_TIME_OPT        "[Link Time optimization]")
add_feature_info(GAME_HOT_RELOADABLE    GAME_HOT_RELOADABLE  "[Game hot reload support]")
add_feature_info(ROBUST_GL_CHECK        ROBUST_GL_CHECK      "[Robust GL checks in Debug]")
add_feature_info(NO_CONSOLE             NO_CONSOLE           "[No Console with Window]")
add_feature_info(RTTI                   RTTI                 "[Run Time type information]")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
