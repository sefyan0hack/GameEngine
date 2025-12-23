
#include <memory>
#include <random>

#include <Engine.hpp>


constexpr auto WINDOW_WIDTH = 1180;
constexpr auto WINDOW_HIEGHT = 640;


MAIN_FUNC {
    try {
        #if defined(ANDROID_PLT)
        CWindow window(arg1->window);
        #else
        CWindow window(WINDOW_WIDTH, WINDOW_HIEGHT, "");
        #endif

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