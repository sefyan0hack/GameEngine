#pragma once

#include "ui/ui.hpp"
#include "inputs/inputs.hpp"
#include "graphics/graphics.hpp"
#include "core/core.hpp"

int main(int, char**) {
    auto& app = APP::self();
    app.init();
    app.run();
}