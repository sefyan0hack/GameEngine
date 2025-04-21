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

TEST(Transform, formatter) {
    auto r = format("{}", Transform(vec3()));
    auto e = format(
        R"({{ "position": {}, "rotation": {}, "scale": {} }})", 
        vec3(), vec3(), vec3(1.0f, 1.0f, 1.0f)
    );
    EXPECT_EQ(r, e);
}