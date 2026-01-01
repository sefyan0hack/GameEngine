#include <APP.hpp>

MAIN_FUNC {
    void* platform_state = nullptr;

    #if defined(ANDROID_PLT)
        platform_state = arg1;
        extern "C" auto wait_android_native_window(void*) -> bool;
        if (!wait_android_native_window(arg1)) return;
    #endif

    APP app(platform_state);
    app.run();
}