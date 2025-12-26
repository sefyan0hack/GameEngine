
#include <memory>
#include <random>

#include <Engine.hpp>


#if defined(ANDROID_PLT)
bool wait_android_native_window(android_app* state) { // wait for native window and if destroyRequested return false
    if (!state) return false;

    while (state->window == nullptr && !state->destroyRequested) {
        int events;
        struct android_poll_source* source;
        if (ALooper_pollOnce(-1, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(state, source);
        }
    }

    return (state->window != nullptr && !state->destroyRequested);
}
#endif

MAIN_FUNC {
    try {
        void* platform_state = nullptr;

        #if defined(ANDROID_PLT)
        platform_state = arg1;
        if (!wait_android_native_window(arg1)) return;
        #endif

        APP app(platform_state);
        app.run();

    } catch(const Exception& e) {
        debug::log(e.all());

    } catch(const std::exception& e) {
        debug::log(e.what());

    } catch(...) {
        debug::log("Unknown Exception");
    };
}