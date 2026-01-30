#include <gtest/gtest.h>
#include <core/Utils.hpp>

using namespace std;

// Tests for utils::split
TEST(SplitBySpace, String) {
    auto r = utils::split("hi my name sofyane", " ");
    decltype(r) expect = {"hi", "my", "name", "sofyane"};
    EXPECT_EQ(r, expect);
}

TEST(SplitBySpace, EmptyString) {
    auto r = utils::split("", " ");
    EXPECT_TRUE(r.empty());
}

TEST(SplitBySpace, StringWith1Delim) {
    auto r = utils::split(" ", " ");
    decltype(r) expect = {"", ""};
    EXPECT_EQ(r, expect);
}

TEST(SplitBySpace, StringWith2Delim) {
    auto r = utils::split("  ", " ");
    decltype(r) expect = {"", "", ""};
    EXPECT_EQ(r, expect);
}

TEST(SplitBySpace, NullString) {
    auto r = utils::split((const char*)nullptr, " ");
    EXPECT_TRUE(r.empty());
}

TEST(SplitByDot, StringBeginsDelim) {
    auto r = utils::split(".hi", ".");
    decltype(r) expect = {"", "hi"};
    EXPECT_EQ(r, expect);
}

TEST(SplitByDot, StringEndsDelim) {
    auto r = utils::split("hi.", ".");
    decltype(r) expect = {"hi", ""};
    EXPECT_EQ(r, expect);
}

TEST(SplitByDot, StringBeginsAndEndsDelim) {
    auto r = utils::split(".hi.", ".");
    decltype(r) expect = { "", "hi", ""};
    EXPECT_EQ(r, expect);
}

TEST(SplitByDot, StringWith2DelimAtached) {
    auto r = utils::split("im.the..best.dev.", ".");
    decltype(r) expect = {"im", "the", "", "best", "dev", ""};
    EXPECT_EQ(r, expect);
}

// Tests for utils::replace
TEST(Replace, ExistingChar) {
    auto r = utils::replace("fuzz", 'f', 'b');
    EXPECT_EQ(r, "buzz");
}

TEST(Replace, ExistingCharWithNullChar) {
    auto r = utils::replace("fuzz", 'f', '\0');
    EXPECT_EQ(r, "fuzz");
}

TEST(Replace, NotExistingChar) {
    auto r = utils::replace("fuzz", 'a', 'b');
    EXPECT_EQ(r, "fuzz");
}

TEST(Replace, NullChar) {
    auto r = utils::replace("fuzz", '\0', 'b');
    EXPECT_EQ(r, "fuzz");
}

TEST(Replace, EmptyStingNullCharWithChar) {
    auto r = utils::replace("", '\0', 'a');
    EXPECT_EQ(r, "");
}

TEST(Replace, EmptySting) {
    auto r = utils::replace("", 'b', 'a');
    EXPECT_EQ(r, "");
}

TEST(Replace, WithSameChar) {
    auto r = utils::replace("hi", 'i', 'i');
    EXPECT_EQ(r, "hi");
}
