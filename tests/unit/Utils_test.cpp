#include <gtest/gtest.h>
#include <core/Utils.hpp>

using namespace std;

// Tests for utils::split
TEST(StringUtils, SplitBasic) {
    auto r = utils::split("hi my name sofyane", " ");
    decltype(r) expect = {"hi", "my", "name", "sofyane"};
    EXPECT_EQ(r, expect);
}

TEST(StringUtils, SplitEmptyString) {
    auto r = utils::split("", " ");
    EXPECT_TRUE(r.empty());
}

TEST(StringUtils, SplitStringWith1Delim) {
    auto r = utils::split(" ", " ");
    decltype(r) expect = {"", ""};
    EXPECT_EQ(r, expect);
}

TEST(StringUtils, SplitStringWith2Delim) {
    auto r = utils::split("  ", " ");
    decltype(r) expect = {"", "", ""};
    EXPECT_EQ(r, expect);
}

TEST(StringUtils, SplitNullString) {
    auto r = utils::split((const char*)nullptr, " ");
    EXPECT_TRUE(r.empty());
}

TEST(StringUtils, SplitStringBeginsDelim) {
    auto r = utils::split(".hi", ".");
    decltype(r) expect = {"", "hi"};
    EXPECT_EQ(r, expect);
}

TEST(StringUtils, SplitStringEndsDelim) {
    auto r = utils::split("hi.", ".");
    decltype(r) expect = {"hi", ""};
    EXPECT_EQ(r, expect);
}

TEST(StringUtils, SplitStringBeginsAndEndsDelim) {
    auto r = utils::split(".hi.", ".");
    decltype(r) expect = { "", "hi", ""};
    EXPECT_EQ(r, expect);
}

TEST(StringUtils, SplitStringWith2DelimAtached) {
    auto r = utils::split("im.the..best.dev.", ".");
    decltype(r) expect = {"im", "the", "", "best", "dev", ""};
    EXPECT_EQ(r, expect);
}

// Tests for utils::replace
TEST(StringUtils, ReplaceBasic) {
    auto r = utils::replace("fuzz", 'f', 'b');
    EXPECT_EQ(r, "buzz");
}

TEST(StringUtils, ReplaceExistingCharWithNullChar) {
    auto r = utils::replace("fuzz", 'f', '\0');
    EXPECT_EQ(r, "fuzz");
}

TEST(StringUtils, ReplaceNotExistingChar) {
    auto r = utils::replace("fuzz", 'a', 'b');
    EXPECT_EQ(r, "fuzz");
}

TEST(StringUtils, ReplaceNullChar) {
    auto r = utils::replace("fuzz", '\0', 'b');
    EXPECT_EQ(r, "fuzz");
}

TEST(StringUtils, ReplaceEmptyStingNullCharWithChar) {
    auto r = utils::replace("", '\0', 'a');
    EXPECT_EQ(r, "");
}

TEST(StringUtils, ReplaceEmptySting) {
    auto r = utils::replace("", 'b', 'a');
    EXPECT_EQ(r, "");
}

TEST(StringUtils, ReplaceWithSameChar) {
    auto r = utils::replace("hi", 'i', 'i');
    EXPECT_EQ(r, "hi");
}
