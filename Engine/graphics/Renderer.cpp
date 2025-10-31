#include <core/Log.hpp>
#include <core/ResourceManager.hpp>
#include <core/Window.hpp>
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


Renderer::Renderer(const CWindow& window, const Camera& camera)
    : m_Window(window)
    , m_OpenGl(m_Window.handle(), m_Window.surface())
    , m_Camera(camera)

{}
Renderer::~Renderer(){
}

auto Renderer::render(const Scene& scene, const std::shared_ptr<ShaderProgram> program) -> void
{
    scene.render_sky();

    program->use();
    program->set_uniform("View", m_Camera.view());
    program->set_uniform("Projection", m_Camera.projection());
    program->set_uniform("Eye", m_Camera.position());

    static std::shared_ptr<Material> currMatt = nullptr;
    //Drwaing
    for(auto &obj: scene.entities()){
        program->set_uniform("Model", obj.model());

        if(currMatt != obj.material()){
            currMatt = obj.material();
            obj.material()->bind(program);
        }

        auto mesh = obj.mesh();
        draw(*mesh);
    }
}


auto Renderer::draw(const Mesh& mesh) -> void
{
    static GLuint currVAO = mesh.VAO;
    if(currVAO != mesh.VAO){
        currVAO = mesh.VAO;
        gl::BindVertexArray(currVAO);
    }
    gl::DrawArrays(GL_TRIANGLES, 0, mesh.vextex_size());
}

auto Renderer::opengl() const -> const gl::OpenGL&
{
	return m_OpenGl;
}

auto Renderer::set_viewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) -> void
{
    gl::Viewport(x, y, width, height);
}


auto Renderer::enable_wireframe() -> void
{
    #if defined(WEB_PLT)
    static bool webPolyModeAvailable = app->m_Renderer.opengl().has_extension("WEBGL_polygon_mode");
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

auto Renderer::disable_wireframe() -> void
{
    #if defined(WEB_PLT)
    static bool webPolyModeAvailable = app->m_Renderer.opengl().has_extension("WEBGL_polygon_mode");
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


auto Renderer::enable_points() -> void
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

auto Renderer::disable_points() -> void
{
    #if !defined(WEB_PLT)
    gl::Disable(GL_PROGRAM_POINT_SIZE);
    gl::PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif
}

auto Renderer::clear_screen(GLenum buffersmask)  -> void
{
    gl::Clear(buffersmask);
}