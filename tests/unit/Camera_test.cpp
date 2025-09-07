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
    MEMBER_FUN(set_fov);\
    MEMBER_FUN(set_aspect_ratio);\
    MEMBER_FUN(set_clipping);\
    MEMBER_FUN(move);\
    MEMBER_FUN(update_vectors);\
    MEMBER_FUN(process_mouse_movement);\
    MEMBER_FUN(view);\
    MEMBER_FUN(perspective);\
    MEMBER_FUN(position);\
    MEMBER_FUN(forward);\
    MEMBER_FUN(up);\
    MEMBER_FUN(right);\

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
    EXPECT_EQ(lookAt(m_Position, m_Position + m_Forward, m_Up), view());
}

TEST_F(Camera_Test_Ctor1, position) {
    EXPECT_EQ(m_Position, position());
}

TEST_F(Camera_Test_Ctor1, forward) {
    EXPECT_EQ(m_Forward, forward());
}

TEST_F(Camera_Test_Ctor1, up) {
    EXPECT_EQ(m_Up, up());
}

TEST_F(Camera_Test_Ctor1, right) {
    EXPECT_EQ(m_Right, right());
}
