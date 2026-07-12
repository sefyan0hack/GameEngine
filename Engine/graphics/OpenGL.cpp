#include "OpenGL.hpp"

#include "gl.hpp"
#include "Window.hpp"

#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>

#include <unordered_map>

#define PACK(x, y) ((uint32_t(x) << 16) | (uint32_t(y) & 0xFFFF))

auto get_info(uint32_t name) -> int32_t
{
    int32_t r = 0;
    gl::GetIntegerv(name, &r);
    return r;
}

#define gl_info(name) logg::info(#name" : {}", get_info(name))

OpenGL::OpenGL([[maybe_unused]] const CWindow& window)
    : m_Window(window)
    , m_Config(find_config(m_Window))
    , m_Context(create_context())
    , m_Major(0)
    , m_Minor(0)
{
    if (make_current()) load_functions();
    else throw Exception("Failed to make context current.");

    auto [w, h] = window.dims();

    gl::Viewport(0, 0, w, h);

    gl::GetIntegerv(GL_MAJOR_VERSION, &m_Major);
    gl::GetIntegerv(GL_MINOR_VERSION, &m_Minor);

    if (PACK(m_Major, m_Minor) < PACK(MIN_REQUIRED_MAJOR_VERSION, MIN_REQUIRED_MINOR_VERSION))
    {
        throw Exception(
            "Min required OpenGL version is {}.{} but got {}.{}",
            MIN_REQUIRED_MAJOR_VERSION,
            MIN_REQUIRED_MINOR_VERSION,
            m_Major,
            m_Minor
        );
    }

    auto vendor = reinterpret_cast<const char*>(gl::GetString(GL_VENDOR));
    auto renderer = reinterpret_cast<const char*>(gl::GetString(GL_RENDERER));
    auto version = reinterpret_cast<const char*>(gl::GetString(GL_VERSION));
    auto glsl_version = reinterpret_cast<const char*>(gl::GetString(GL_SHADING_LANGUAGE_VERSION));

    auto Vendor = vendor ? vendor : "unknown";
    auto Renderer = renderer ? renderer : "unknown";
    auto Version = version ? version : "unknown";
    auto GlslVersion = glsl_version ? glsl_version : "unknown";

    if constexpr (DEBUG) enable_debug();

    logg::info(os::build_info());
    logg::info("===================================[GL Info]=========================================");
    logg::info("Platform Name: {}", os::name());
    logg::info("Platform Arch: {} ({}) bit", os::arch(), os::bits());
    logg::info("GL Version : {}", Version);
    logg::info("GL Shading Version : {}", GlslVersion);
    logg::info("GL Vendor : {}", Vendor);
    logg::info("GL Renderer : {}", Renderer);
    logg::info("GL Debug : {}", DEBUG ? "true" : "false");
    logg::info("===================================[GL Extention]=========================================");
    logg::info(gl::extensions());
    logg::info("===================================[Metrics]==========================================");
    
    gl_info(GL_MAX_TEXTURE_SIZE);
    gl_info(GL_MAX_3D_TEXTURE_SIZE);
    gl_info(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
    gl_info(GL_MAX_ARRAY_TEXTURE_LAYERS);
    gl_info(GL_MAX_TEXTURE_IMAGE_UNITS);
    gl_info(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
    gl_info(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
    gl_info(GL_MAX_UNIFORM_BLOCK_SIZE);
    gl_info(GL_MAX_UNIFORM_BUFFER_BINDINGS);
    gl_info(GL_MAX_COMBINED_UNIFORM_BLOCKS);
    gl_info(GL_MAX_RENDERBUFFER_SIZE);
    gl_info(GL_MAX_COLOR_ATTACHMENTS);
    gl_info(GL_MAX_SAMPLES);
    gl_info(GL_SAMPLES);
}

auto OpenGL::window() const -> const CWindow&
{
    return m_Window;
}

auto OpenGL::context() const -> GL_CTX
{
    return m_Context;
}

auto OpenGL::config() const -> GL_CFG
{
    return m_Config;
}

auto OpenGL::version() const -> std::pair<int32_t, int32_t>
{
 return {m_Major, m_Minor};
}

auto OpenGL::is_current() const -> bool
{
    return gl::GetCurrentContext() == m_Context;
}


auto OpenGL::enable_debug() const -> void
{
    // Enable Opengl debug
    #if defined(CORE_GL)

    auto messgae_callback_func = +[](uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t, const char* message, const void *) -> void
    {
        std::unordered_map<uint32_t, const char*> m {
            {GL_DEBUG_SOURCE_API, "GL_DEBUG_SOURCE_API"},
            {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "GL_DEBUG_SOURCE_WINDOW_SYSTEM"},
            {GL_DEBUG_SOURCE_SHADER_COMPILER, "GL_DEBUG_SOURCE_SHADER_COMPILER"},
            {GL_DEBUG_SOURCE_THIRD_PARTY, "GL_DEBUG_SOURCE_THIRD_PARTY"},
            {GL_DEBUG_SOURCE_APPLICATION, "GL_DEBUG_SOURCE_APPLICATION"},
            {GL_DEBUG_SOURCE_OTHER, "GL_DEBUG_SOURCE_OTHER"},

            {GL_DEBUG_TYPE_ERROR, "GL_DEBUG_TYPE_ERROR"},
            {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"},
            {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"},
            {GL_DEBUG_TYPE_PORTABILITY, "GL_DEBUG_TYPE_PORTABILITY"},
            {GL_DEBUG_TYPE_PERFORMANCE, "GL_DEBUG_TYPE_PERFORMANCE"},
            {GL_DEBUG_TYPE_OTHER, "GL_DEBUG_TYPE_OTHER"},
            
            #if defined(GL_DEBUG_TYPE_MARKER) && defined(GL_DEBUG_TYPE_PUSH_GROUP) && defined(GL_DEBUG_TYPE_POP_GROUP)
            {GL_DEBUG_TYPE_MARKER, "GL_DEBUG_TYPE_MARKER"},
            {GL_DEBUG_TYPE_PUSH_GROUP, "GL_DEBUG_TYPE_PUSH_GROUP"},
            {GL_DEBUG_TYPE_POP_GROUP, "GL_DEBUG_TYPE_POP_GROUP"},
            #endif

            {GL_DEBUG_SEVERITY_HIGH, "GL_DEBUG_SEVERITY_HIGH"},
            {GL_DEBUG_SEVERITY_MEDIUM, "GL_DEBUG_SEVERITY_MEDIUM"},
            {GL_DEBUG_SEVERITY_LOW, "GL_DEBUG_SEVERITY_LOW"},
            {GL_DEBUG_SEVERITY_NOTIFICATION, "GL_DEBUG_SEVERITY_NOTIFICATION"}
        };
        logg::error("source : {}, type: {}, id: {}, severity: {}, msg: {}.", m[source], m[type], id, m[severity], message);
    };

    if (PACK(m_Major, m_Minor) >= PACK(4,3) || gl::extensions().contains("GL_KHR_debug")) {

        GET_GLEXT_FUNCTION_THROW(glDebugMessageCallback);
        GET_GLEXT_FUNCTION_THROW(glDebugMessageControl);
        gl::Enable(GL_DEBUG_OUTPUT);
        gl::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback_ext(messgae_callback_func, nullptr);

        #if defined(GL_DEBUG_TYPE_PUSH_GROUP) && defined(GL_DEBUG_TYPE_POP_GROUP)
        glDebugMessageControl_ext(
            GL_DEBUG_SOURCE_APPLICATION,
            GL_DEBUG_TYPE_PUSH_GROUP,
            GL_DONT_CARE,
            0,
            nullptr,
            GL_FALSE
        );

        glDebugMessageControl_ext(
            GL_DEBUG_SOURCE_APPLICATION,
            GL_DEBUG_TYPE_POP_GROUP,
            GL_DONT_CARE,
            0,
            nullptr,
            GL_FALSE
        );
        #endif

    } else if(gl::extensions().contains("GL_ARB_debug_output")) {
        GET_GLEXT_FUNCTION_THROW(glDebugMessageCallbackARB);
        GET_GLEXT_FUNCTION_THROW(glDebugMessageControlARB);
        gl::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

        glDebugMessageCallbackARB_ext(messgae_callback_func, nullptr);

        #if defined(GL_DEBUG_TYPE_PUSH_GROUP) && defined(GL_DEBUG_TYPE_POP_GROUP)
        glDebugMessageControlARB_ext(
            GL_DEBUG_SOURCE_APPLICATION,
            GL_DEBUG_TYPE_PUSH_GROUP,
            GL_DONT_CARE,
            0,
            nullptr,
            GL_FALSE
        );

        glDebugMessageControlARB_ext(
            GL_DEBUG_SOURCE_APPLICATION,
            GL_DEBUG_TYPE_POP_GROUP,
            GL_DONT_CARE,
            0,
            nullptr,
            GL_FALSE
        );
        #endif
    }
    #endif
}

auto OpenGL::resolve_function(const char* name) -> void* {
    void *address = gl::GetProcAddress<void*>(name);

    if(address == nullptr){
        address = os::get_proc_address(EG_OPENGL_MODULE_NAME, name);
    }

    if (address == nullptr) {
        throw Exception("Couldn't load {} function `{}`", EG_OPENGL_MODULE_NAME, name);
    }

    return address;
}

auto OpenGL::load_functions() -> void
{
    #define FUNC_GL_X(name) gl::name = reinterpret_cast<decltype(&gl##name)>(resolve_function("gl"#name));
    FUNCTIONS_GL_LIST
    #undef FUNC_GL_X
}
///////////////////////////////////////////////////////////////////////////////////////////

auto gl::extensions() -> std::string
{
    static std::string extensions = []()
    {
        int32_t count{};
        gl::GetIntegerv(GL_NUM_EXTENSIONS, &count);
        std::string exts;

        for (int32_t i = 0; i < count; ++i) {
            exts += std::format("{} ", (const char*)gl::GetStringi(GL_EXTENSIONS, i));
        }

        return exts;
    }();
    return extensions;
}

auto gl::push_debug_group(const char* name) -> void
{
    #ifdef CORE_GL
    if (PACK(OpenGL::MIN_REQUIRED_MAJOR_VERSION, OpenGL::MIN_REQUIRED_MINOR_VERSION) >= PACK(4,3) || gl::extensions().contains("GL_KHR_debug")) {
        static uint32_t id{};
        GET_GLEXT_FUNCTION_THROW(glPushDebugGroup);
        glPushDebugGroup_ext(GL_DEBUG_SOURCE_APPLICATION, id++, -1, name);
    }
    #endif
}

auto gl::pop_debug_group() -> void
{
    #ifdef CORE_GL
    if (PACK(OpenGL::MIN_REQUIRED_MAJOR_VERSION, OpenGL::MIN_REQUIRED_MINOR_VERSION) >= PACK(4,3) || gl::extensions().contains("GL_KHR_debug")) {
        GET_GLEXT_FUNCTION_THROW(glPopDebugGroup);
        glPopDebugGroup_ext();
    }
    #endif
}

static auto ObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar *label) -> void
{
    #ifdef CORE_GL
    if (PACK(OpenGL::MIN_REQUIRED_MAJOR_VERSION, OpenGL::MIN_REQUIRED_MINOR_VERSION) >= PACK(4,3) || gl::extensions().contains("GL_KHR_debug")) {
        GET_GLEXT_FUNCTION_THROW(glObjectLabel);
        glObjectLabel_ext(identifier, name, length, label);
    }
    #endif
}

auto gl::label_texture(uint32_t id, const char* name) -> void
{
    #ifndef GL_TEXTURE
    #define GL_TEXTURE 0
    #endif

    ObjectLabel(GL_TEXTURE, id, -1, name);
}

auto gl::label_vertex_array(uint32_t id, const char* name) -> void
{
    #ifndef GL_VERTEX_ARRAY
    #define GL_VERTEX_ARRAY 0
    #endif

    ObjectLabel(GL_VERTEX_ARRAY, id, -1, name);
}

auto gl::label_buffer(uint32_t id, const char* name) -> void
{
    #ifndef GL_BUFFER
    #define GL_BUFFER 0
    #endif

    ObjectLabel(GL_BUFFER, id, -1, name);
}

auto gl::label_shader(uint32_t id, const char* name) -> void
{
    #ifndef GL_SHADER
    #define GL_SHADER 0
    #endif

    ObjectLabel(GL_SHADER, id, -1, name);
}

auto gl::label_program(uint32_t id, const char* name) -> void
{
    #ifndef GL_PROGRAM
    #define GL_PROGRAM 0
    #endif

    ObjectLabel(GL_PROGRAM, id, -1, name);
}