#include <gtest/gtest.h>
#include <core/Camera.hpp>

using namespace std;
using namespace glm;

using Camera_Test_Ctor1 = Camera::Test<1>;
using Camera_Test_Ctor2 = Camera::Test<2>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_Position);\
    MEMBER_VAR(m_Forward);\
    MEMBER_VAR(m_Up);\
    MEMBER_VAR(m_Right);\
    MEMBER_VAR(m_Yaw);\
    MEMBER_VAR(m_Pitch);\
    MEMBER_VAR(m_FOV);\
    MEMBER_VAR(m_Near);\
    MEMBER_VAR(m_Far);\
    MEMBER_VAR(m_AspectRatio);\
    \
    MEMBER_FUN(SetFOV);\
    MEMBER_FUN(SetAspectRatio);\
    MEMBER_FUN(SetClipping);\
    MEMBER_FUN(Move);\
    MEMBER_FUN(UpdateVectors);\
    MEMBER_FUN(ProcessMouseMovement);\
    MEMBER_FUN(View);\
    MEMBER_FUN(Perspective);\
    MEMBER_FUN(Position);\
    MEMBER_FUN(Forward);\
    MEMBER_FUN(Up);\
    MEMBER_FUN(Right);\

template<>
struct Camera::Test<1> : public ::testing::Test {
    Camera member;
    TEST_INTERFACE
};

TEST_F(Camera_Test_Ctor1, ctor) {
    EXPECT_EQ(m_Position, vec3(0, 1, 4));
    EXPECT_EQ(m_Forward, vec3(0, 0, -1));
    EXPECT_EQ(m_Up, Camera::WORLD_UP);
    EXPECT_EQ(m_Right, vec3(1, 0, 0));
    EXPECT_EQ(m_Yaw, -90);
    EXPECT_EQ(m_Pitch, 0);
    EXPECT_EQ(m_FOV, 45.0f);
    EXPECT_EQ(m_Near, 0.1f);
    EXPECT_EQ(m_Far, 1000.0f);
    EXPECT_EQ(m_AspectRatio, 16.0f/9.0f);
}

TEST_F(Camera_Test_Ctor1, View) {
    EXPECT_EQ(lookAt(m_Position, m_Position + m_Forward, m_Up), View());
}

TEST_F(Camera_Test_Ctor1, Position) {
    EXPECT_EQ(m_Position, Position());
}

TEST_F(Camera_Test_Ctor1, Forward) {
    EXPECT_EQ(m_Forward, Forward());
}

TEST_F(Camera_Test_Ctor1, Up) {
    EXPECT_EQ(m_Up, Up());
}

TEST_F(Camera_Test_Ctor1, Right) {
    EXPECT_EQ(m_Right, Right());
}
