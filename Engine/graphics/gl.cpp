#include "gl.hpp"
namespace gl {

    #undef X
    #ifdef ROBUST_GL_CHECK
    #   define X(name) Function<PFN_gl##name> name;
    #else
    #   define X(name) PFN_gl##name name = Function<PFN_gl##name>::default_;
    #endif
    
    extern "C" {
        GLFUNCS
    }

    auto get_proc_address(const char* name) -> void* {
        void *address = reinterpret_cast<void*>(XXXGetProcAddress(name));

        if(address == nullptr){
            address = utils::get_proc_address(OPENGL_MODULE_NAME, name);
        }

        if (address != nullptr) {
            debug::print("from LIB:`{}`: load function `{}` at : {}", OPENGL_MODULE_NAME, name, address);
        } else {
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
            name = Function<PFN_gl##name>{};\
            name.m_Func  = reinterpret_cast<PFN_gl##name>(gl::get_proc_address("gl"#name));\
            name.m_After = [](std::string info) { auto err = glGetError(); if(err != GL_NO_ERROR) if(!info.contains("glClear")) throw Exception("gl error id {} {}", err, info); };\
            name.m_Name  = "gl"#name;
        #else
        #   define X(name)\
            name = reinterpret_cast<PFN_gl##name>(gl::get_proc_address("gl"#name));
        #endif

        GLFUNCS
    }

    auto export_opengl_state(void **state) -> void
    {
        int index = 0;
        #undef X
        #define X(name) state[index++] = (void*)name;
        
        GLFUNCS
    }

    auto import_opengl_state(void **state) -> void
    {
        int index = 0;
        #undef X
        #if ROBUST_GL_CHECK
        #   define X(name) \
            name.m_Func  = reinterpret_cast<std::remove_reference_t<PFN_gl##name>>(state[index++]);\
            name.m_After = [](std::string info) { auto err = glGetError(); if(err != GL_NO_ERROR) if(!info.contains("glClear")) throw Exception("gl error id {} {}", err, info); };\
            name.m_Name  = "gl"#name;
        #else
        #   define X(name) name = reinterpret_cast<std::remove_reference_t<PFN_gl##name>>(state[index++]);
        #endif

        GLFUNCS
    }
}