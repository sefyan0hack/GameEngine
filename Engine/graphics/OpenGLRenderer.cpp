#include "OpenGLRenderer.hpp"

#include <algorithm>
#include <execution>
#include <core/Log.hpp>
#include <core/ResourceManager.hpp>
#include "Window.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "SkyBox.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Texture.hpp"


OpenGLRenderer::OpenGLRenderer(const CWindow& window)
    : m_Window(window)
    , m_GApi(window)
    , x(0), y(0)
    , width(window.width()), height(window.height())
{}



auto OpenGLRenderer::render(const Scene& scene, const std::shared_ptr<ShaderProgram> program) const -> void
{
    scene.render_sky();
    auto camera = scene.main_camera();

    program->use();
    program->set_uniform("View", camera.view());
    program->set_uniform("Projection", camera.projection());
    program->set_uniform("Eye", camera.position());

    static std::shared_ptr<Material> currMatt = nullptr;
    //Drwaing
    std::for_each( std::execution::seq,
        scene.entities().cbegin(), scene.entities().cend(),
        [&](const auto& obj){
            program->set_uniform("Model", obj.model());

            if(currMatt != obj.material()){
                currMatt = obj.material();
                obj.material()->bind(program);
            }

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
    this->x= x;
    this->y= y;
    this->width= width;
    this->height= height;
    gl::Viewport(x, y, width, height);
}

auto OpenGLRenderer::viewport() const -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
{
    return {x, y, width, height};
}

auto OpenGLRenderer::enable_wireframe() -> void
{
    #if defined(WEB_PLT)
    static bool webPolyModeAvailable = app->m_Renderer.has_extension("WEBGL_polygon_mode");
    if (webPolyModeAvailable) {
        EM_ASM({
            const gl = Module.ctx;
            const ext = gl.getExtension("WEBGL_polygon_mode");
            ext.polygonModeWEBGL(gl.FRONT_AND_BACK, ext.LINE_WEBGL);
        });
    }
    #else
    gl::Enable(GL_LINE_SMOOTH);
    gl::Enable(GL_POLYGON_OFFSET_LINE);
    gl::PolygonOffset(-1.0f, -1.0f);
    gl::PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    #endif
}

auto OpenGLRenderer::disable_wireframe() -> void
{
    #if defined(WEB_PLT)
    static bool webPolyModeAvailable = app->m_Renderer.has_extension("WEBGL_polygon_mode");
    if (webPolyModeAvailable) {
        EM_ASM({
            const gl = Module.ctx;
            const ext = gl.getExtension("WEBGL_polygon_mode");
            ext.polygonModeWEBGL(gl.FRONT_AND_BACK, ext.FILL_WEBGL);
        });
    }
    #else
    gl::Disable(GL_POLYGON_OFFSET_LINE);
    gl::Disable(GL_LINE_SMOOTH);
    gl::PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif
}


auto OpenGLRenderer::enable_points() -> void
{
    #if !defined(WEB_PLT)
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
    #if !defined(WEB_PLT)
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