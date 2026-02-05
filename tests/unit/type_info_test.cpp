#include <gtest/gtest.h>

using namespace std;

TEST(TypeQuery, GetTypeName) {
    auto name = ::type_name<int>();
    EXPECT_EQ(name, "int");
}

TEST(TypeQuery, GetTypeKind) {

    {
        auto kind = ::type_kind<int>();
        EXPECT_EQ(kind, "primitive");
    }

    {
        struct X {};
        auto kind = ::type_kind<X>();
        EXPECT_EQ(kind, "class/struct");
    }

    {
        union X {};
        auto kind = ::type_kind<X>();
        EXPECT_EQ(kind, "union");
    }

    {
        enum X { a };
        auto kind = ::type_kind<X>();
        EXPECT_EQ(kind, "enum");
    }

    {
        auto kind = ::type_kind<int*>();
        EXPECT_EQ(kind, "unknown");
    }

}

TEST(TypeQuery, GetTypeHash) {

    auto hash_int = ::type_hash<int>();
    auto hash_float = ::type_hash<float>();

    EXPECT_NE(hash_int, hash_float);
}