#include <gtest/gtest.h>
#include <core/Function.hpp>


using namespace std;

using SubType = std::size_t (EG_APIENTRY * )(int32_t, int32_t);


// TEST_F(Function_Test_Ctor1, ctor) {
//     int32_t arg1 = 1, arg2 = 2;

//     EXPECT_NE(m_Func, nullptr);
//     EXPECT_EQ(m_Befor, nullptr);
//     EXPECT_EQ(m_After, nullptr);
//     EXPECT_EQ(m_Name, "add");

//     EXPECT_EQ(m_Func(arg1, arg2), arg1+arg2);

//     EXPECT_EQ(member(arg1, arg2), arg1+arg2);

//     auto expectargvalues = std::tuple<int32_t, int32_t>(arg1, arg2);
//     EXPECT_EQ(m_ArgsValues,  expectargvalues);

//     EXPECT_EQ(m_ReturnType, ::type_name<std::size_t>());

//     auto expargstypes = std::array{::type_name<int32_t>(), ::type_name<int32_t>()};
//     EXPECT_EQ(m_ArgsTypes, expargstypes);
// }

// TEST_F(Function_Test_Ctor1, default_) {
//     auto got = ::type_name<decltype(default_(1,2))>();
//     auto expct = ::type_name<decltype(m_Func(1,2))>();
//     EXPECT_EQ(got, expct);
// }

// TEST_F(Function_Test_Ctor1, ArgsXXX) {
//     auto got = args_values().size();
//     auto expct = 2;
//     EXPECT_EQ(got, expct);
//     EXPECT_EQ(got, args_types().size());
//     EXPECT_EQ(args_count(), expct);
    
// }

// TEST_F(Function_Test_Ctor1, ReturnType) {
//     auto got = return_type();
//     auto expct = ::type_name<std::size_t>();
//     EXPECT_EQ(got, expct);
// }

// TEST_F(Function_Test_Ctor1, this_func_sig) {
//     auto got = this_func_sig();
//     auto expct = std::format(
//         "{} {}({} arg_1 = 0, {} arg_2 = 0)",
//         ::type_name<std::size_t>(),
//         m_Name,
//         ::type_name<int32_t>(),
//         ::type_name<int32_t>()
//     );
//     EXPECT_EQ(got, expct);
// }
