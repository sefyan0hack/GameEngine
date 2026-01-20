#include <gtest/gtest.h>
#include <graphics/Transform.hpp>

using namespace std;
using namespace glm;

TEST(Transform, ctor) {
    auto r = Transform(vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(r.position, vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(r.rotation, vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(r.scale, vec3(10.0f, 1.0f, 1.0f));
}

TEST(Transform, empty_ctor) {
    auto r = Transform();
    EXPECT_EQ(r.position, vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(r.rotation, vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(r.scale, vec3(1.0f, 1.0f, 1.0f));
}