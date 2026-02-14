#include "APP.hpp"
#include <emscripten/emscripten.h>

auto APP::run() -> void
{
    emscripten_set_main_loop_arg(loop_body, this, 0, 1);
}