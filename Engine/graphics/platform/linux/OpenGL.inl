


OpenGL::~OpenGL()
{
    // glXMakeCurrent(m_Surface, Window{},  GLCTX{});
    // glXDestroyContext(m_Surface, m_Context);
}


// OpenGL::OpenGL(const OpenGL &other)
//     : m_Context(GLCTX{})
//     , m_Major(other.m_Major)
//     , m_Minor(other.m_Minor)
//     , m_CreationTime(std::time(nullptr))
// {
//     glXCopyContext(this->m_Surface, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
//     // no error check for now  ` X11 ` Shit
// }


// auto OpenGL::operator=(const OpenGL &other) -> OpenGL&
// {
//     if(this != &other){
//         this->m_Context = GLCTX{};
//         this->m_Major = other.m_Major;
//         this->m_Minor = other.m_Minor;
//         this->m_CreationTime = std::time(nullptr);

//         glXCopyContext(this->m_Surface, other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
//         // no error check for now  ` X11 ` Shit
//     }
//     return *this;
// }

auto OpenGL::make_current_opengl([[maybe_unused]] const CWindow& window)  -> bool
{
    return glXMakeCurrent(window.surface(), window.handle(), m_Context);
}

auto OpenGL::create_opengl_context([[maybe_unused]] const CWindow& window) -> GLCTX
{
    auto surface = window.surface();
    static int32_t visualAttribs[] = {
        GLX_X_RENDERABLE,  true,
        GLX_DOUBLEBUFFER,  true,
        GLX_RED_SIZE,       ChannelBits,
        GLX_GREEN_SIZE,     ChannelBits,
        GLX_BLUE_SIZE,      ChannelBits,
        GLX_ALPHA_SIZE,     AlphaBits,
        GLX_DEPTH_SIZE,     DepthBufferBits,
        0
    };
    
    int32_t fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(surface, DefaultScreen(surface), visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        throw Exception("Failed to get framebuffer config.");
    }

    XVisualInfo* visInfo = glXGetVisualFromFBConfig(surface, fbc[0]);
    if (!visInfo) {
        XFree(fbc);
        throw Exception("Failed to get visual info.");
    }

    int32_t contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, GLMajorVersion,
        GLX_CONTEXT_MINOR_VERSION_ARB, GLMinorVersion,
        #ifdef DEBUG
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB | GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        #endif
        0
    };

    glXCreateContextAttribsARB = (decltype(glXCreateContextAttribsARB))glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    auto context = glXCreateContextAttribsARB(surface, fbc[0], nullptr, True, contextAttribs);
    XFree(fbc);
    XFree(visInfo);

    if (!context) {
        throw Exception("Failed to create GLX context.");
    }

    return context;
}