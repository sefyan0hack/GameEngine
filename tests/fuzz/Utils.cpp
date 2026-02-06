#include <fuzztest/fuzztest.h>
#include <core/Utils.hpp>
#include <string>
#include <cmath>
#include <print>

void SplitDoesNotCrash(std::string input, char delim) {
    // Optional: restrict delimiter to printable chars
    if (delim < 32 || delim > 126) return;

    std::string d(1, delim);

    utils::split(input.c_str(), d);
}

FUZZ_TEST(UtilsSplitFuzz, SplitDoesNotCrash);