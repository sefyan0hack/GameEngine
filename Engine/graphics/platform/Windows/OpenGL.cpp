#include "OpenGL.hpp"
#include "Window.hpp"
#include "gl.hpp"

#include <core/Log.hpp>
#include <core/Exception.hpp>

#include <vector>

#undef min
#undef max

extern const TCHAR* window_class_name();

OpenGL::~OpenGL()
{
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_Context);
}

auto OpenGL::make_current()  -> bool
{
    GET_GLEXT_FUNCTION_NO_THROW(wglMakeContextCurrentARB);
    if(wglMakeContextCurrentARB_ext){
        return wglMakeContextCurrentARB_ext(m_Window.surface(), m_Window.surface(), m_Context);
    } else {
        return wglMakeCurrent(m_Window.surface(), m_Context);
    }
}

auto OpenGL::find_config([[maybe_unused]] const CWindow& window) -> GL_CFG
{
    return 0;
}

auto OpenGL::create_context() -> GL_CTX
{
    //TODO: destroy dummy window + cleanups
    auto dummy_window = CreateWindow(
        window_class_name(),
        "dummy",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr,
        GetModuleHandleA( nullptr ),
        nullptr
    );

    auto dummy_surface = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        static_cast<BYTE>(8 * 3),  // RGB bits
        static_cast<BYTE>(8),         // Alpha bits
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        static_cast<BYTE>(24),// Depth buffer
        static_cast<BYTE>(8), // Stencil buffer
        0,
        PFD_MAIN_PLANE,
        0, 0, 0
    };

    auto pixel_format = ChoosePixelFormat(dummy_surface, &pfd);

    if (!pixel_format)
        throw Exception("Failed to find a suitable pixel format. : {}", GetLastError());

    if(!DescribePixelFormat(dummy_surface, pixel_format, sizeof(pfd), &pfd))
        throw Exception("Failed Describe pixel format. : {}", GetLastError());

    if (!SetPixelFormat(dummy_surface, pixel_format, &pfd))
        throw Exception("Failed to set the pixel format. : {}", GetLastError());

    GL_CTX dummy_context = nullptr;

    dummy_context = wglCreateContext(dummy_surface);
    if (!dummy_context) {
        throw Exception("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    wglMakeCurrent(dummy_surface, dummy_context);

    std::string extensions;
    {
        auto glGetIntegerv_ext = reinterpret_cast<decltype(&glGetIntegerv)>(resolve_function("glGetIntegerv"));
        auto glGetStringi_ext  = reinterpret_cast<decltype(&glGetStringi)>(resolve_function("glGetStringi"));

        int32_t numExtensions = 0;
        glGetIntegerv_ext(GL_NUM_EXTENSIONS, &numExtensions);

        for (int32_t i = 0; i < numExtensions; ++i) {
            extensions += std::format("{} ", (const char*)glGetStringi_ext(GL_EXTENSIONS, i));
        }
    }

    GET_GLEXT_FUNCTION_NO_THROW(wglGetExtensionsStringARB);
    if(wglGetExtensionsStringARB_ext)
        extensions += wglGetExtensionsStringARB_ext(dummy_surface);

    ////////////////////////////////////////////////////////

    auto surface = m_Window.surface();

    if (extensions.contains("WGL_ARB_pixel_format"))
    {
        GET_GLEXT_FUNCTION_THROW(wglChoosePixelFormatARB);

        // extension_supported shuld work caus gl ctx it valid here
        bool is_WGL_ARB_framebuffer_sRGB = extensions.contains("WGL_ARB_framebuffer_sRGB");
        bool is_WGL_ARB_multisample = extensions.contains("WGL_ARB_multisample");

        std::vector<int> attribs;

        attribs.push_back(WGL_DRAW_TO_WINDOW_ARB); attribs.push_back(GL_TRUE);
        attribs.push_back(WGL_SUPPORT_OPENGL_ARB); attribs.push_back(GL_TRUE);
        attribs.push_back(WGL_DOUBLE_BUFFER_ARB);  attribs.push_back(GL_TRUE);
        attribs.push_back(WGL_SWAP_METHOD_ARB);  attribs.push_back(WGL_SWAP_EXCHANGE_ARB);
        attribs.push_back(WGL_ACCELERATION_ARB);   attribs.push_back(WGL_FULL_ACCELERATION_ARB);
        attribs.push_back(WGL_DEPTH_BITS_ARB);     attribs.push_back(24);
        attribs.push_back(WGL_RED_BITS_ARB);       attribs.push_back(8);
        attribs.push_back(WGL_GREEN_BITS_ARB);     attribs.push_back(8);
        attribs.push_back(WGL_BLUE_BITS_ARB);      attribs.push_back(8);
        attribs.push_back(WGL_ALPHA_BITS_ARB);     attribs.push_back(8);

        attribs.push_back(WGL_PIXEL_TYPE_ARB);     attribs.push_back(WGL_TYPE_RGBA_ARB);

        if(is_WGL_ARB_framebuffer_sRGB){
            attribs.push_back(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB); attribs.push_back(GL_TRUE);
        }

        if(is_WGL_ARB_multisample && MSAA != 0){
            attribs.push_back(WGL_SAMPLE_BUFFERS_ARB);   attribs.push_back(1);
            attribs.push_back(WGL_SAMPLES_ARB);   attribs.push_back(MSAA);
        }

        attribs.push_back(0);

        int format{};
        UINT formats{};

        if (!wglChoosePixelFormatARB_ext(surface, attribs.data(), nullptr, 1, &format, &formats) || formats == 0)
            throw Exception("OpenGL does not support required pixel format.");

        pixel_format = format;
    }

    if(!DescribePixelFormat(surface, pixel_format, sizeof(pfd), &pfd))
        throw Exception("Failed Describe pixel format. : {}", GetLastError());

    if (!SetPixelFormat(surface, pixel_format, &pfd))
        throw Exception("Failed to set the pixel format. : {}", GetLastError());

    // modern OpenGL context
    if (extensions.contains("WGL_ARB_create_context")){
        bool is_WGL_ARB_create_context_profile = extensions.contains("WGL_ARB_create_context_profile");

        int flags = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
        if constexpr (DEBUG) flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
    
        std::vector<int> attribs;

        attribs.push_back(WGL_CONTEXT_MAJOR_VERSION_ARB); attribs.push_back(MIN_REQUIRED_MAJOR_VERSION);
        attribs.push_back(WGL_CONTEXT_MINOR_VERSION_ARB); attribs.push_back(MIN_REQUIRED_MINOR_VERSION);
        attribs.push_back(WGL_CONTEXT_FLAGS_ARB); attribs.push_back(flags);

        if(is_WGL_ARB_create_context_profile){
            attribs.push_back(WGL_CONTEXT_PROFILE_MASK_ARB); attribs.push_back(WGL_CONTEXT_CORE_PROFILE_BIT_ARB);
        }

        attribs.push_back(0);

        GET_GLEXT_FUNCTION_THROW(wglCreateContextAttribsARB);

        auto modern_context = wglCreateContextAttribsARB_ext(surface, nullptr, attribs.data());

        if(!modern_context){
            throw Exception("Cannot create OpenGL {}.{} not supported?", MIN_REQUIRED_MAJOR_VERSION, MIN_REQUIRED_MINOR_VERSION);
        } else {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(dummy_context);
            dummy_context = modern_context;
        }
    } else {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(dummy_context);
        dummy_context = wglCreateContext(surface);
    }

    DestroyWindow(dummy_window);
    return dummy_context;
}