#include "gl.hpp"
#include <core/Log.hpp>
#include <core/Exception.hpp>
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
        #define FUNC_GL_X(name) gl::name = reinterpret_cast<decltype(&gl##name)>(gl::get_proc_address("gl"#name));
        FUNCTIONS_GL_LIST
        #undef FUNC_GL_X
    }
}