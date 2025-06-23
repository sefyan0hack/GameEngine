#include <gtest/gtest.h>
#include <core/Camera.hpp>
#include <core/Window.hpp>

using namespace std;
using namespace glm;

using Camera_Test_Ctor1 = Camera::Test<1>;
using Camera_Test_Ctor2 = Camera::Test<2>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_Position);\
    MEMBER_VAR(m_FrontDir);\
    MEMBER_VAR(m_UpDir);\
    MEMBER_VAR(m_RightDir);\
    MEMBER_VAR(m_Window);\
    MEMBER_VAR(m_Sensitivity);\
    MEMBER_VAR(m_Yaw);\
    MEMBER_VAR(m_Pitch);\
    \
    MEMBER_FUN(SetFrontVector);\
    MEMBER_FUN(SetUpVector);\
    MEMBER_FUN(SetRightVector);\
    MEMBER_FUN(MoveFroward);\
    MEMBER_FUN(MoveBackward);\
    MEMBER_FUN(MoveUP);\
    MEMBER_FUN(MoveDown);\
    MEMBER_FUN(MoveRight);\
    MEMBER_FUN(MoveLeft)\
    MEMBER_FUN(UpdateVectors);\
    MEMBER_FUN(MoseMove);\
    MEMBER_FUN(View);\
    MEMBER_FUN(Perspective);\
    MEMBER_FUN(Position);\
    MEMBER_FUN(FrontDir);\
    MEMBER_FUN(UpDir);\
    MEMBER_FUN(RightDir);\
    MEMBER_FUN(Sensitivity);

template<>
struct Camera::Test<1> : public ::testing::Test {
    Camera member;
    TEST_INTERFACE
};

TEST_F(Camera_Test_Ctor1, ctor) {
    EXPECT_EQ(m_Position, vec3(0, 1, 4));
    EXPECT_EQ(m_FrontDir, vec3(0, 0, -1));
    EXPECT_EQ(m_UpDir, vec3(0, 1, 0));
    EXPECT_EQ(m_RightDir, vec3(1, 0, 0));
    EXPECT_EQ(m_Window, nullptr);
    EXPECT_EQ(m_Sensitivity, 0.11f);
    EXPECT_EQ(m_Yaw, -90);
    EXPECT_EQ(m_Pitch, 0);
}

TEST_F(Camera_Test_Ctor1, SetFrontVector) {
    auto value = vec3(0,0,1);
    SetFrontVector(value);
    EXPECT_EQ(m_FrontDir, value);
}

TEST_F(Camera_Test_Ctor1, SetUpVector) {
    auto value = vec3(1,0,0);
    SetUpVector(value);
    EXPECT_EQ(m_UpDir, value);
}

TEST_F(Camera_Test_Ctor1, SetRightVector) {
    auto value = vec3(1,0,0);
    SetRightVector(value);
    EXPECT_EQ(m_RightDir, value);
}

TEST_F(Camera_Test_Ctor1, MoveFroward) {
    auto speed = 5.0f;
    auto e = m_Position - (-m_FrontDir * speed);
    MoveFroward(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test_Ctor1, MoveBackward) {
    auto speed = 5.0f;
    auto e = m_Position + (-m_FrontDir * speed);
    MoveBackward(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test_Ctor1, MoveUP) {
    auto speed = 5.0f; 
    auto e = m_Position + (m_UpDir  * speed);
    MoveUP(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test_Ctor1, MoveDown) {
    auto speed = 5.0f;
    auto e = m_Position - (m_UpDir  * speed);
    MoveDown(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test_Ctor1, MoveRight) {
    auto speed = 5.0f;
    auto e = m_Position + (glm::cross(m_FrontDir, m_UpDir)  * speed);
    MoveRight(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test_Ctor1, MoveLeft) {
    auto speed = 5.0f;
    auto e = m_Position - (glm::cross(m_FrontDir, m_UpDir)  * speed);
    MoveLeft(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test_Ctor1, View) {
    EXPECT_EQ(lookAt(m_Position, m_Position + m_FrontDir, m_UpDir), View());
}

TEST_F(Camera_Test_Ctor1, Position) {
    EXPECT_EQ(m_Position, Position());
}

TEST_F(Camera_Test_Ctor1, FrontDir) {
    EXPECT_EQ(m_FrontDir, FrontDir());
}

TEST_F(Camera_Test_Ctor1, UpDir) {
    EXPECT_EQ(m_UpDir, UpDir());
}

TEST_F(Camera_Test_Ctor1, RightDir) {
    EXPECT_EQ(m_RightDir, RightDir());
}

TEST_F(Camera_Test_Ctor1, Sensitivity) {
    EXPECT_EQ(m_Sensitivity, Sensitivity());
}