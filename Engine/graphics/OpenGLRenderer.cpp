#include "OpenGLRenderer.hpp"

#include <algorithm>
#include <span>
#include "Window.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "SkyBox.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"

extern cmrc::embedded_filesystem embed_filesystem;


OpenGLRenderer::OpenGLRenderer(const OpenGL& ctx)
    : m_GApi(ctx)
    , m_X(0), m_Y(0)
    , m_Width(ctx.window().dims().first), m_Height(ctx.window().dims().second)
    , m_Vert(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/main.vert"), GL_VERTEX_SHADER))
    , m_Frag(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/main.frag"), GL_FRAGMENT_SHADER))
    , m_Program(std::make_shared<ShaderProgram>(m_Vert, m_Frag))
{}

auto OpenGLRenderer::render(const Scene& scene) const -> void
{
    auto camera = scene.main_camera();
    Material::render_sky(camera);

    m_Program->use();
    m_Program->set_uniform("View", camera.view());
    m_Program->set_uniform("Projection", camera.projection());
    m_Program->set_uniform("Eye", camera.position());

    //Drwaing
    std::for_each(
        scene.entities().begin(), scene.entities().end(),
        [&](const auto& obj){
            m_Program->set_uniform("Model", obj.model());

            obj.material()->bind(m_Program);

            auto mesh = obj.mesh();
            draw(*mesh);
        }
    );

}


auto OpenGLRenderer::draw(const Mesh& mesh) const -> void
{
    static GLuint currVAO = mesh.VAO;
    if(currVAO != mesh.VAO){
        currVAO = mesh.VAO;
        gl::BindVertexArray(currVAO);
    }
    gl::DrawArrays(GL_TRIANGLES, 0, mesh.vextex_size());
}

auto OpenGLRenderer::graphic_api() const -> std::string_view
{
	return ::type_name<std::remove_const_t<decltype(m_GApi)>>();
}

auto OpenGLRenderer::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void
{
    std::tie(m_X, m_Y, m_Width, m_Height) = {x, y, width, height};
    gl::Viewport(m_X, m_Y, m_Width, m_Height);
}

auto OpenGLRenderer::viewport() const -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
{
    return {m_X, m_Y, m_Width, m_Height};
}

auto OpenGLRenderer::enable_wireframe() -> void
{
    #if defined(WEB_PLT)
    static bool webPolyModeAvailable = has_extension("WEBGL_polygon_mode");
    if (webPolyModeAvailable) {
        EM_ASM({
            const gl = Module.ctx;
            const ext = gl.getExtension("WEBGL_polygon_mode");
            ext.polygonModeWEBGL(gl.FRONT_AND_BACK, ext.LINE_WEBGL);
        });
    }
    #elif defined(CORE_GL)
    gl::Enable(GL_LINE_SMOOTH);
    gl::Enable(GL_POLYGON_OFFSET_LINE);
    gl::PolygonOffset(-1.0f, -1.0f);
    gl::PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    #endif
}

auto OpenGLRenderer::disable_wireframe() -> void
{
    #if defined(WEB_PLT)
    static bool webPolyModeAvailable = has_extension("WEBGL_polygon_mode");
    if (webPolyModeAvailable) {
        EM_ASM({
            const gl = Module.ctx;
            const ext = gl.getExtension("WEBGL_polygon_mode");
            ext.polygonModeWEBGL(gl.FRONT_AND_BACK, ext.FILL_WEBGL);
        });
    }
    #endif

    #if defined(CORE_GL)
    gl::Disable(GL_POLYGON_OFFSET_LINE);
    gl::Disable(GL_LINE_SMOOTH);
    gl::PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif
}


auto OpenGLRenderer::enable_points() -> void
{
    #if defined(CORE_GL)
    gl::Enable(GL_PROGRAM_POINT_SIZE);
    GLfloat widths[2];
    auto& min = widths[0];
    auto& max = widths[1];
    gl::GetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, widths);
    gl::PointSize((max-min)/2);

    gl::PolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    #endif
}

auto OpenGLRenderer::disable_points() -> void
{
    #if defined(CORE_GL)
    gl::Disable(GL_PROGRAM_POINT_SIZE);
    gl::PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif
}

auto OpenGLRenderer::clear_screen(uint32_t buffersmask)  -> void
{
    gl::Clear((GLenum)buffersmask);
}

auto OpenGLRenderer::has_extension(const std::string &ext) -> bool
{
    return m_GApi.has_extension(ext);
}