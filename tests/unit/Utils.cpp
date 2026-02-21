#include <gtest/gtest.h>
#include <core/Utils.hpp>

using namespace std;

TEST(Utils, NullPtrTostring)
{
    int* p = nullptr;
    auto r = utils::to_string(p);

    EXPECT_EQ(r, "null");
}

TEST(Utils, PrimitivePtrTostring)
{
    int p = 42;
    auto r = utils::to_string(&p);

    EXPECT_EQ(r, "42");
}

TEST(Utils, PrimitivePtrPtrTostring)
{
    int x = 42;
    int* p = &x;
    auto r = utils::to_string(&p);

    EXPECT_EQ(r, "42");
}

TEST(Utils, NonFormatableTypeTostring)
{
    struct X { int x; };
    auto p = X{ 42 };

    auto r = utils::to_string(&p);

    EXPECT_EQ(r, "");
}

// TEST(Utils, StringPtrTostring)
// {
//     auto p = "hello";

//     auto r = utils::to_string(&p);

//     EXPECT_STREQ(r.c_str(), p);
// }


TEST(Utils, MatchShouldGoToFloatLambda)
{
    std::variant<char, int, float> t = float{1.5};
    auto r = utils::match( t,
        [](char){ return "char"; },
        [](int){ return "int"; },
        [](float){ return "float";}
    );

    EXPECT_STREQ(r, "float");
}

TEST(Utils, MatchShouldFallToGenericLambda)
{
    std::variant<char, int, float> t = float{1.5};
    auto r = utils::match( t,
        [](char){ return "char"; },
        [](int){ return "int"; },
        [](auto){ return "auto";}
    );

    EXPECT_STREQ(r, "auto");
}
