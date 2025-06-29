#include <gtest/gtest.h>
#include <core/Utils.hpp>
#include <fstream>

using namespace std;
using namespace utils;

#if !defined(WEB_PLT)
TEST(load_file_async, exist) {
    auto filename = "Hello.txt";
    ofstream file { filename };
    if(!file.is_open()){
        SUCCEED();
    }
    
    auto r = load_file_async(filename).get();
    EXPECT_EQ(r.has_value(), true);
}

TEST(load_file_async, not_exist) {
    auto r = load_file_async("filenotexist.ext").get();
    EXPECT_EQ(r.has_value(), false);
}
#endif

TEST(split, string) {
    auto r = split("hi my name sofyane", " ");
    decltype(r) expect = {"hi", "my", "name", "sofyane"};
    EXPECT_EQ(r, expect);
}

TEST(split, empty_string) {
    auto r = split("", " ");
    decltype(r) expect;
    EXPECT_EQ(r, expect);
}

TEST(split, empty_null) {
    auto r = split((const char*)nullptr, " ");
    decltype(r) expect;
    EXPECT_EQ(r, expect);
}

TEST(replace, with_char) {
    auto r = replace("fuzz", 'f', 'b');
    EXPECT_EQ(r, "buzz");
}

TEST(replace, with_null_char) {
    auto r = replace("fuzz", 'f', '\0');
    EXPECT_EQ(r, "fuzz");
}

TEST(replace, with_not_exist_char) {
    auto r = replace("fuzz", 'a', 'b');
    EXPECT_EQ(r, "fuzz");
}

TEST(replace, c_null) {
    auto r = replace("fuzz", '\0', 'b');
    EXPECT_EQ(r, "fuzz");
}
