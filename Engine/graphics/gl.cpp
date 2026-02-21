#include "gl.hpp"
#include <core/Log.hpp>
#include <core/SysInfo.hpp>

namespace gl {
    auto get_proc_address(const char* name) -> void* {
        void *address = gl::GetProcAddress<void*>(name);

        if(address == nullptr){
            address = os::get_proc_address(EG_OPENGL_MODULE_NAME, name);
        }

        if (address == nullptr) {
            throw Exception("Couldn't load {} function `{}`", EG_OPENGL_MODULE_NAME, name);
        }

        return address;
    }

    auto load_opengl_functions() -> void
    {
        #undef X
        #ifdef ROBUST_GL_CHECK
        #   define X(name)\
            name = Function<decltype(&gl##name)>(\
                reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name)),\
                "gl"#name,\
                nullptr,\
                [](std::string info) { auto err = glGetError(); if (err != GL_NO_ERROR) [[unlikely]] if(!info.contains("glClear")) throw Exception("gl error id {} {}", err, info); }\
            );
        #else
        #   define X(name)\
            name = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name));
        #endif

        GLFUNCS
    }

    auto get_integer(GLenum name) -> GLint
    {
        GLint maxTexSize = -1;

        gl::GetIntegerv(name, &maxTexSize);

        if (maxTexSize != -1)
            return maxTexSize;
        else
            throw Exception("GetIntegerv Failed");
    }

    auto get_boolean(GLenum name) -> GLboolean
    {
        GLboolean maxTexSize = false;
        gl::GetBooleanv(name, &maxTexSize);
        return maxTexSize;
    }

}