include(CMakeDependentOption)
include(FeatureSummary)

option(UNIT_TESTS           "Build Unit Tests"                        OFF)
option(FUZZ_TESTS           "Build Fuzz Tests"                        OFF)
option(HARDEN               "Hardening"                               OFF)
option(OBJ_LIFETIME_TRACE   "Object life time tracing"                OFF)
option(ROBUST_GL_CHECK      "Robust GL checks in Debug builds `slow`" OFF)
cmake_dependent_option(COVERAGE "Code coverage"  ON [[ UNIT_TESTS OR FUZZ_TESTS ]] OFF)
cmake_dependent_option(CONSOLE_ATTACHED "Console Attached to Window"  OFF [[ CMAKE_BUILD_TYPE STREQUAL "Release" ]] ON)

####################################################################################################
# Print Option Status
####################################################################################################

add_feature_info(UNIT_TESTS             UNIT_TESTS           "[Build unit tests]")
add_feature_info(FUZZ_TESTS             FUZZ_TESTS           "[Build Fuzz tests]")
add_feature_info(COVERAGE               COVERAGE             "[Generate coverage reports]")
add_feature_info(HARDEN                 HARDEN               "[Security hardening flags]")
add_feature_info(ROBUST_GL_CHECK        ROBUST_GL_CHECK      "[Robust GL checks in Debug]")
add_feature_info(CONSOLE_ATTACHED       CONSOLE_ATTACHED     "[Console Attached to Window]")
add_feature_info(OBJ_LIFETIME_TRACE     OBJ_LIFETIME_TRACE   "[Object life time tracing]")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
