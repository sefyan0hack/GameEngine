#include "OpenGL.hpp"

#include "gl.hpp"
#include "Window.hpp"

#include <GL/glcorearb.h>
#include <core/Log.hpp>
#include <core/SysInfo.hpp>
#include <core/Exception.hpp>

#include <unordered_map>

#define PACK(x, y) ((uint32_t(x) << 16) | (uint32_t(y) & 0xFFFF))

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

    check_extensions();
    init_max_members();

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
    logg::info("Multisample Anti-Aliasing : {}/{}", gl::get_intv(GL_SAMPLES), MAX_MSAA);
    logg::info("Max anisotropic: {}", MAX_ANISOTROPY);

    logg::info("===================================[GL Extention]=========================================");
    logg::info(gl::extensions());
    logg::info("===================================[Plt Extention]=========================================");
    logg::info(m_Window.platform_extensions());
    logg::info("===================================[Metrics]==========================================");

    logg::info("MAX_TEXTURE_SIZE: {}", MAX_TEXTURE_SIZE);
    logg::info("MAX_3D_TEXTURE_SIZE: {}", MAX_3D_TEXTURE_SIZE);
    logg::info("MAX_CUBE_MAP_TEXTURE_SIZE: {}", MAX_CUBE_MAP_TEXTURE_SIZE);
    logg::info("MAX_ARRAY_TEXTURE_LAYERS: {}", MAX_ARRAY_TEXTURE_LAYERS);
    logg::info("MAX_FRAGMENT_TEXTURE_UNITS: {}", MAX_FRAGMENT_TEXTURE_UNITS);
    logg::info("MAX_VERTEX_TEXTURE_UNITS: {}", MAX_VERTEX_TEXTURE_UNITS);
    logg::info("MAX_COMBINED_TEXTURE_UNITS: {}", MAX_COMBINED_TEXTURE_UNITS);
    logg::info("MAX_VERTEX_UNIFORM_COMPONENTS: {}", MAX_VERTEX_UNIFORM_COMPONENTS);
    logg::info("MAX_FRAGMENT_UNIFORM_COMPONENTS: {}", MAX_FRAGMENT_UNIFORM_COMPONENTS);
    logg::info("MAX_UNIFORM_BLOCK_SIZE: {}", MAX_UNIFORM_BLOCK_SIZE);
    logg::info("MAX_UNIFORM_BUFFER_BINDINGS: {}", MAX_UNIFORM_BUFFER_BINDINGS);
    logg::info("MAX_COMBINED_UNIFORM_BLOCKS: {}", MAX_COMBINED_UNIFORM_BLOCKS);
    logg::info("MAX_RENDERBUFFER_SIZE: {}", MAX_RENDERBUFFER_SIZE);
    logg::info("MAX_COLOR_ATTACHMENTS: {}", MAX_COLOR_ATTACHMENTS);
}

auto OpenGL::init_max_members() -> void
{
    s_MAX_FRAGMENT_TEXTURE_UNITS = gl::get_intv(GL_MAX_TEXTURE_IMAGE_UNITS);
    s_MAX_VERTEX_TEXTURE_UNITS = gl::get_intv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
    s_MAX_COMBINED_TEXTURE_UNITS = gl::get_intv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

    s_MAX_MSAA = gl::get_intv(GL_MAX_SAMPLES);

    if (
        OpenGL::is_GL_EXT_texture_filter_anisotropic ||
        OpenGL::is_GL_ARB_texture_filter_anisotropic
    ){
        #if defined(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)
        #define MAX_TEXTURE_MAX_ANISOTROPY GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
        #elif defined(GL_MAX_TEXTURE_MAX_ANISOTROPY_ARB)
        #define MAX_TEXTURE_MAX_ANISOTROPY GL_MAX_TEXTURE_MAX_ANISOTROPY_ARB
        #elif defined(GL_MAX_TEXTURE_MAX_ANISOTROPY)
        #define MAX_TEXTURE_MAX_ANISOTROPY GL_MAX_TEXTURE_MAX_ANISOTROPY
        #else
        #define MAX_TEXTURE_MAX_ANISOTROPY -1 
        #endif

        if(MAX_TEXTURE_MAX_ANISOTROPY != -1)
            s_MAX_ANISOTROPY = gl::get_floatv(MAX_TEXTURE_MAX_ANISOTROPY);
    }

    s_MAX_TEXTURE_SIZE = gl::get_intv(GL_MAX_TEXTURE_SIZE);
    s_MAX_3D_TEXTURE_SIZE = gl::get_intv(GL_MAX_3D_TEXTURE_SIZE);
    s_MAX_CUBE_MAP_TEXTURE_SIZE = gl::get_intv(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
    s_MAX_ARRAY_TEXTURE_LAYERS = gl::get_intv(GL_MAX_ARRAY_TEXTURE_LAYERS);
    s_MAX_VERTEX_UNIFORM_COMPONENTS = gl::get_intv(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
    s_MAX_FRAGMENT_UNIFORM_COMPONENTS = gl::get_intv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
    s_MAX_UNIFORM_BLOCK_SIZE = gl::get_intv(GL_MAX_UNIFORM_BLOCK_SIZE);
    s_MAX_UNIFORM_BUFFER_BINDINGS = gl::get_intv(GL_MAX_UNIFORM_BUFFER_BINDINGS);
    s_MAX_COMBINED_UNIFORM_BLOCKS = gl::get_intv(GL_MAX_COMBINED_UNIFORM_BLOCKS);
    s_MAX_RENDERBUFFER_SIZE = gl::get_intv(GL_MAX_RENDERBUFFER_SIZE);
    s_MAX_COLOR_ATTACHMENTS = gl::get_intv(GL_MAX_COLOR_ATTACHMENTS);
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

#if defined(GL_VERSION_4_3)
    static auto APIENTRY message_callback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t, const char* message, const void *) -> void
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

        auto msg = std::format("OpenGL: source : {}, type: {}, id: {}, msg: {}", m[source], m[type], id, message);
        switch(severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:
                logg::error(msg);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
            case GL_DEBUG_SEVERITY_LOW:
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                logg::warn(msg);
                break;
        }
    };
#endif

auto OpenGL::enable_debug() const -> void
{
    if constexpr (!DEBUG) return;
 
    // Enable Opengl debug
    #if defined(CORE_GL)

    if (PACK(m_Major, m_Minor) >= PACK(4,3) || OpenGL::is_GL_KHR_debug) {

        GET_GLEXT_FUNCTION_THROW(glDebugMessageCallback);
        GET_GLEXT_FUNCTION_THROW(glDebugMessageControl);
        gl::Enable(GL_DEBUG_OUTPUT);
        gl::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback_ext(message_callback, nullptr);

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

    } else if(OpenGL::is_GL_ARB_debug_output) {
        GET_GLEXT_FUNCTION_THROW(glDebugMessageCallbackARB);
        GET_GLEXT_FUNCTION_THROW(glDebugMessageControlARB);
        gl::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

        glDebugMessageCallbackARB_ext(message_callback, nullptr);

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

auto OpenGL::check_extensions() -> void
{
    auto exts = gl::extensions();

    is_GL_KHR_debug = exts.contains("GL_KHR_debug");
    is_GL_ARB_debug_output = exts.contains("GL_ARB_debug_output");
    is_GL_EXT_disjoint_timer_query = exts.contains("GL_EXT_disjoint_timer_query");
    is_GL_EXT_texture_filter_anisotropic = exts.contains("GL_EXT_texture_filter_anisotropic");
    is_GL_ARB_texture_filter_anisotropic = exts.contains("GL_ARB_texture_filter_anisotropic");
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
    if constexpr (!OpenGL::DEBUG) return;

    #ifdef CORE_GL
    if (PACK(OpenGL::MIN_REQUIRED_MAJOR_VERSION, OpenGL::MIN_REQUIRED_MINOR_VERSION) >= PACK(4,3) || OpenGL::is_GL_KHR_debug) {
        static uint32_t id{};
        GET_GLEXT_FUNCTION_THROW(glPushDebugGroup);
        glPushDebugGroup_ext(GL_DEBUG_SOURCE_APPLICATION, id++, -1, name);
    }
    #endif
}

auto gl::pop_debug_group() -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifdef CORE_GL
    if (PACK(OpenGL::MIN_REQUIRED_MAJOR_VERSION, OpenGL::MIN_REQUIRED_MINOR_VERSION) >= PACK(4,3) || OpenGL::is_GL_KHR_debug) {
        GET_GLEXT_FUNCTION_THROW(glPopDebugGroup);
        glPopDebugGroup_ext();
    }
    #endif
}

static auto ObjectLabel(GLenum identifier, GLuint name, GLsizei length, const GLchar *label) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifdef CORE_GL
    if (PACK(OpenGL::MIN_REQUIRED_MAJOR_VERSION, OpenGL::MIN_REQUIRED_MINOR_VERSION) >= PACK(4,3) || OpenGL::is_GL_KHR_debug) {
        GET_GLEXT_FUNCTION_THROW(glObjectLabel);
        glObjectLabel_ext(identifier, name, length, label);
    }
    #endif
}

auto gl::label_texture(uint32_t id, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_TEXTURE
    #define GL_TEXTURE 0
    #endif

    static size_t c{1};

    std::string full_name = std::format("Texture[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_TEXTURE, id, -1, full_name.c_str());
}

auto gl::label_vertex_array(uint32_t id, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_VERTEX_ARRAY
    #define GL_VERTEX_ARRAY 0
    #endif

    static size_t c{1};

    std::string full_name = std::format("VAO[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_VERTEX_ARRAY, id, -1, full_name.c_str());
}

auto gl::label_array_buffer(uint32_t id, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_BUFFER
    #define GL_BUFFER 0
    #endif

    static size_t c{1};

    std::string full_name = std::format("VBO[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_BUFFER, id, -1, full_name.c_str());
}

auto gl::label_index_buffer(uint32_t id, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_BUFFER
    #define GL_BUFFER 0
    #endif

    static size_t c{1};

    std::string full_name = std::format("IBO[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_BUFFER, id, -1, full_name.c_str());
}

auto gl::label_uniform_buffer(uint32_t id, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_BUFFER
    #define GL_BUFFER 0
    #endif

    static size_t c{1};

    std::string full_name = std::format("UBO[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_BUFFER, id, -1, full_name.c_str());
}

auto gl::label_shader(uint32_t id, GLenum type, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_SHADER
    #define GL_SHADER 0
    #endif

    std::string full_name;
    switch(type){
        case GL_VERTEX_SHADER: full_name = "Vertex "; break;
        case GL_FRAGMENT_SHADER: full_name = "Fragment "; break;
    }

    static size_t c{1};

    full_name += std::format("Shader[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_SHADER, id, -1, full_name.c_str());
}

auto gl::label_program(uint32_t id, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_PROGRAM
    #define GL_PROGRAM 0
    #endif

    static size_t c{1};

    std::string full_name = std::format("Program[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_PROGRAM, id, -1, full_name.c_str());
}

auto gl::label_querie(uint32_t id, const char* name) -> void
{
    if constexpr (!OpenGL::DEBUG) return;

    #ifndef GL_QUERY
    #define GL_QUERY 0
    #endif

    static size_t c{1};

    std::string full_name = std::format("Querie[{}]: ", c++);
    if (name) full_name += name;

    ObjectLabel(GL_QUERY, id, -1, full_name.c_str());
}

auto gl::get_boolv (GLenum pname) -> bool
{
    uint8_t r{};
    gl::GetBooleanv(pname, &r);
    return r;
}

auto gl::get_floatv (GLenum pname) -> float
{
    float r{};
    gl::GetFloatv(pname, &r);
    return r;
}

auto gl::get_intv (GLenum pname) -> int32_t
{
    int32_t r{};
    gl::GetIntegerv(pname, &r);
    return r;
}

auto gl::get_query_object_i64(GLuint id, GLenum pname, GLint64* param) -> void
{
    #ifdef CORE_GL
        GET_GLEXT_FUNCTION_THROW(glGetQueryObjecti64v);
        glGetQueryObjecti64v_ext(id, pname, param);
    #else
        if(OpenGL::is_GL_EXT_disjoint_timer_query){
            GET_GLEXT_FUNCTION_THROW(glGetQueryObjecti64vEXT);
            glGetQueryObjecti64vEXT_ext(id, pname, param);
        }
    #endif
}

auto gl::get_query_object_ui64(GLuint id, GLenum pname, GLuint64* param) -> void
{
    #ifdef CORE_GL
        GET_GLEXT_FUNCTION_THROW(glGetQueryObjectui64v);
        glGetQueryObjectui64v_ext(id, pname, param);
    #else
        if(OpenGL::is_GL_EXT_disjoint_timer_query){
            GET_GLEXT_FUNCTION_THROW(glGetQueryObjectui64vEXT);
            glGetQueryObjectui64vEXT_ext(id, pname, param);
        }
    #endif
}

#if defined(GL_TIME_ELAPSED)
#define TIME_ELAPSED GL_TIME_ELAPSED
#elif defined(GL_TIME_ELAPSED_EXT)
#define TIME_ELAPSED GL_TIME_ELAPSED_EXT
#else
#define TIME_ELAPSED 0
#endif

auto gl::get_query_if_available(GLuint id, GLuint64* ns) -> void
{
    GLuint available{};
    gl::GetQueryObjectuiv(id, GL_QUERY_RESULT_AVAILABLE, &available);

    if (available) {
        gl::get_query_object_ui64(id, GL_QUERY_RESULT, ns);
    }
}

auto gl::begin_query_time_elapsed(GLuint id) -> void
{
    gl::BeginQuery(TIME_ELAPSED, id);
}

auto gl::end_query_time_elapsed() -> void
{
    gl::EndQuery(TIME_ELAPSED);
}

auto gl::query_timestamp(GLuint id) -> void
{
    #if defined(CORE_GL)
    GET_GLEXT_FUNCTION_THROW(glQueryCounter);
    glQueryCounter_ext(id, GL_TIMESTAMP);
    #else
    if(OpenGL::is_GL_EXT_disjoint_timer_query){
        GET_GLEXT_FUNCTION_THROW(glQueryCounterEXT);
        glQueryCounterEXT_ext(id, GL_TIMESTAMP_EXT);
    }
    #endif
}

auto gl::texture_param_anisotropic(GLenum target) -> void
{
    #if defined(GL_TEXTURE_MAX_ANISOTROPY_EXT)
    #define TEXTURE_MAX_ANISOTROPY GL_TEXTURE_MAX_ANISOTROPY_EXT
    #elif defined(GL_TEXTURE_MAX_ANISOTROPY_ARB)
    #define TEXTURE_MAX_ANISOTROPY GL_TEXTURE_MAX_ANISOTROPY_ARB
    #elif defined(GL_TEXTURE_MAX_ANISOTROPY)
    #define TEXTURE_MAX_ANISOTROPY GL_TEXTURE_MAX_ANISOTROPY
    #else
    #define TEXTURE_MAX_ANISOTROPY -1 
    #endif

    if (
        OpenGL::is_GL_EXT_texture_filter_anisotropic ||
        OpenGL::is_GL_ARB_texture_filter_anisotropic
    ){
        gl::TexParameterf(target, TEXTURE_MAX_ANISOTROPY, OpenGL::ANISOTROPY);
    }
}

auto gl::create_vertex_array(const char* name) -> uint32_t
{
    uint32_t r{};
    gl::GenVertexArrays(1, &r);

    gl::label_vertex_array(r, name);
    return r;
}

auto gl::create_array_buffer(const char* name) -> uint32_t
{
    uint32_t r{};
    gl::GenBuffers(1, &r);
    
    gl::label_array_buffer(r, name);
    return r;
}

auto gl::create_index_buffer(const char* name) -> uint32_t
{
    uint32_t r{};
    gl::GenBuffers(1, &r);
    
    gl::label_index_buffer(r, name);
    return r;
}

auto gl::create_uniform_buffer(const char* name) -> uint32_t
{
    uint32_t r{};
    gl::GenBuffers(1, &r);
    
    gl::label_uniform_buffer(r, name);
    return r;
}

auto gl::create_texture(const char* name) -> uint32_t
{
    uint32_t r{};
    gl::GenTextures(1, &r);
    
    gl::label_texture(r, name);
    return r;
}

auto gl::create_shader(GLenum type, const char* name) -> uint32_t
{
    uint32_t r = gl::CreateShader(type);

    gl::label_shader(r, type, name);
    return r;
}

auto gl::create_program(const char* name) -> uint32_t
{
    uint32_t r = gl::CreateProgram();

    gl::label_program(r, name);
    return r;
}


auto gl::create_querie(const char* name) -> uint32_t
{
    uint32_t r{};
    gl::GenQueries(1, &r);

    gl::label_querie(r, name);
    return r;
}
