
#include <memory>
#include <random>

#include <DynLib.hpp>

int main() {
    auto lib = "./libGame";
    auto game = DynLib(lib);
    game.load();

    auto create_and_run_game = game.get_function<void(*)()>("create_and_run_game");

    create_and_run_game();

    return 0;
}