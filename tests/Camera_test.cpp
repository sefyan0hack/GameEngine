#include <gtest/gtest.h>
#include <core/Camera.hpp>

using namespace std;
using namespace glm;

struct Camera_Test : public ::testing::Test {
    Camera member;

    MEMBER_VAR(m_Position);
    MEMBER_VAR(m_FrontDir);
    MEMBER_VAR(m_UpDir);
    MEMBER_VAR(m_RightDir);
    MEMBER_VAR(m_Window);
    MEMBER_VAR(m_Sensitivity);
    MEMBER_VAR(m_Yaw);
    MEMBER_VAR(m_Pitch);

    MEMBER_FUN(void, SetFrontVector, (vec3));
    MEMBER_FUN(void, SetUpVector, (vec3));
    MEMBER_FUN(void, SetRightVector, (vec3));
    MEMBER_FUN(void, MoveFroward, (float));
    MEMBER_FUN(void, MoveBackward, (float));
    MEMBER_FUN(void, MoveUP, (float));
    MEMBER_FUN(void, MoveDown, (float));
    MEMBER_FUN(void, MoveRight, (float));
    MEMBER_FUN(void, MoveLeft,  (float));
    MEMBER_FUN(void, EnableMSAA, (void));
    MEMBER_FUN(void, UpdateVectors, (void));
    MEMBER_FUN(void, MoseMove, (bool));
    MEMBER_FUN(mat4, View, (void) const);
    MEMBER_FUN(mat4, Perspective, (void) const);
    MEMBER_FUN(vec3, Position, (void) const);
    MEMBER_FUN(vec3, FrontDir, (void) const);
    MEMBER_FUN(vec3, UpDir, (void) const);
    MEMBER_FUN(vec3, RightDir, (void) const);
    MEMBER_FUN(float, Sensitivity, (void) const);
};

TEST_F(Camera_Test, ctor) {
    EXPECT_EQ(m_Position, vec3(0, 2, 0));
    EXPECT_EQ(m_FrontDir, vec3(0, 0, -1));
    EXPECT_EQ(m_UpDir, vec3(0, 1, 0));
    EXPECT_EQ(m_RightDir, vec3(1, 0, 0));
    EXPECT_EQ(m_Window, nullptr);
    EXPECT_EQ(m_Sensitivity, 0.11f);
    EXPECT_EQ(m_Yaw, -90);
    EXPECT_EQ(m_Pitch, 0);
}

TEST_F(Camera_Test, SetFrontVector) {
    auto value = vec3(0,0,1);
    SetFrontVector(value);
    EXPECT_EQ(m_FrontDir, value);
}

TEST_F(Camera_Test, SetUpVector) {
    auto value = vec3(1,0,0);
    SetUpVector(value);
    EXPECT_EQ(m_UpDir, value);
}

TEST_F(Camera_Test, SetRightVector) {
    auto value = vec3(1,0,0);
    SetRightVector(value);
    EXPECT_EQ(m_RightDir, value);
}

TEST_F(Camera_Test, MoveFroward) {
    auto speed = 5.0f;
    auto e = m_Position - (-m_FrontDir * speed);
    MoveFroward(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test, MoveBackward) {
    auto speed = 5.0f;
    auto e = m_Position + (-m_FrontDir * speed);
    MoveBackward(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test, MoveUP) {
    auto speed = 5.0f; 
    auto e = m_Position + (m_UpDir  * speed);
    MoveUP(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test, MoveDown) {
    auto speed = 5.0f;
    auto e = m_Position - (m_UpDir  * speed);
    MoveDown(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test, MoveRight) {
    auto speed = 5.0f;
    auto e = m_Position + (glm::cross(m_FrontDir, m_UpDir)  * speed);
    MoveRight(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test, MoveLeft) {
    auto speed = 5.0f;
    auto e = m_Position - (glm::cross(m_FrontDir, m_UpDir)  * speed);
    MoveLeft(speed);
    EXPECT_EQ(m_Position, e);
}

TEST_F(Camera_Test, View) {
    EXPECT_EQ(lookAt(m_Position, m_Position + m_FrontDir, m_UpDir), View());
}

TEST_F(Camera_Test, Position) {
    EXPECT_EQ(m_Position, Position());
}

TEST_F(Camera_Test, FrontDir) {
    EXPECT_EQ(m_FrontDir, FrontDir());
}

TEST_F(Camera_Test, UpDir) {
    EXPECT_EQ(m_UpDir, UpDir());
}

TEST_F(Camera_Test, RightDir) {
    EXPECT_EQ(m_RightDir, RightDir());
}

TEST_F(Camera_Test, Sensitivity) {
    EXPECT_EQ(m_Sensitivity, Sensitivity());
}