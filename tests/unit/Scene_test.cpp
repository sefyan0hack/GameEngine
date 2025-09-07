#include <gtest/gtest.h>
#include <core/Scene.hpp>

using namespace std;

using Scene_Test_Ctor1 = Scene::Test<1>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_SkyBox);\
    MEMBER_VAR(m_Entities);\
    \
    MEMBER_FUN(add);\
    MEMBER_FUN(entities);\
    MEMBER_FUN(set_skybox);\
    MEMBER_FUN(clear);

template<>
struct Scene::Test<1> : public ::testing::Test {
    Scene member;
    TEST_INTERFACE
};

// TEST_F(Scene_Test_Ctor1, ctor) {
    // EXPECT_EQ(m_Entities.size(), 0); disable for C++ exception with description "GetShaderiv Failed" thrown in the test fixture's constructor.
// }