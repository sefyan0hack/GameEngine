


OpenGL::~OpenGL()
{
    if (m_Context > 0) {
        emscripten_webgl_destroy_context(m_Context);
    }
}


OpenGL::OpenGL(const OpenGL &other)
    : m_Context(GLCTX{})
    , m_Surface(other.m_Surface)
    , m_Major(other.m_Major)
    , m_Minor(other.m_Minor)
    , m_CreationTime(std::time(nullptr))
{
}


auto OpenGL::operator=(const OpenGL &other) -> OpenGL&
{
    if(this != &other){
        this->m_Context = GLCTX{};
        this->m_Surface = other.m_Surface;    
        this->m_Major = other.m_Major;
        this->m_Minor = other.m_Minor;
        this->m_CreationTime = std::time(nullptr);

    }
    return *this;
}

auto OpenGL::make_current_opengl(GLCTX ctx, [[maybe_unused]] H_SRF srf, [[maybe_unused]] H_WIN win)  -> bool {
    return emscripten_webgl_make_context_current(ctx) == EMSCRIPTEN_RESULT_SUCCESS;
}

auto OpenGL::init_opengl() -> void
{

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.alpha = EM_TRUE;
    attrs.depth = EM_TRUE;
    attrs.stencil = EM_TRUE;
    attrs.antialias = EM_TRUE;
    attrs.premultipliedAlpha = EM_FALSE;
    attrs.preserveDrawingBuffer = EM_FALSE;
    attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
    attrs.failIfMajorPerformanceCaveat = EM_FALSE;
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    attrs.enableExtensionsByDefault = EM_TRUE;

    m_Context = emscripten_webgl_create_context(m_Surface, &attrs);

    if (m_Context <= 0) {
        throw Exception("Failed to create WebGL context: error {}", static_cast<int32_t>(m_Context));
    }
}