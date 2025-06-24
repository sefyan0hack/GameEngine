// ===========================================================
//  Function_AddTest.cpp  (C++‑20)
//  Unit tests for Function<std::size_t(int32_t,int32_t)>
// ===========================================================

#include <gtest/gtest.h>
#include <core/Function.hpp>
#include <array>
#include <tuple>
#include <format>

// ────────────────────────────────────────────────────────────
//  Helpers
// ────────────────────────────────────────────────────────────
template <typename T>
constexpr std::string_view type_name()
{
#if defined(__clang__) || defined(__GNUC__)
    return {__PRETTY_FUNCTION__ + 34, sizeof(__PRETTY_FUNCTION__) - 36};
#elif defined(_MSC_VER)
    return {__FUNCSIG__ + 40, sizeof(__FUNCSIG__) - 112};
#else
    return "unknown‑compiler";
#endif
}

// ────────────────────────────────────────────────────────────
//  Fixture
// ────────────────────────────────────────────────────────────
class AddFunctionTest : public ::testing::Test
{
protected:
    using func_sig = std::size_t(int32_t, int32_t);

    // SUT – “system under test”
    Function<func_sig> sut;

    void SetUp() override
    {
        sut.setFunction([](int32_t a, int32_t b) noexcept { return static_cast<std::size_t>(a + b); });
        sut.setName("add");
        // leave before/after hooks null by default
    }

    // convenience aliases
    constexpr static std::array<std::string_view, 2> k_arg_types{
        type_name<int32_t>(), type_name<int32_t>()};
};

// ────────────────────────────────────────────────────────────
//  Compile‑time checks (fail at build, not at runtime)
// ────────────────────────────────────────────────────────────
static_assert(std::is_same_v<AddFunctionTest::func_sig, std::size_t(int32_t, int32_t)>);
static_assert(Function<AddFunctionTest::func_sig>::arity == 2, "Function must take exactly two args");

// ────────────────────────────────────────────────────────────
//  TESTS
// ────────────────────────────────────────────────────────────
TEST_F(AddFunctionTest, DefaultState)
{
    EXPECT_NE(sut.function(), nullptr) << "Function pointer should be initialised";
    EXPECT_EQ(sut.before(), nullptr)   << "No before‑hook expected by default";
    EXPECT_EQ(sut.after(),  nullptr)   << "No after‑hook expected by default";
    EXPECT_EQ(sut.name(),  "add")      << "Name should be ‘add’";
}

TEST_F(AddFunctionTest, InvocationAndCapturedState)
{
    constexpr int32_t a = 1, b = 2;
    const auto expected_sum = static_cast<std::size_t>(a + b);

    EXPECT_EQ(sut(a, b), expected_sum) << "add(1,2) must equal 3";

    const auto captured_args = std::make_tuple(a, b);
    EXPECT_EQ(sut.argsValues(), captured_args) << "Captured argument values mismatch";

    EXPECT_EQ(sut.returnType(), type_name<std::size_t>()) << "Return‑type reflection mismatch";
    EXPECT_EQ(sut.argsTypes(),  k_arg_types)              << "Argument‑type reflection mismatch";
}

TEST_F(AddFunctionTest, ArgsAndCallCountHelpers)
{
    constexpr std::size_t expected_arity = 2;
    EXPECT_EQ(sut.argsCount(), expected_arity) << "Arity helper broken";

    // Call twice more and verify counter
    (void)sut(3, 4);
    (void)sut(5, 6);
    EXPECT_EQ(sut.callsCount(), 3) << "Call counter should track every invocation";
}

TEST_F(AddFunctionTest, SignatureFormatting)
{
    const auto sig = sut.thisFuncSig();
    const auto expect =
        std::format("{} {}({} arg1 = 0, {} arg2 = 0)",
                    type_name<std::size_t>(),
                    sut.name(),
                    type_name<int32_t>(),
                    type_name<int32_t>());

    EXPECT_EQ(sig, expect) << "Human‑readable signature string mismatch";
}
