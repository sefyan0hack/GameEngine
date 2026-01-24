#include <gtest/gtest.h>
#include <core/fmts.hpp>
#include <graphics/Transform.hpp>
#include <graphics/OpenGL.hpp>
#include <graphics/Mesh.hpp>
#include <graphics/Shader.hpp>
#include <graphics/Material.hpp>
#include <graphics/GameObject.hpp>
#include <graphics/Camera.hpp>
#include <format>
#include <map>
#include <vector>

#include <core/res.hpp>
extern cmrc::embedded_filesystem embed_filesystem;
#ifdef __cpp_lib_formatters

using namespace glm;

TEST(Formatters, glm_vec2){
    auto r = ::format("{}", vec2());
    auto e = std::string(R"({ "x": 0, "y": 0 })");
    EXPECT_EQ(r, e);
}

TEST(Formatters, glm_vec3){
    auto r = ::format("{}", vec3());
    auto e = std::string(R"({ "x": 0, "y": 0, "z": 0 })");
    EXPECT_EQ(r, e);
}

TEST(Formatters, glm_mat4){
    auto r = ::format("{}", mat4());
    auto e = std::string("[[ 0, 0, 0, 0 ], [ 0, 0, 0, 0 ], [ 0, 0, 0, 0 ], [ 0, 0, 0, 0 ]]");
    EXPECT_EQ(r, e);
}

TEST(Formatters, MapWrapper){
    auto m = std::map<int32_t, std::string>{ {1, "one"}, {2, "two"}, {3, "three"} };
    auto r = ::format("{}", MapWrapper(m));
    auto e = std::string(R"({ "1": "one", "2": "two", "3": "three" })");
    EXPECT_EQ(r, e);
}
TEST(Formatters, MapWrapper_empty){
    auto r = ::format("{}", MapWrapper(std::map<int32_t, std::string>()));
    auto e = std::string(R"({  })");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper){
    auto v = std::vector<int32_t>{ 1, 2, 3 };
    auto r = ::format("{}", VecWrapper(v));
    auto e = std::string(R"([ 1, 2, 3 ])");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper_single) {
    auto v = std::vector<int32_t>{42};
    auto r = ::format("{}", VecWrapper(v));
    auto e = std::string(R"([ 42 ])");
    EXPECT_EQ(r, e);
}

TEST(Formatters, VecWrapper_empty){
    auto r = ::format("{}", VecWrapper(std::vector<int32_t>()));
    auto e = std::string(R"([  ])");
    EXPECT_EQ(r, e);
}

TEST(Formatters, Transform) {
    auto obj = Transform(vec3());
    auto r = ::format("{}", obj);
    auto e = ::format(
        R"({{ "position": {}, "rotation": {}, "scale": {} }})", 
        obj.position, obj.rotation, obj.scale
    );
    EXPECT_EQ(r, e);
}

TEST(Formatters, Camera){
    auto obj = Camera();
    auto r = ::format("{}", obj);
    auto e = ::format(
        R"({{ "position": {}, "view": {} }})",
        obj.position(), obj.view()
    );

    EXPECT_EQ(r, e);
}

// disable opengl tests `Github Runner are headless`

TEST(Formatters, DISABLED_Shader){
    auto shader = Shader();

    auto r = ::format("{}", shader);
    auto e = ::format(
        R"({{ "id": {}, "type": "{}" }})",
        shader.id(), shader.type_name()
    );
    EXPECT_EQ(r, e);
}

TEST(Formatters, DISABLED_Mesh){
    auto mesh = Mesh(std::vector<Vertex>());
        
    auto r = ::format("{}", mesh);
    auto e = ::format(
        R"({{ "VAO": {}, "VBO": {}, "verticesSize": {} }})",
         mesh.VAO, mesh.VBO, mesh.vextex_size()
    );
    EXPECT_EQ(r, e);
}

// TEST(Formatters, DISABLED_Material){
//     auto shader_vert = ::make_shared<Shader>(embed_filesystem.open("res/Shaders/skybox.vert"), GL_VERTEX_SHADER);
//     auto shader_frag = ::make_shared<Shader>(embed_filesystem.open("res/Shaders/skybox.frag"), GL_FRAGMENT_SHADER);

//     auto material = Material();

//     auto r = ::format("{}", material);
//     auto e = format(
//         R"({{ "Program": {} }})", // ??
//         *material.program()
//     );
//     EXPECT_EQ(r, e);
// }

TEST(Formatters, DISABLED_GameObject){
    auto shader_vert = std::make_shared<Shader>(embed_filesystem.open("res/Shaders/skybox.vert"), GL_VERTEX_SHADER);
    auto shader_frag = std::make_shared<Shader>(embed_filesystem.open("res/Shaders/skybox.frag"), GL_FRAGMENT_SHADER);
    auto mesh = std::make_shared<Mesh>(std::vector<Vertex>());
    auto material = std::make_shared<Material>();
    
    auto gameobj = GameObject(vec3(), material, mesh);
    
    auto r = ::format("{}", gameobj);
    auto e = format(
        R"({{ "transform": {} }})",
        gameobj.transform()
    );
    EXPECT_EQ(r, e);
}
#endif
