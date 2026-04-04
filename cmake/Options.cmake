include(CMakeDependentOption)
include(FeatureSummary)

option(UNIT_TESTS           "Build Unit Tests"                        OFF)
option(FUZZ_TESTS           "Build Fuzz Tests"                        OFF)
option(HARDEN               "Hardening"                               OFF)
cmake_dependent_option(COVERAGE "Code coverage"  ON [[ UNIT_TESTS OR FUZZ_TESTS ]] OFF)

####################################################################################################
# Print Option Status
####################################################################################################

add_feature_info(UNIT_TESTS             UNIT_TESTS           "[Build unit tests]")
add_feature_info(FUZZ_TESTS             FUZZ_TESTS           "[Build Fuzz tests]")
add_feature_info(COVERAGE               COVERAGE             "[Generate coverage reports]")
add_feature_info(HARDEN                 HARDEN               "[Security hardening flags]")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
