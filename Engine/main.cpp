
#include <memory>
#include <random>

#include <Engine.hpp>


int main() {
    try {
        APP app;
        app.run();

    } catch(const Exception& e) {
        debug::log(e.all());

    } catch(const std::exception& e) {
        debug::log(e.what());

    } catch(...) {
        debug::log("Unknown Exception");
    };

    return 0;
}