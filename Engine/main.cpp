
#include <memory>
#include <random>

#include <Engine.hpp>


[[maybe_unused]] constexpr auto WINDOW_WIDTH = 1180;
[[maybe_unused]] constexpr auto WINDOW_HIEGHT = 640;


#if defined(ANDROID_PLT)
struct android_app *Android_app = nullptr;
#endif

MAIN_FUNC {
    try {
        #if defined(ANDROID_PLT)
        Android_app = arg1;
        if(Android_app == nullptr) throw Exception("android_app is null");

        // Wait for the window handle to be valid
        while (arg1->window == nullptr) {
            int ident;
            int events;
            struct android_poll_source* source;
            if ((ident = ALooper_pollOnce(-1, nullptr, &events, (void**)&source)) >= 0) {
                if (source != nullptr) source->process(Android_app, source);
            }
            if (arg1->destroyRequested != 0) return; 
        }

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
}