#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/key_codes.h>
#include <GL/gl.h> // im including gl.h for some defines :) so it may caus prbblloooom
#define GL_GLES_PROTOTYPES 1
#include <gl3.h>
#include <gl2ext.h>

#define XXXGetProcAddress(name) emscripten_webgl_get_proc_address(name)

using H_WIN     = const char*;
using H_SRF     = const char*;
using GLCTX     = EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;


#define GLFUNCS GLFUNCS_COMMON