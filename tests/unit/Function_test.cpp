#include <gtest/gtest.h>
#include <core/Function.hpp>
#include <core/Log.hpp>
#include <cstdint>

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