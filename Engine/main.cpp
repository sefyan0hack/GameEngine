
#include <memory>
#include <random>

#include "Engine.hpp"

int main() {
    try {
        APP app;
        app.run();

    } catch(const Exception& e) {
        debug::print(e.all());

    } catch(const std::exception& e) {
        debug::print(e.what());

    } catch(...) {
        debug::print("Unknown Exception");
    };

    return 0;
}