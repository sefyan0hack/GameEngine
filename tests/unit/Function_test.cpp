#include <gtest/gtest.h>
#include <core/Function.hpp>
#include <core/Log.hpp>
#include <Platform.hpp>

using namespace std;


TEST(FunctionTest, EmptytFunction) {

    auto add = Function<int32_t(*)(int32_t, int32_t)>();

    EXPECT_EQ(
        add.function(), decltype(add)::default_
    );

    EXPECT_EQ(
        add.name(), ""
    );
}

TEST(FunctionTest, TestFunctionParams) {

    auto add = Function<int64_t(*)(int32_t, int32_t)>();

    EXPECT_EQ(
        add.args_types()[0], ::type_name<int32_t>()
    );

    EXPECT_EQ(
        add.args_types()[1], ::type_name<int32_t>()
    );

    EXPECT_EQ(
        add.return_type(), ::type_name<int64_t>()
    );
}

TEST(FunctionTest, ConstructFunction) {

    auto abs_f = +[](int32_t x) -> uint32_t { return x > 0 ? x : -x; };

    auto abs = Function<decltype(abs_f)>(
        abs_f,
        "abs"
    );

    EXPECT_EQ(
        abs.name(), "abs"
    );

    EXPECT_EQ(
        abs.function(), abs_f
    );

    EXPECT_EQ(
        abs.args_types()[0], ::type_name<int32_t>()
    );

    EXPECT_EQ(
        abs.return_type(), ::type_name<uint32_t>()
    );
}

TEST(FunctionTest, FunctionCallsInc) {

    auto abs_f = +[](int32_t x) -> uint32_t { return x > 0 ? x : -x; };

    auto abs = Function<decltype(abs_f)>(
        abs_f,
        "abs"
    );

    EXPECT_EQ(
        abs.calls(), 0
    );

    abs(-10);

    EXPECT_EQ(
        abs.calls(), 1
    );
}
