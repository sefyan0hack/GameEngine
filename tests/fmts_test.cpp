#include <gtest/gtest.h>
#include <core/fmts.hpp>
#include <core/Transform.hpp>
#include <core/Camera.hpp>
#include <core/Mesh.hpp>
#include <core/Shader.hpp>
#include <core/Material.hpp>
#include <core/GameObject.hpp>
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

TEST(Formatters, Transform) {
    auto obj = Transform(vec3());
    auto r = format("{}", obj);
    auto e = format(
        R"({{ "position": {}, "rotation": {}, "scale": {} }})", 
        obj.position, obj.rotation, obj.scale
    );
    EXPECT_EQ(r, e);
}

TEST(Formatters, Camera){
    auto obj = Camera();
    auto r = std::format("{}", obj);
    auto e = format(
        R"({{ "position": {}, "sensitivity": {}, "view": {} }})",
        obj.Position(), obj.Sensitivity(), obj.View()
    );

    EXPECT_EQ(r, e);
}

TEST(Formatters, Shader){
    auto obj = Shader();
    auto r = std::format("{}", obj);
    auto e = format(
        R"({{ "id": {}, "type": "{}" }})",
        obj.id(), obj.TypeName()
    );
    EXPECT_EQ(r, e);
}

TEST(Formatters, Mesh){
    auto obj = Mesh(std::vector<Vertex>());
    auto r = std::format("{}", obj);
    auto e = format(
        R"({{ "name": "{}", "VAO": {}, "VBO": {}, "EBO": {}, "verticesSize": {} }})",
        obj.name, obj.VAO, obj.VBO, obj.EBO, obj.VextexSize()
    );
    EXPECT_EQ(r, e);
}


// TEST(Formatters, Material){
//     auto obj = Material(Shader(), Shader());
//     auto r = std::format("{}", obj);
//     auto e = format(
//         R"({{ "id": {}, "attribs": {}, "uniforms": {} }})",
//         obj.id(), MapWrapper{obj.Attribs()}, MapWrapper{obj.Uniforms()}
//     );
//     EXPECT_EQ(r, e);
// }

// TEST(Formatters, GameObject){
//     auto m = Material(Shader(), Shader());
//     auto me = Mesh(std::vector<Vertex>());
//     auto obj = GameObject(vec3(), m, me);
//     auto r = std::format("{}", obj);
//     auto e = format(
//         R"({{"name": "{}", "transform": {}, "material": {}, "mesh": {} }})",
//         obj.Name(), obj.transform(), *obj.material(), *obj.mesh()
//     );
//     EXPECT_EQ(r, e);
// }