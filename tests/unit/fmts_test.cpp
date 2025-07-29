#include <gtest/gtest.h>
#include <core/fmts.hpp>
#include <core/Transform.hpp>
#include <core/OpenGL.hpp>
#include <core/Camera.hpp>
#include <core/Mesh.hpp>
#include <core/Shader.hpp>
#include <core/Material.hpp>
#include <core/GameObject.hpp>
#include <core/Log.hpp>
#include <format>
#include <iostream>
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
    auto m = map<int32_t, string>{ {1, "one"}, {2, "two"}, {3, "three"} };
    auto r = std::format("{}", MapWrapper(m));
    auto e = string(R"({"1": "one", "2": "two", "3": "three"})");
    EXPECT_EQ(r, e);
}
TEST(Formatters, MapWrapper_empty){
    auto r = std::format("{}", MapWrapper(map<int32_t, string>()));
    auto e = string(R"({})");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper){
    auto v = vector<int32_t>{ 1, 2, 3 };
    auto r = std::format("{}", VecWrapper(v));
    auto e = string(R"([1, 2, 3])");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper_single) {
    auto v = vector<int32_t>{42};
    auto r = std::format("{}", VecWrapper(v));
    auto e = string(R"([42])");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper_empty){
    auto r = std::format("{}", VecWrapper(vector<int32_t>()));
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
        R"({{ "position": {}, "view": {} }})",
        obj.Position(), obj.View()
    );

    EXPECT_EQ(r, e);
}

// disable opengl tests `Github Runner are headless`

TEST(Formatters, DISABLED_Shader){
    IF_THROWS_IGNOR({
        auto shader = Shader();

        auto r = std::format("{}", shader);
        auto e = format(
            R"({{ "id": {}, "type": "{}", "file": "{}" }})",
            shader.id(), shader.TypeName(), shader.File()
        );
        EXPECT_EQ(r, e);
    });
}

TEST(Formatters, DISABLED_Mesh){
    auto mesh = Mesh(std::vector<Vertex>());
        
    auto r = std::format("{}", mesh);
    auto e = format(
        R"({{ "name": "{}", "VAO": {}, "VBO": {}, "EBO": {}, "verticesSize": {} }})",
        mesh.name, mesh.VAO, mesh.VBO, mesh.EBO, mesh.VextexSize()
    );
    EXPECT_EQ(r, e);
}

TEST(Formatters, DISABLED_Material){
    IF_THROWS_IGNOR({
        auto shader_vert = std::make_shared<Shader>(SHADER(skybox)".vert", GL_VERTEX_SHADER);
        auto shader_frag = std::make_shared<Shader>(SHADER(skybox)".frag", GL_FRAGMENT_SHADER);

        auto material = Material(shader_vert, shader_frag);

        auto r = std::format("{}", material);
        auto e = format(
            R"({{ "Program": {} }})",
            *material.Program()
        );
        EXPECT_EQ(r, e);
    });
}

TEST(Formatters, DISABLED_GameObject){
    IF_THROWS_IGNOR({
        auto shader_vert = std::make_shared<Shader>(SHADER(skybox)".vert", GL_VERTEX_SHADER);
        auto shader_frag = std::make_shared<Shader>(SHADER(skybox)".frag", GL_FRAGMENT_SHADER);
        auto mesh = std::make_shared<Mesh>(std::vector<Vertex>());
        auto material = std::make_shared<Material>(shader_vert, shader_frag);
        
        auto gameobj = GameObject(vec3(), material, mesh);
        
        auto r = std::format("{}", gameobj);
        auto e = format(
            R"({{"name": "{}", "transform": {}, "material": {}, "mesh": {} }})",
            gameobj.Name(), gameobj.transform(), *gameobj.material(), *gameobj.mesh()
        );
        EXPECT_EQ(r, e);
    });
}
