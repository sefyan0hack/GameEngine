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
#include "gl.hpp"

#include <core/Log.hpp>
#include <core/Exception.hpp>

#include <ui/Text.hpp>

#include <algorithm>
#include <chrono>
#include <cstddef>

#if defined(min) || defined(max)
#undef min
#undef max
#endif

namespace UBO {

    struct alignas(16) Camera
    {
        constexpr static int32_t BINDING_POINT = 0;
        alignas(16) emath::mat4 Projection;
        alignas(16) emath::mat4 View;
        emath::vec3 Position;
    };

    struct alignas(16) SunLight
    {
        constexpr static int32_t BINDING_POINT = 1;
        alignas(16) emath::vec3 Direction;
        alignas(16) emath::vec3 Color;
        alignas(16) emath::vec3 Ambient;
    };

}

OpenGLRenderer::OpenGLRenderer(const OpenGL& ctx, Text& text)
    : m_GApi(ctx)
    , m_DrawMode(DrawMode::Triangles)
    , m_Depth {
        std::make_shared<ShaderProgram>("res/shaders/depth.vert", "res/shaders/depth.frag")
    }
    , m_Scene {
        std::make_shared<ShaderProgram>("res/shaders/scene.vert", "res/shaders/scene.frag")
    }
    , m_SkyBox {
        std::make_shared<ShaderProgram>("res/shaders/skybox.vert", "res/shaders/skybox.frag"),
        Texture::texture_cubemap("res/textures/forest.jpg")
    }
    , m_Text {
        text,
        std::make_shared<ShaderProgram>("res/shaders/text.vert", "res/shaders/text.frag"),
        0, 0, 0
    }
    , m_CameraUBO(0), m_SunUBO(0)
{
    {
        gl::label_program(m_Depth.Program->id(), "Depth pre-pass");
        gl::label_program(m_Scene.Program->id(), "Scene");
        gl::label_program(m_SkyBox.Program->id(), "SkyBox");
        gl::label_program(m_Text.Program->id(), "Text");
    }

    set_depth(true);
    set_stencil(true);
    set_blend(true);
    set_face_cull(true);

    // Initialize buffers
    prepare_text_buffers();

    gl::GenTextures(1, &m_Text.Atlas);
    gl::label_texture(m_Text.Atlas, "Text Atlas");

    gl::BindTexture(GL_TEXTURE_2D, m_Text.Atlas);

    auto [w, h] = m_Text.Text.atlas_dims();
    std::vector<uint8_t> bitmap = m_Text.Text.bitmap(w,h);

    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.data());

    // Set texture parameters
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Prepeare Camera UBO --------------------------------------------------------------------------
    gl::GenBuffers(1, &m_CameraUBO);
    gl::label_buffer(m_CameraUBO, "Camera UBO");

    gl::BindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);

    gl::BufferData(GL_UNIFORM_BUFFER, sizeof(UBO::Camera), nullptr, GL_DYNAMIC_DRAW);

    gl::BindBufferBase(GL_UNIFORM_BUFFER, UBO::Camera::BINDING_POINT, m_CameraUBO);

    // Link shaders uniform block to binding point
    {
        uint32_t DepthblockIndex = gl::GetUniformBlockIndex(m_Depth.Program->id(), "Camera");
        gl::UniformBlockBinding(m_Depth.Program->id(), DepthblockIndex, UBO::Camera::BINDING_POINT);

        uint32_t SceneblockIndex = gl::GetUniformBlockIndex(m_Scene.Program->id(), "Camera");
        gl::UniformBlockBinding(m_Scene.Program->id(), SceneblockIndex, UBO::Camera::BINDING_POINT);

        uint32_t SkyBoxblockIndex = gl::GetUniformBlockIndex(m_SkyBox.Program->id(), "Camera");
        gl::UniformBlockBinding(m_SkyBox.Program->id(), SkyBoxblockIndex, UBO::Camera::BINDING_POINT);
    }

    // Prepeare Sun UBO --------------------------------------------------------------------------
    gl::GenBuffers(1, &m_SunUBO);
    gl::label_buffer(m_SunUBO, "Sun UBO");

    gl::BindBuffer(GL_UNIFORM_BUFFER, m_SunUBO);

    gl::BufferData(GL_UNIFORM_BUFFER, sizeof(UBO::SunLight), nullptr, GL_DYNAMIC_DRAW);

    gl::BindBufferBase(GL_UNIFORM_BUFFER, UBO::SunLight::BINDING_POINT, m_SunUBO);

    // Link shaders uniform block to binding point
    {
        uint32_t SceneblockIndex = gl::GetUniformBlockIndex(m_Scene.Program->id(), "SunLight");
        gl::UniformBlockBinding(m_Scene.Program->id(), SceneblockIndex, UBO::SunLight::BINDING_POINT);
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

        gl::BindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);
        gl::BufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
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

        gl::BindBuffer(GL_UNIFORM_BUFFER, m_SunUBO);
        gl::BufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
    }

    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // TODO: enable when im fragment-bound
    // gl::DepthMask(GL_TRUE);
    // gl::DepthFunc(GL_LESS);
    // depthpre_pass(scene);
    // gl::DepthMask(GL_FALSE);
    // gl::DepthFunc(GL_LEQUAL);
    scene_pass(scene);
    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_LEQUAL);
    skybox_pass();
    gl::DepthMask(GL_FALSE);
    gl::DepthFunc(GL_ALWAYS);
    text_pass();
    gl::DepthMask(GL_TRUE);
    gl::DepthFunc(GL_LESS);
}

auto OpenGLRenderer::depthpre_pass(const Scene& scene) const -> void
{
    gl::push_debug_group("depthpre_pass");

    gl::ColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    m_Depth.Program->use();
    m_Stats.pipeline_switch++;

    Mesh* currentMesh = nullptr;

    for (const auto& obj : scene.entities())
    {
        auto mesh = obj.mesh().get();

        m_Depth.Program->set_uniform("Model", obj.model());

        if (currentMesh != mesh)
        {
            currentMesh = mesh;
            gl::BindVertexArray(mesh->VAO);
            m_Stats.mesh_switch++;
        }

        gl::DrawElements(GL_TRIANGLES, int32_t(mesh->indices_size()), GL_UNSIGNED_SHORT, (void*)0);
        m_Stats.draw_call++;
    }

    gl::ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    gl::pop_debug_group();
}

auto OpenGLRenderer::scene_pass(const Scene& scene) const -> void
{
    gl::push_debug_group("scene_pass");

    m_Stats.reset();

    m_Scene.Program->use();
    m_Stats.pipeline_switch++;

    //Drwaing
    Mesh* currentMesh = nullptr;
    Material* currentMaterial = nullptr;

    for(const auto& obj : scene.entities()) {
        auto mesh = obj.mesh().get();
        auto material = obj.material().get();

        auto v_count = mesh->vertex_size();
        auto i_count = mesh->indices_size();

        m_Stats.vertices += v_count;
        m_Stats.indices += i_count;

        m_Scene.Program->set_uniform("Model", obj.model());

        // Bind material only when it changes
        if (currentMaterial != material)
        {
            currentMaterial = material;

            // diffuse
            gl::ActiveTexture(GL_TEXTURE0 + 0);
            currentMaterial->diffuse()->bind();
            m_Scene.Program->set_uniform("uDiffuseMap", 0);

            m_Stats.texture_switch++;
        }

        // Bind mesh only when it changes
        if (currentMesh != mesh)
        {
            currentMesh = mesh;
            gl::BindVertexArray(currentMesh->VAO);
            m_Stats.mesh_switch++;
        }

        gl::DrawElements(GL_TRIANGLES, int32_t(mesh->indices_size()), GL_UNSIGNED_SHORT, (void*)0);
        m_Stats.draw_call++;
    }
    gl::pop_debug_group();
}

auto OpenGLRenderer::skybox_pass() const -> void
{
    gl::push_debug_group("skybox_pass");


    m_SkyBox.Program->use();
    m_Stats.pipeline_switch++;

    gl::ActiveTexture(GL_TEXTURE0);
    m_SkyBox.Texture->bind();
    m_SkyBox.Program->set_uniform("uDiffuseMap", 0);
    m_Stats.texture_switch++;

    m_Stats.vertices += 3;
    // gl::BindVertexArray(VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, 3);
    m_Stats.draw_call++;

    gl::pop_debug_group();
}

auto OpenGLRenderer::text_pass() const -> void {
    gl::push_debug_group("text_pass");

    auto [width, height] = m_GApi.window().dims();
    m_Text.Text.fill_text_buffer(width, height);

    m_Text.Program->use();
    m_Stats.pipeline_switch++;

    uint32_t u_ScreenSize = uint32_t(width) | (uint32_t(height) << 16);
    m_Text.Program->set_uniform("u_ScreenSize", u_ScreenSize);

    auto r = uint8_t(std::clamp(Text::DEFAULT_FONT_COLOR.x, 0.0f, 1.0f) * 255.0f + 0.5f);
    auto g = uint8_t(std::clamp(Text::DEFAULT_FONT_COLOR.y, 0.0f, 1.0f) * 255.0f + 0.5f);
    auto b = uint8_t(std::clamp(Text::DEFAULT_FONT_COLOR.z, 0.0f, 1.0f) * 255.0f + 0.5f);

    auto color =
        (uint32_t(r)      ) |
        (uint32_t(g) << 8 ) |
        (uint32_t(b) << 16) |
        (uint32_t(255) << 24);

    m_Text.Program->set_uniform("u_Color", color);

    gl::ActiveTexture(GL_TEXTURE0);
    gl::BindTexture(GL_TEXTURE_2D, m_Text.Atlas);
    m_Text.Program->set_uniform("u_Texture", 0);

    gl::BindVertexArray(m_Text.VAO);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_Text.VBO);
    m_Stats.texture_switch++;

    auto text_glyphs = m_Text.Text.glyphs();

    for (size_t offset = 0; offset < text_glyphs.size(); offset += TEXT_BATCH_SIZE)
    {
        auto batchCount = std::min(TEXT_BATCH_SIZE, text_glyphs.size() - offset);
        ptrdiff_t bytesToCopy = batchCount * sizeof(Text::Glyph);

        void* mappedMemory = gl::MapBufferRange(GL_ARRAY_BUFFER, 0, bytesToCopy, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        if (mappedMemory) {
            m_Stats.vertices += 4;
            std::memcpy(mappedMemory, text_glyphs.data() + offset, bytesToCopy);
            gl::UnmapBuffer(GL_ARRAY_BUFFER);
            gl::DrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<int32_t>(batchCount));
            m_Stats.draw_call++;
        }
    }

    m_Text.Text.clear_glyphs();
    m_Text.Text.clear();

    gl::pop_debug_group();
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
    gl::GenVertexArrays(1, &m_Text.VAO);
    gl::label_vertex_array(m_Text.VAO, "Text VAO");

    gl::BindVertexArray(m_Text.VAO);

    // Dynamic instance VBO
    gl::GenBuffers(1, &m_Text.VBO);
    gl::label_buffer(m_Text.VBO, "Text VBO");

    gl::BindBuffer(GL_ARRAY_BUFFER, m_Text.VBO);
    gl::BufferData(GL_ARRAY_BUFFER, TEXT_BATCH_SIZE * sizeof(Text::Glyph), nullptr, GL_STREAM_DRAW);

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


    


    