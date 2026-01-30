#include "gl.hpp"
#include <core/Log.hpp>
#include <core/SysInfo.hpp>
namespace gl {

    auto get_proc_address(const char* name) -> void* {
        void *address = reinterpret_cast<void*>(gl::GetProcAddress(name));

        if(address == nullptr){
            address = os::get_proc_address(OPENGL_MODULE_NAME, name);
        }

        if (address == nullptr) {
            throw Exception("Couldn't load {} function `{}`", OPENGL_MODULE_NAME, name);
        }

        return address;
    }

    auto get_integer(GLenum name) -> GLint
    {
        constexpr auto INVALID = std::numeric_limits<GLint>::max();

        GLint maxTexSize = INVALID;

        gl::GetIntegerv(name, &maxTexSize);

        if (maxTexSize != INVALID)
            return maxTexSize;
        else
            throw Exception("GetIntegerv Failed");
    }

    auto get_boolean(GLenum name) -> GLboolean
    {
        constexpr auto INVALID = std::numeric_limits<GLboolean>::max();

        GLboolean maxTexSize = INVALID;

        gl::GetBooleanv(name, &maxTexSize);

        if (maxTexSize != INVALID)
            return maxTexSize;
        else
            throw Exception("GetBooleanv Failed");
    }


    auto load_opengl_functions() -> void
    {
        #undef X
        #ifdef ROBUST_GL_CHECK
        #   define X(name)\
            name = Function<PFN_gl##name>(\
                reinterpret_cast<PFN_gl##name>(gl::get_proc_address("gl"#name)),\
                "gl"#name,\
                nullptr,\
                [](std::string info) { auto err = glGetError(); if(err != GL_NO_ERROR) if(!info.contains("glClear")) throw Exception("gl error id {} {}", err, info); }\
            );
        #else
        #   define X(name)\
            name = reinterpret_cast<PFN_gl##name>(gl::get_proc_address("gl"#name));
        #endif

        GLFUNCS
    }

    auto export_opengl_functions() -> void**
    {
        #undef X
        #define X(name) (void*)&name,
        
        static void* funcs[OPENGL_FUNCTIONS_COUNT] = { GLFUNCS };

        return funcs;
    }

    auto import_opengl_functions(void **funcs) -> void
    {
        int index = 0;
        #undef X
        #if ROBUST_GL_CHECK
        #   define X(name) name = *static_cast<Function<PFN_gl##name>*>(funcs[index++]);
        #else
        #   define X(name) name = reinterpret_cast<PFN_gl##name>(funcs[index++]);
        #endif

        GLFUNCS
    }
}