#include <gtest/gtest.h>
#include <ctime>
#include <core/Camera.hpp>

using namespace std;
using namespace glm;

TEST(move, MoveCameraByOffset) {
    auto cam = Camera();
    auto offset = vec3(2,2,2);
    
    auto old_pos = cam.position();
    cam.move(offset);
    auto new_pos = cam.position();
    auto expected_new_pos = old_pos + cam.right() * offset.x + cam.forward() * offset.y + cam.up() * offset.z;

    EXPECT_TRUE(glm::epsilonEqual(new_pos, expected_new_pos, 1e-3f).x);
    EXPECT_TRUE(glm::epsilonEqual(new_pos, expected_new_pos, 1e-3f).y);
    EXPECT_TRUE(glm::epsilonEqual(new_pos, expected_new_pos, 1e-3f).z);
}

TEST(set_fov, SettingFovValue) {
    auto cam = Camera();

    auto rnd = testing::internal::Random(std::time(nullptr));
    auto expeced_fov = rnd.Generate(100);
    cam.set_fov(expeced_fov);

    auto new_fov = cam.fov();

    EXPECT_EQ(new_fov, expeced_fov);
}