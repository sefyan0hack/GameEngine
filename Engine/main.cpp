
#include <memory>
#include <random>

#include <Engine.hpp>


constexpr auto WINDOW_WIDTH = 1180;
constexpr auto WINDOW_HIEGHT = 640;


int main() {
    try {
        CWindow window(WINDOW_WIDTH, WINDOW_HIEGHT, "");
        APP app(window);
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