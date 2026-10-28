#pragma once

#include "ui/ui.hpp"
#include "inputs/inputs.hpp"
#include "graphics/graphics.hpp"
#include "core/core.hpp"

#define MAIN_FUNC_ENGINE(T)\
    int main(int, char**) { \
        PROFILER_BEGIN_SESSION();\
        PROFILE_FUNCTION();\
        auto app = APP();\
        T * game = nullptr;\
        { PROFILE_ZONE("Game::Game()"); game = new T(app); }\
        app.set_game(game);\
        app.run();\
        delete game;\
        PROFILER_END_SESSION();\
    }
