


OpenGL::~OpenGL()
{
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_Context);
}


// OpenGL::OpenGL(const OpenGL &other)
//     : m_Context(GLCTX{})
//     , m_Major(other.m_Major)
//     , m_Minor(other.m_Minor)
//     , m_CreationTime(std::time(nullptr))
// {
//     auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
//     if(tst != TRUE) throw Exception("couldn't Copy Opengl Context");
// }


// auto OpenGL::operator=(const OpenGL &other) -> OpenGL&
// {
//     if(this != &other){
//         this->m_Context = GLCTX{};
//         this->m_Major = other.m_Major;
//         this->m_Minor = other.m_Minor;
//         this->m_CreationTime = std::time(nullptr);

//         auto tst = wglCopyContext(other.m_Context, this->m_Context, GL_ALL_ATTRIB_BITS);
//         if(tst != TRUE) throw Exception("couldn't Copy Opengl Context");
//     }
//     return *this;
// }

auto OpenGL::make_current_opengl([[maybe_unused]] const CWindow& window)  -> bool
{
    return wglMakeCurrent(window.surface(), m_Context);
}

auto OpenGL::create_opengl_context(const CWindow& window) -> GLCTX
{
    auto surface = window.surface();

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        static_cast<BYTE>(ChannelBits * 3),  // RGB bits
        static_cast<BYTE>(AlphaBits),         // Alpha bits
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        static_cast<BYTE>(DepthBufferBits),   // Depth buffer
        static_cast<BYTE>(StencilBufferBits), // Stencil buffer
        0,
        PFD_MAIN_PLANE,
        0, 0, 0
    };
    auto pixel_format = ChoosePixelFormat(surface, &pfd);
    if (!pixel_format) {
        throw Exception("Failed to find a suitable pixel format. : {}", GetLastError());
    }
    if (!SetPixelFormat(surface, pixel_format, &pfd)) {
        throw Exception("Failed to set the pixel format. : {}", GetLastError());
    }

    GLCTX dummy_context = nullptr;

    dummy_context = wglCreateContext(surface);
    if (!dummy_context) {
        throw Exception("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    if (!wglMakeCurrent(surface, dummy_context)) {
        throw Exception("Failed to activate dummy OpenGL rendering context. : {}", GetLastError());
    }

    wglGetExtensionsStringARB  = reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB"));
    wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

    if (!wglGetExtensionsStringARB){
        throw Exception("Failed to load wglGetExtensionsStringARB. : {}", GetLastError());
    }

    if(!wglCreateContextAttribsARB) {
        throw Exception("Failed to load wglCreateContextAttribsARB. : {}", GetLastError());
    }

    int32_t gl_attribs[] = { 
        WGL_CONTEXT_MAJOR_VERSION_ARB, GLMajorVersion,
        WGL_CONTEXT_MINOR_VERSION_ARB, GLMinorVersion,
    #ifdef DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    #endif
        0,
    };


    GLCTX opengl_context = nullptr;
    if (nullptr == (opengl_context = wglCreateContextAttribsARB(surface, nullptr, gl_attribs))) {
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            throw Exception("Unsupported GL Version {}.{}", GLMajorVersion, GLMinorVersion);
        }
        throw Exception("Failed to create the final rendering context!");
    }

    wglDeleteContext(dummy_context);
 
    return opengl_context;
}