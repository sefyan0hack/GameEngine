#include <gtest/gtest.h>
#include <ctime>
#include <graphics/Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;

inline auto rnd = testing::internal::Random(static_cast<uint32_t>(std::time(nullptr)));

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

    auto expeced_fov = (float)rnd.Generate(100);
    cam.set_fov(expeced_fov);

    auto new_fov = cam.fov();

    EXPECT_EQ(new_fov, expeced_fov);
}

TEST(clipping, SettingClippingValue) {
    auto cam = Camera();

    auto expeced_clipping = std::make_pair<float, float>((float)rnd.Generate(2), (float)rnd.Generate(1000));
    cam.set_clipping(expeced_clipping.first, expeced_clipping.second);

    auto new_clipping = cam.clipping();

    EXPECT_EQ(new_clipping, expeced_clipping);
}
TEST(aspect_ratio, SettingAspectRatioValue) {
    auto cam = Camera();

    auto expeced_aspect_ratio = rnd.Generate(100);
    cam.set_aspect_ratio((float)expeced_aspect_ratio);

    auto new_aspect_ratio = cam.aspect_ratio();

    EXPECT_EQ(new_aspect_ratio, expeced_aspect_ratio);
}