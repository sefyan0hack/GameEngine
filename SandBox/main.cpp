
#include <memory>
#include <random>
#include <thread>
#include <chrono>

#include <core/DynLib.hpp>

int main() {
    auto lib = "./libGame";
    auto game = DynLib(lib);
    game.load();

    auto create_and_run_game = game.get_function<void(*)()>("create_and_run_game");
    if (!create_and_run_game) Exception("Can't load symbol `{}`: {}", "create_and_run_game", game.error());

    create_and_run_game();

    return 0;
}