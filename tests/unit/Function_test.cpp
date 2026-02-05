#include <gtest/gtest.h>
#include <core/Function.hpp>
#include <core/Log.hpp>
#include <Platform.hpp>

using namespace std;

TEST(Function, EmptytFunction) {

    auto add = Function<int32_t(*)(int32_t, int32_t)>();

    EXPECT_EQ(
        add.function(), decltype(add)::default_
    ) << "add.functions should be defaulted to empty func";

    EXPECT_EQ(
        add.name(), ""
    ) << "name should be empty";
}

TEST(Function, TestFunctionParams) {

    auto add = Function<int64_t(*)(int32_t, int32_t)>();

    EXPECT_EQ(
        add.args_types()[0], ::type_name<int32_t>()
    ) << "first arg match";

    EXPECT_EQ(
        add.args_types()[1], ::type_name<int32_t>()
    ) << "second arg match";

    EXPECT_EQ(
        add.return_type(), ::type_name<int64_t>()
    ) << "return type match";
}

TEST(Function, ConstructFunction) {

    auto abs_f = +[](int32_t x) -> uint32_t { return x > 0 ? x : -x; };

    auto abs = Function<decltype(abs_f)>(
        abs_f,
        "abs"
    );

    EXPECT_EQ(
        abs.name(), "abs"
    ) << "name should be eq to the one is set";

    EXPECT_EQ(
        abs.function(), abs_f
    ) << "address of func should be eq";

    EXPECT_EQ(
        abs.args_types()[0], ::type_name<int32_t>()
    ) << "second arg match";

    EXPECT_EQ(
        abs.return_type(), ::type_name<uint32_t>()
    ) << "return type match";
}

TEST(Function, FunctionCallsInc) {

    auto abs_f = +[](int32_t x) -> uint32_t { return x > 0 ? x : -x; };

    auto abs = Function<decltype(abs_f)>(
        abs_f,
        "abs"
    );

    EXPECT_EQ(
        abs.calls(), 0
    ) << "the calls should be 0 at start";

    abs(-10);

    EXPECT_EQ(
        abs.calls(), 1
    ) << "calls should be incrimented by 1";
}