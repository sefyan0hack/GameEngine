#include <gtest/gtest.h>
#include <core/Utils.hpp>
#include <windows.h>

TEST(load_file_async, exist) {
    auto r = load_file_async(__FILE__).get();
    EXPECT_EQ(r.has_value(), true);
}
TEST(load_file_async, notexist) {
    auto r = load_file_async("filenotexist.ext").get();
    EXPECT_EQ(r.has_value(), false);
}