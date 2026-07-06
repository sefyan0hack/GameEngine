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
#include "gl.hpp"

#include <core/Log.hpp>
#include <core/Exception.hpp>

#include <ui/Text.hpp>

#include <algorithm>

#if defined(min) || defined(max)
#undef min
#undef max
#endif

struct CameraUBO
{
    constexpr static int32_t BINDING_POINT = 0;
    emath::mat4 Projection;
    emath::mat4 View;
};

OpenGLRenderer::OpenGLRenderer(const OpenGL& ctx, Text& text)
    : m_GApi(ctx)
    , m_DrawMode(DrawMode::Triangles)
    , m_Depth {
        std::make_shared<ShaderProgram>("res/Shaders/depth.vert", "res/Shaders/depth.frag")
    }
    , m_Scene {
        std::make_shared<ShaderProgram>("res/Shaders/scene.vert", "res/Shaders/scene.frag")
    }
    , m_SkyBox {
        std::make_shared<ShaderProgram>("res/Shaders/skybox.vert", "res/Shaders/skybox.frag"),
        std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6facesfiles("res/forest.jpg"))
    }
    , m_Text {
        text,
        std::make_shared<ShaderProgram>("res/Shaders/text.vert", "res/Shaders/text.frag"),
        0, 0, 0
    }
    , m_CameraUBO(0)
{
    set_depth(true);
    set_stencil(true);
    set_blend(true);
    set_face_cull(true);

    // Initialize buffers
    prepare_text_buffers();

    gl::GenTextures(1, &m_Text.Atlas);
    gl::BindTexture(GL_TEXTURE_2D, m_Text.Atlas);

    auto [w, h] = m_Text.Text.atlas_dims();
    std::vector<uint8_t> bitmap = m_Text.Text.bitmap(w,h);

    gl::TexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.data());

    // Set texture parameters
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Prepeare Camera UBO --------------------------------------------------------------------------
    gl::GenBuffers(1, &m_CameraUBO);
    gl::BindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);

    // 2 mat4 = 128 bytes
    gl::BufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW);

    // Binding point 0
    gl::BindBufferBase(GL_UNIFORM_BUFFER, 0, m_CameraUBO);

    // Link shaders uniform block to binding point
    {
        GLuint DepthblockIndex = gl::GetUniformBlockIndex(m_Depth.Program->id(), "Camera");
        gl::UniformBlockBinding(m_Depth.Program->id(), DepthblockIndex, CameraUBO::BINDING_POINT);

        GLuint SceneblockIndex = gl::GetUniformBlockIndex(m_Scene.Program->id(), "Camera");
        gl::UniformBlockBinding(m_Scene.Program->id(), SceneblockIndex, CameraUBO::BINDING_POINT);

        GLuint SkyBoxblockIndex = gl::GetUniformBlockIndex(m_SkyBox.Program->id(), "Camera");
        gl::UniformBlockBinding(m_SkyBox.Program->id(), SkyBoxblockIndex, CameraUBO::BINDING_POINT);
    }
}

auto OpenGLRenderer::render(const Scene& scene) const -> void
{
    { // Uploading Camera UBO
        CameraUBO data {
            scene.main_camera().projection(),
            scene.main_camera().view()
        };
        
        gl::BindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);
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

    gl::ColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    m_Depth.Program->use();
    m_Stats.shaderBinds++;

    Mesh* currentMesh = nullptr;

    for (const auto& obj : scene.entities())
    {
        auto mesh = obj.mesh().get();

        m_Depth.Program->set_uniform("Model", obj.model());

        if (currentMesh != mesh)
        {
            currentMesh = mesh;
            gl::BindVertexArray(mesh->VAO);
        }

        gl::DrawArrays(GL_TRIANGLES, 0, mesh->vertex_size());
    }

    gl::ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

auto OpenGLRenderer::scene_pass(const Scene& scene) const -> void
{
    m_Stats.reset();

    m_Scene.Program->use();
    m_Stats.shaderBinds++;

    //Drwaing
    Mesh* currentMesh = nullptr;
    Material* currentMaterial = nullptr;

    std::for_each(
        scene.entities().begin(), scene.entities().end(),
        [&](const auto& obj){

            auto mesh = obj.mesh().get();
            auto material = obj.material().get();

            auto v_count = mesh->vertex_size();
            m_Stats.vertex_cout += v_count;

            m_Scene.Program->set_uniform("Model", obj.model());

            // Bind material only when it changes
            if (currentMaterial != material)
            {
                currentMaterial = material;
                currentMaterial->bind(m_Scene.Program);
                m_Stats.materialBinds++;
            }

            // Bind mesh only when it changes
            if (currentMesh != mesh)
            {
                currentMesh = mesh;
                gl::BindVertexArray(currentMesh->VAO);
                m_Stats.vaoBinds++;
            }

            switch(m_DrawMode)
            {
                case DrawMode::Triangles:
                    gl::DrawArrays(GL_TRIANGLES, 0, v_count);
                    m_Stats.drawCalls++;
                    break;
                case DrawMode::Line:
                    gl::DrawArrays(GL_LINES, 0, (v_count / 3) * 6);
                    m_Stats.drawCalls++;
                    break;
                case DrawMode::Point:
                    gl::DrawArrays(GL_POINTS, 0, v_count);
                    m_Stats.drawCalls++;
                    break;
            }
        }
    );
}

auto OpenGLRenderer::skybox_pass() const -> void
{
    m_SkyBox.Program->use();
    m_Stats.shaderBinds++;

    gl::ActiveTexture(GL_TEXTURE0);
    m_SkyBox.Texture->bind();
    m_SkyBox.Program->set_uniform("uDiffuseMap", 0);

    // gl::BindVertexArray(VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, 3);
}

auto OpenGLRenderer::text_pass() const -> void {
    auto [width, height] = m_GApi.window().dims();
    m_Text.Text.fill_text_buffer(width, height);

    m_Text.Program->use();
    m_Stats.shaderBinds++;

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

    auto text_glyphs = m_Text.Text.glyphs();

    for (size_t offset = 0; offset < text_glyphs.size(); offset += TEXT_BATCH_SIZE)
    {
        auto batchCount = std::min(TEXT_BATCH_SIZE, text_glyphs.size() - offset);
        GLsizeiptr bytesToCopy = batchCount * sizeof(Text::Glyph);

        void* mappedMemory = gl::MapBufferRange(GL_ARRAY_BUFFER, 0, bytesToCopy, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        if (mappedMemory) {
            std::memcpy(mappedMemory, text_glyphs.data() + offset, bytesToCopy);
            gl::UnmapBuffer(GL_ARRAY_BUFFER);
            gl::DrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, static_cast<GLsizei>(batchCount));
        }
    }

    m_Text.Text.clear_glyphs();
    m_Text.Text.clear();
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
    gl::Clear((GLenum)buffersmask);
}

auto OpenGLRenderer::extension_supported(const std::string &ext) -> bool
{
    return m_GApi.extension_supported(ext);
}

auto OpenGLRenderer::stats() const -> RenderStats
{
    return m_Stats;
}

auto OpenGLRenderer::prepare_text_buffers() -> void {
    // Generate and bind VAO
    gl::GenVertexArrays(1, &m_Text.VAO);
    gl::BindVertexArray(m_Text.VAO);

    // Dynamic instance VBO
    gl::GenBuffers(1, &m_Text.VBO);

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


    


    