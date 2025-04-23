#include <gtest/gtest.h>
#include <core/fmts.hpp>
#include <format>
#include <map>
#include <vector>

using namespace std;
using namespace glm;

TEST(Formatters, glm_vec2){
    auto r = std::format("{}", vec2());
    auto e = string(R"({ "x": 0, "y": 0 })");
    EXPECT_EQ(r, e);
}

TEST(Formatters, glm_vec3){
    auto r = std::format("{}", vec3());
    auto e = string(R"({ "x": 0, "y": 0, "z": 0 })");
    EXPECT_EQ(r, e);
}

TEST(Formatters, glm_mat4){
    auto r = std::format("{}", mat4());
    auto e = string("[[ 0, 0, 0, 0 ], [ 0, 0, 0, 0 ], [ 0, 0, 0, 0 ], [ 0, 0, 0, 0 ]]");
    EXPECT_EQ(r, e);
}

TEST(Formatters, MapWrapper){
    auto m = map<int, string>{ {1, "one"}, {2, "two"}, {3, "three"} };
    auto r = std::format("{}", MapWrapper(m));
    auto e = string(R"({"1": "one", "2": "two", "3": "three"})");
    EXPECT_EQ(r, e);
}
TEST(Formatters, MapWrapper_empty){
    auto r = std::format("{}", MapWrapper(map<int, string>()));
    auto e = string(R"({})");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper){
    auto v = vector<int>{ 1, 2, 3 };
    auto r = std::format("{}", VecWrapper(v));
    auto e = string(R"([1, 2, 3])");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper_single) {
    auto v = vector<int>{42};
    auto r = std::format("{}", VecWrapper(v));
    auto e = string(R"([42])");
    EXPECT_EQ(r, e);
}
TEST(Formatters, VecWrapper_empty){
    auto r = std::format("{}", VecWrapper(vector<int>()));
    auto e = string(R"([])");
    EXPECT_EQ(r, e);
}