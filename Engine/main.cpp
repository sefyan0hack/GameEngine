#include <APP.hpp>

#if defined(ANDROID_PLT)
    extern "C" bool wait_android_native_window(void*);
#endif

MAIN_FUNC {
    void* platform_state = nullptr;

    #if defined(ANDROID_PLT)
        platform_state = arg1;
        if (!wait_android_native_window(arg1)) return;
    #endif

    APP app(platform_state);
    app.run();
}