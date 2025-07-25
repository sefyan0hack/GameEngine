#include <gtest/gtest.h>
#include <core/Scene.hpp>

using namespace std;

using Scene_Test_Ctor1 = Scene::Test<1>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_SkyBox);\
    MEMBER_VAR(m_Entities);\
    \
    MEMBER_FUN(operator <<);\
    MEMBER_FUN(Entities);\
    MEMBER_FUN(setSkyBox);\
    MEMBER_FUN(skyBox);\
    MEMBER_FUN(to_json);

template<>
struct Scene::Test<1> : public ::testing::Test {
    Scene member;
    TEST_INTERFACE
};

TEST_F(Scene_Test_Ctor1, ctor) {
    EXPECT_EQ(m_Entities.size(), 0);
}