#include <gtest/gtest.h>
#include <core/Function.hpp>

using namespace std;

using SubType = size_t (APIENTRY * )(int, int);

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
    MEMBER_FUN(ArgsValues);\
    MEMBER_FUN(ReturnType);\
    MEMBER_FUN(ArgsTypes);\
    MEMBER_FUN(ArgsCount);\
    MEMBER_FUN(CallsCount);\
    MEMBER_FUN(functionCount);\
    MEMBER_FUN(formated_function_sig);\
    MEMBER_FUN(function_info);\
    MEMBER_FUN(format_arguments);\
    MEMBER_FUN(to_string);\
    MEMBER_FUN(format_pointer);\
    MEMBER_FUN(functionPtrSigature);


template<> template<>
struct Function<SubType>::Test<1> : public ::testing::Test {
    Function<SubType> member;

    Test(){
        member.m_Func = []( int a, int b ) -> size_t { return a+b; };
        member.m_Befor = nullptr;
        member.m_After = nullptr;
        member.m_Name = "add";
    }

    TEST_INTERFACE
};


TEST_F(Function_Test_Ctor1, ctor) {
    EXPECT_NE(m_Func, nullptr);
    EXPECT_EQ(m_Befor, nullptr);
    EXPECT_EQ(m_After, nullptr);
    EXPECT_EQ(m_Name, "add");
    EXPECT_EQ(m_Func(1,2), 1+2);
    EXPECT_EQ(m_ReturnType, ::type_name<size_t>());

    auto expargstypes = std::array<std::string, 2>{::type_name<int>(), ::type_name<int>()};
    EXPECT_EQ(m_ArgsTypes, expargstypes);
    EXPECT_EQ(m_CallCount,  1 );
}