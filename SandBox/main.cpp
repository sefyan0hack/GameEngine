
#include <memory>
#include <random>
#include <thread>
#include <chrono>

#include <core/DynLib.hpp>

int main() {
    auto lib = "./libGame.dll";
    auto game = DynLib(lib);
    game.load();

    auto create_and_run_game = game.get_function<void(*)()>("create_and_run_game");

    std::thread([&](){
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            create_and_run_game = game.get_function<void(*)()>("foo");
            // auto curr_mod_time = DynLib::file_mod_time(std::string(lib));
            // if(curr_mod_time > game.mod_time())
            // {
            //     game.upadte_mod_time(curr_mod_time);
            //     game.reload();
            //     create_and_run_game = game.get_function<void(*)()>("create_and_run_game");
            // }
        }
    }).detach();

    create_and_run_game();

    return 0;
}