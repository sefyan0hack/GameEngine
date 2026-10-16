#include "OpenGLRenderer.hpp"

#include "Window.hpp"
#include "Camera.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "emath/vec3.hpp"
#include "emath/vec4.hpp"
#include "emath/uvec2.hpp"
#include "gl.hpp"

#include "../../res/shaders/defines.h"
#include "gl.inl"

#include <GL/glcorearb.h>
#include <core/Log.hpp>
#include <core/Exception.hpp>

#include <ui/Text.hpp>

#include <algorithm>
#include <ranges>
#include <chrono>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <numeric>

namespace UBO {

    struct Camera
    {
        constexpr static int32_t BINDING_POINT = 0;

        alignas(16) emath::mat4 Projection;
        alignas(16) emath::mat4 View;
        emath::vec3 Position;
    };

    struct SunLight
    {
        constexpr static int32_t BINDING_POINT = 1;

        alignas(16) emath::vec3 Direction;
        alignas(16) emath::vec3 Color;
        alignas(16) emath::vec3 Ambient;
    };

    struct Globle
    {
        constexpr static int32_t BINDING_POINT = 2;

        alignas(8) emath::uvec2 ScreenResolution;
        alignas(4) uint32_t Frame;
        alignas(4) uint32_t Flags;
    };

}

OpenGLRenderer::OpenGLRenderer(const OpenGL& ctx, Text& text)
    : m_GApi(ctx)
    , m_DrawMode(DrawMode::Triangles)
    , m_Depth {
        std::make_shared<ShaderProgram>("res/shaders/depth.vert", "res/shaders/depth.frag", "Depth pre-pass"),
        gl::create_querie("Depth Time Elapsed")
    }
    , m_Scene {
        std::make_shared<ShaderProgram>("res/shaders/scene.vert", "res/shaders/scene.frag", "Scene"),
        gl::create_querie("Scene Time Elapsed")
    }
    , m_SkyBox {
        std::make_shared<ShaderProgram>("res/shaders/skybox.vert", "res/shaders/skybox.frag", "SkyBox"),
        Texture::texture_cubemap("res/textures/forest.jpg"),
        gl::create_querie("SkyBox Time Elapsed")
    }
    , m_Text {
        text,
        std::make_shared<ShaderProgram>("res/shaders/text.vert", "res/shaders/text.frag", "Text"),
        gl::create_vertex_array("Text"), 
        gl::create_array_buffer("Text"),
        gl::create_texture("Text Atlas"),
        gl::create_querie("Text Time Elapsed")
    }
    , m_Ui {
        std::make_shared<ShaderProgram>("res/shaders/ui.vert", "res/shaders/ui.frag", "Ui")
    }
    , m_Stats()
    , m_Frame(0)
    , m_Gpu_time_elaped {
        {m_Depth.Program->name(), 0},
        {m_Scene.Program->name(), 0},
        {m_SkyBox.Program->name(), 0},
        {m_Text.Program->name(), 0}
    }
{

    set_depth(true);
    set_stencil(true);
    set_blend(true);
    set_face_cull(true);

    // Initialize buffers
    prepare_text_buffers();

    gl::BindTexture(GL_TEXTURE_2D, m_Text.Atlas);

    auto [w, h] = m_Text.Text.atlas_dims();
    std::vector<uint8_t> bitmap = m_Text.Text.bitmap(w,h);

    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.data());

    // Set texture parameters
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Prepeare Camera UBO --------------------------------------------------------------------------
    ShaderProgram::create_ubo("Camera", sizeof(UBO::Camera));
    m_Depth.Program->attach_ubo("Camera");
    m_Scene.Program->attach_ubo("Camera");
    m_SkyBox.Program->attach_ubo("Camera");

    // Prepeare Sun UBO --------------------------------------------------------------------------
    ShaderProgram::create_ubo("SunLight", sizeof(UBO::SunLight));
    m_Scene.Program->attach_ubo("SunLight");
    
    // Prepeare Globle UBO --------------------------------------------------------------------------
    ShaderProgram::create_ubo("Globle", sizeof(UBO::Globle));
    m_Text.Program->attach_ubo("Globle");
    m_Ui.Program->attach_ubo("Globle");

    {
        // set texture uints
        m_Scene.Program->use();
        static const std::vector<int32_t> units = []{
            std::vector<int32_t> a(OpenGL::MAX_FRAGMENT_TEXTURE_UNITS);
            std::iota(a.begin(), a.end(), 0);
            return a;
        }();
        m_Scene.Program->set_uniform("uDiffuseMaps[0]", units.data(), units.size());
    }
}

auto OpenGLRenderer::render(const Scene& scene) const -> void
{
    {
        auto& cam = scene.main_camera();

        // Uploading Camera UBO
        UBO::Camera data {
            .Projection = cam.projection(),
            .View = cam.view(),
            .Position = cam.position()
        };

        ShaderProgram::set_ubo("Camera", sizeof(UBO::Camera), &data);
    }

    {
        static auto start = std::chrono::steady_clock::now();

        float t = std::chrono::duration<float>(
            std::chrono::steady_clock::now() - start
        ).count();

        float angle  = t * 0.5f;
        float height = std::sin(angle);
        height = std::max(height, 0.1f);

        // Uploading Sun UBO
        UBO::SunLight data {
            .Direction = emath::vec3(
                std::cos(angle),
                -height,
                std::sin(angle)
            ),
            .Color = emath::vec3(1.0f),
            .Ambient = emath::vec3(0.4f)
        };

        ShaderProgram::set_ubo("SunLight", sizeof(UBO::SunLight), &data);
    }

    {
        // Uploading Globle UBO

        auto [w, h] = m_GApi.window().dims();
        auto ScreenResolution = emath::uvec2(w, h);

        UBO::Globle data {
            .ScreenResolution = ScreenResolution,
            .Frame = m_Frame,
            .Flags = 0
        };

        ShaderProgram::set_ubo("Globle", sizeof(UBO::Globle), &data);
    }

    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    {
        gl::begin_query_time_elapsed(m_Depth.time_elapsed);
        gl::DepthMask(GL_TRUE);
        gl::DepthFunc(GL_LESS);
        depthpre_pass(scene);
        gl::end_query_time_elapsed();
    }
    {
        gl::begin_query_time_elapsed(m_Scene.time_elapsed);
        gl::DepthMask(GL_FALSE);
        gl::DepthFunc(GL_LEQUAL);
        scene_pass(scene);
        gl::end_query_time_elapsed();
    }
    {
        gl::begin_query_time_elapsed(m_SkyBox.time_elapsed);
        gl::DepthMask(GL_FALSE);
        gl::DepthFunc(GL_LEQUAL);
        gl::Disable(GL_BLEND);
        skybox_pass();
        gl::end_query_time_elapsed();
    }

    gl::Enable(GL_BLEND);
    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_ALWAYS);

    {
        ui_pass();
    }

    {
        gl::begin_query_time_elapsed(m_Text.time_elapsed);
        text_pass();
        gl::end_query_time_elapsed();
    }

    gl::DepthMask(GL_TRUE);
    gl::DepthFunc(GL_LESS);

    gl::get_query_if_available(m_Depth.time_elapsed, &m_Gpu_time_elaped[m_Depth.Program->name()]);
    gl::get_query_if_available(m_Scene.time_elapsed, &m_Gpu_time_elaped[m_Scene.Program->name()]);
    gl::get_query_if_available(m_SkyBox.time_elapsed, &m_Gpu_time_elaped[m_SkyBox.Program->name()]);
    gl::get_query_if_available(m_Text.time_elapsed, &m_Gpu_time_elaped[m_Text.Program->name()]);

    m_Frame++;
}

auto OpenGLRenderer::depthpre_pass(const Scene& scene) const -> void
{
    gl::push_debug_group("depthpre_pass");

    gl::ColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    m_Depth.Program->use();

    auto Entities = scene.entities() | std::views::chunk_by(
        [](const GameObject& a, const GameObject& b)
        {
            return a.mesh() == b.mesh();
        }
    );

    for (const auto& group : Entities)
    {
        m_Depth.model_data.clear();

        Mesh* mesh = group.begin()->mesh().get();

        for (const auto& e : group) {
            m_Depth.model_data.emplace_back(e.model());
        }

        gl::BindVertexArray(mesh->VAO);

        gl::BindBuffer(GL_ARRAY_BUFFER, mesh->modelVBO);
        gl::BufferData(GL_ARRAY_BUFFER, m_Depth.model_data.size() * sizeof(decltype( m_Depth.model_data)::value_type), m_Depth.model_data.data(), GL_STREAM_DRAW);

        gl::DrawElementsInstanced(
            GL_TRIANGLES,
            int32_t(mesh->indices_size()),
            GL_UNSIGNED_SHORT,
            nullptr,
            m_Depth.model_data.size()
        );
    }

    gl::ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    gl::pop_debug_group();
}

auto OpenGLRenderer::scene_pass(const Scene& scene) const -> void
{
    gl::push_debug_group("scene_pass");

    m_Stats.reset();

    auto Entities = scene.entities() | std::views::chunk_by(
        [](const GameObject& a, const GameObject& b)
        {
            return a.mesh() == b.mesh();
        }
    );

    m_Scene.Program->use();
    
    for (auto group : Entities)
    {
        Mesh* mesh = group.begin()->mesh().get();

        gl::BindVertexArray(mesh->VAO);
        m_Stats.unique_mesh++;

        auto it = group.begin();

        while (it != group.end())
        {
            m_Scene.model_data.clear();
            m_Scene.tex_data.clear();

            std::vector<uint32_t> textureList;
            std::unordered_map<uint32_t, int32_t> texToUnit;

            for (;int32_t(textureList.size()) < OpenGL::MAX_FRAGMENT_TEXTURE_UNITS && it != group.end(); ++it)
            {
                uint32_t texID = it->material()->diffuse()->id();

                if (auto [iter, inserted] = texToUnit.try_emplace(texID, static_cast<int32_t>(textureList.size())); inserted) {
                    textureList.push_back(texID);
                }
                m_Scene.model_data.emplace_back(it->model());
                m_Scene.tex_data.emplace_back(texToUnit[texID]);
            }

            for (size_t i = 0; i < textureList.size(); ++i) {
                gl::ActiveTexture(GL_TEXTURE0 + static_cast<int32_t>(i));
                gl::BindTexture(GL_TEXTURE_2D, textureList[i]);
            }

            std::vector<int32_t> units(textureList.size());
            std::iota(units.begin(), units.end(), 0);
            m_Scene.Program->set_uniform("uDiffuseMaps[0]", units.data(), units.size());

            // send models matrix
            gl::BindBuffer(GL_ARRAY_BUFFER, mesh->modelVBO);
            gl::BufferData(GL_ARRAY_BUFFER, m_Scene.model_data.size() * sizeof(decltype(m_Scene.model_data)::value_type), m_Scene.model_data.data(), GL_STREAM_DRAW);

            // send texture slot id
            gl::BindBuffer(GL_ARRAY_BUFFER, mesh->texVBO);
            gl::BufferData(GL_ARRAY_BUFFER, m_Scene.tex_data.size() * sizeof(decltype(m_Scene.tex_data)::value_type), m_Scene.tex_data.data(), GL_STREAM_DRAW);

            gl::DrawElementsInstanced(
                GL_TRIANGLES,
                int32_t(mesh->indices_size()),
                GL_UNSIGNED_SHORT,
                nullptr,
                m_Scene.model_data.size()
            );

            m_Stats.draw_call++;
            m_Stats.vertices += mesh->vertex_size() * m_Scene.model_data.size();
            m_Stats.indices  += mesh->indices_size() * m_Scene.model_data.size();
        }
    }

    gl::pop_debug_group();
}

auto OpenGLRenderer::skybox_pass() const -> void
{
    gl::push_debug_group("skybox_pass");


    m_SkyBox.Program->use();

    gl::ActiveTexture(GL_TEXTURE0);
    m_SkyBox.Texture->bind();
    m_SkyBox.Program->set_uniform("uDiffuseMap", 0);

    // gl::BindVertexArray(VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, 3);

    gl::pop_debug_group();
}

auto OpenGLRenderer::text_pass() const -> void {
    gl::push_debug_group("text_pass");

    auto [width, height] = m_GApi.window().dims();
    m_Text.Text.fill_text_buffer(width, height);

    m_Text.Program->use();

    m_Text.Program->set_uniform("u_Color", Text::DEFAULT_FONT_COLOR);

    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_Text.Atlas);
    m_Text.Program->set_uniform("u_Texture", 0);

    gl::BindVertexArray(m_Text.VAO);
    
    auto text_glyphs_data = m_Text.Text.glyphs().data();
    auto text_glyphs_size = int32_t(m_Text.Text.glyphs().size());

    gl::BindBuffer(GL_ARRAY_BUFFER, m_Text.VBO);
    gl::BufferData(GL_ARRAY_BUFFER, text_glyphs_size * sizeof(Text::Glyph), text_glyphs_data, GL_STREAM_DRAW);

    gl::DrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, text_glyphs_size);

    m_Text.Text.clear_glyphs();
    m_Text.Text.clear();

    gl::pop_debug_group();
}

auto OpenGLRenderer::ui_pass() const -> void
{
    m_Ui.Program->use();

    // draw_rectangle({0,0}, {200, 200}, {0,0,0,0.5f});
    // gl::DrawArrays(GL_TRIANGLES, 0, 3);

    // draw_rounded_rectangle({400,200}, {50,50}, 10.0f, {0,0,0,1});
    // gl::DrawArrays(GL_TRIANGLES, 0, 3);

    // draw_circle({300,400}, 60, {0,0,0,1});
    // gl::DrawArrays(GL_TRIANGLES, 0, 3);

    // draw_circle_outline({600,400}, 60, 10, {0,0,0,1});
    // gl::DrawArrays(GL_TRIANGLES, 0, 3);

    // draw_line({0,0}, {500, 500}, 40, {0,0,0,1});
    // gl::DrawArrays(GL_TRIANGLES, 0, 3);

    // draw_rounded_rectangle_outline({0,0}, {500, 500}, 40, 30, {0,0,0,1});
    // gl::DrawArrays(GL_TRIANGLES, 0, 3);

    {
        auto pos = emath::vec2{600, 300};
        draw_rounded_rectangle(pos, {160,40}, 10.0f, {0,0,0,0.8f});
        gl::DrawArrays(GL_TRIANGLES, 0, 3);
        m_Text.Text.text("Submit", pos);
    }

}

auto OpenGLRenderer::set_viewport(int32_t x, int32_t y, int32_t width, int32_t height) -> void
{
    gl::Viewport(x, y, width, height);
}

auto OpenGLRenderer::set_mode(DrawMode mode) -> void
{
    m_DrawMode = mode;
}

auto OpenGLRenderer::clear_screen(uint32_t buffersmask) const -> void
{
    gl::Clear((uint32_t)buffersmask);
}

auto OpenGLRenderer::stats() const -> RenderStats
{
    return m_Stats;
}

auto OpenGLRenderer::prepare_text_buffers() -> void {
    // Generate and bind VAO
    gl::BindVertexArray(m_Text.VAO);

    // Dynamic instance VBO
    gl::BindBuffer(GL_ARRAY_BUFFER, m_Text.VBO);

    // Offset (2 * 4 byte)
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Text::Glyph), (void*)offsetof(Text::Glyph, offset));
    gl::VertexAttribDivisor(0, 1);

    // TexRect (4 * 2 byte)
    gl::EnableVertexAttribArray(1);
    gl::VertexAttribIPointer(1, 4, GL_UNSIGNED_SHORT, sizeof(Text::Glyph), (void*)offsetof(Text::Glyph, texRect));
    gl::VertexAttribDivisor(1, 1);
}


auto  OpenGLRenderer::set_depth(bool v) const -> void
{
    if (v) {
        gl::Enable(GL_DEPTH_TEST);
    } else gl::Disable(GL_DEPTH_TEST);
}

auto  OpenGLRenderer::set_stencil(bool v) const -> void
{
    if (v) {
        gl::Enable(GL_STENCIL_TEST);
        gl::StencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    } else gl::Disable(GL_STENCIL_TEST);
}

auto  OpenGLRenderer::set_blend(bool v) const -> void
{
    if (v) {
        gl::Enable(GL_BLEND);
        gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else gl::Disable(GL_BLEND);

}

auto  OpenGLRenderer::set_face_cull(bool v) const -> void
{
    if (v){ 
        gl::Enable(GL_CULL_FACE);
        gl::CullFace(GL_BACK);
    }  else gl::Disable(GL_CULL_FACE);
}

auto OpenGLRenderer::gpu_time_elapsed() const -> std::unordered_map<std::string, uint64_t>&
{
    return m_Gpu_time_elaped;
}

auto OpenGLRenderer::draw_rectangle(emath::vec2 pos, emath::vec2 size, emath::vec4 color) const -> void
{
    auto PosSize = emath::vec4(pos.x, pos.y, size.x, size.y);
    auto params = emath::vec4{ 0, 0, 0, 0};

    m_Ui.Program->set_uniform("uPosition", PosSize);
    m_Ui.Program->set_uniform("uColor", color);

    m_Ui.Program->set_uniform("uShape", GLSL_SHAPE_rectangle);
    m_Ui.Program->set_uniform("uParams", params);
}

auto OpenGLRenderer::draw_circle(emath::vec2 pos, float radius, emath::vec4 color) const -> void
{
    auto PosSize = emath::vec4(pos.x, pos.y, 0, 0);
    auto params = emath::vec4{ radius, 0, 0, 0};

    m_Ui.Program->set_uniform("uPosition", PosSize);
    m_Ui.Program->set_uniform("uColor", color);

    m_Ui.Program->set_uniform("uShape", GLSL_SHAPE_circle);
    m_Ui.Program->set_uniform("uParams", params);
}

auto OpenGLRenderer::draw_rectangle_outline(emath::vec2 pos, emath::vec2 size, float thikness, emath::vec4 color) const-> void
{
    auto PosSize = emath::vec4{pos.x, pos.y, size.x, size.y};
    auto params = emath::vec4{ thikness, 0, 0, 0};

    m_Ui.Program->set_uniform("uPosition", PosSize);
    m_Ui.Program->set_uniform("uColor", color);

    m_Ui.Program->set_uniform("uShape", GLSL_SHAPE_rectangle_outline);
    m_Ui.Program->set_uniform("uParams", params);
}

auto OpenGLRenderer::draw_rounded_rectangle(emath::vec2 pos, emath::vec2 size, float radius, emath::vec4 color) const-> void
{
    auto PosSize = emath::vec4{pos.x, pos.y, size.x, size.y};
    auto params = emath::vec4{ radius, 0, 0, 0};

    m_Ui.Program->set_uniform("uPosition", PosSize);
    m_Ui.Program->set_uniform("uColor", color);

    m_Ui.Program->set_uniform("uShape", GLSL_SHAPE_rounded_rectangle);
    m_Ui.Program->set_uniform("uParams", params);
}

auto OpenGLRenderer::draw_circle_outline(emath::vec2 pos, float radius, float thikness, emath::vec4 color) const -> void
{
    auto pos_thinkess = emath::vec4{pos.x, pos.y, 0, 0};

    auto params = emath::vec4{ radius, thikness, 0, 0};

    m_Ui.Program->set_uniform("uPosition", pos_thinkess);
    m_Ui.Program->set_uniform("uColor", color);

    m_Ui.Program->set_uniform("uShape", GLSL_SHAPE_circle_outline);
    m_Ui.Program->set_uniform("uParams", params);
}

auto OpenGLRenderer::draw_line(emath::vec2 p1, emath::vec2 p2, float thikness, emath::vec4 color) const -> void
{
    auto p1p2 = emath::vec4{p1.x, p1.y, p2.x, p2.y};
    auto params = emath::vec4{ thikness, 0, 0, 0};

    m_Ui.Program->set_uniform("uPosition", p1p2);
    m_Ui.Program->set_uniform("uColor", color);

    m_Ui.Program->set_uniform("uShape", GLSL_SHAPE_line);
    m_Ui.Program->set_uniform("uParams", params);
}

auto OpenGLRenderer::draw_rounded_rectangle_outline(emath::vec2 pos, emath::vec2 size, float radius, float thikness, emath::vec4 color) const-> void
{
    auto PosSize = emath::vec4{pos.x, pos.y, size.x, size.y};
    auto params = emath::vec4{ radius, thikness, 0, 0};

    m_Ui.Program->set_uniform("uPosition", PosSize);
    m_Ui.Program->set_uniform("uColor", color);

    m_Ui.Program->set_uniform("uShape", GLSL_SHAPE_rounded_rectangle_outline);
    m_Ui.Program->set_uniform("uParams", params);
}
