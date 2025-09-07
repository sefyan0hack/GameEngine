#include <gtest/gtest.h>
#include <core/Function.hpp>


using namespace std;

using SubType = std::size_t (PLT_CALL * )(int32_t, int32_t);

using Function_Test_Ctor1 = Function<SubType>::Test<1>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_Func);\
    MEMBER_VAR(m_Befor);\
    MEMBER_VAR(m_After);\
    MEMBER_VAR(m_Name);\
    MEMBER_VAR(m_ReturnType);\
    MEMBER_VAR(m_ArgsTypes);\
    MEMBER_VAR(m_ArgsValues);\
    MEMBER_VAR(m_CallCount);\
    MEMBER_VAR(m_Count);\
    \
    MEMBER_FUN(operator());\
    MEMBER_FUN(default_);\
    MEMBER_FUN(args_values);\
    MEMBER_FUN(return_type);\
    MEMBER_FUN(args_types);\
    MEMBER_FUN(args_count);\
    MEMBER_FUN(calls_count);\
    MEMBER_FUN(function_count);\
    MEMBER_FUN(this_func_sig);\
    MEMBER_FUN(function_info);\
    MEMBER_FUN(format_arguments);\
    MEMBER_FUN(to_string);

template<> template<>
struct Function<SubType>::Test<1> : public ::testing::Test {
    Function<SubType> member;

    Test(){
        member.m_Func = []( int32_t a, int32_t b ) -> std::size_t { return a+b; };
        member.m_Befor = nullptr;
        member.m_After = nullptr;
        member.m_Name = "add";
    }

    TEST_INTERFACE
};


TEST_F(Function_Test_Ctor1, ctor) {
    int32_t arg1 = 1, arg2 = 2;

    EXPECT_NE(m_Func, nullptr);
    EXPECT_EQ(m_Befor, nullptr);
    EXPECT_EQ(m_After, nullptr);
    EXPECT_EQ(m_Name, "add");

    EXPECT_EQ(m_Func(arg1, arg2), arg1+arg2);

    EXPECT_EQ(member(arg1, arg2), arg1+arg2);

    auto expectargvalues = std::tuple<int32_t, int32_t>(arg1, arg2);
    EXPECT_EQ(m_ArgsValues,  expectargvalues);

    EXPECT_EQ(m_ReturnType, ::type_name<std::size_t>());

    auto expargstypes = std::array{::type_name<int32_t>(), ::type_name<int32_t>()};
    EXPECT_EQ(m_ArgsTypes, expargstypes);
}

TEST_F(Function_Test_Ctor1, default_) {
    auto got = ::type_name<decltype(default_(1,2))>();
    auto expct = ::type_name<decltype(m_Func(1,2))>();
    EXPECT_EQ(got, expct);
}

TEST_F(Function_Test_Ctor1, ArgsXXX) {
    auto got = args_values().size();
    auto expct = 2;
    EXPECT_EQ(got, expct);
    EXPECT_EQ(got, args_types().size());
    EXPECT_EQ(args_count(), expct);
    
}

TEST_F(Function_Test_Ctor1, ReturnType) {
    auto got = return_type();
    auto expct = ::type_name<std::size_t>();
    EXPECT_EQ(got, expct);
}

TEST_F(Function_Test_Ctor1, this_func_sig) {
    auto got = this_func_sig();
    auto expct = std::format(
        "{} {}({} arg_1 = 0, {} arg_2 = 0)",
        ::type_name<std::size_t>(),
        m_Name,
        ::type_name<int32_t>(),
        ::type_name<int32_t>()
    );
    EXPECT_EQ(got, expct);
}
