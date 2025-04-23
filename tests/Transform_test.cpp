#include <gtest/gtest.h>
#include <core/Transform.hpp>

using namespace std;
using namespace glm;

TEST(Transform, ctor) {
    auto r = Transform(vec3());
    EXPECT_EQ(r.position, vec3());
    EXPECT_EQ(r.rotation, vec3());
    EXPECT_EQ(r.scale, vec3(1.0f, 1.0f, 1.0f));
}