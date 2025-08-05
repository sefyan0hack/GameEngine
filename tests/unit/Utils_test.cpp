#include <gtest/gtest.h>
#include <core/Utils.hpp>
#include <fstream>

using namespace std;
using namespace utils;
using namespace filesystem;

#if !defined(WEB_PLT)
TEST(load_file_async, exist) {
    auto tmpDir  = temp_directory_path();
    auto tmpFile = tmpDir / tmpnam(nullptr);
    auto _ = ofstream { tmpFile };

    EXPECT_NO_THROW({
        load_file_async(tmpFile.string()).get();
    });

}

TEST(load_file_async, not_exist) {
    EXPECT_ANY_THROW(
        load_file_async("filenotexist.ext").get();
    );
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
