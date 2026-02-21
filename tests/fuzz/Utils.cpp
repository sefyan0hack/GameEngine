#include <fuzztest/fuzztest.h>
#include <core/Utils.hpp>
#include <string>
#include <cmath>
#include <print>

void to_stringDoesNotCrash(auto* ptr) {
    utils::to_string(ptr);
}

FUZZ_TEST(UtilsSplitFuzz, to_stringDoesNotCrash);