
#include <memory>
#include <random>

#include <Engine.hpp>


MAIN_FUNC {
    try {
        void* platform_state = nullptr;

        #if defined(ANDROID_PLT)
        if(arg1 == nullptr) throw Exception("android_app is null");

        auto state = arg1;

        state->onAppCmd = [](struct android_app* app, int32_t cmd) {
                if (cmd == APP_CMD_INIT_WINDOW) {
                    // Signal that we are ready. 
                    // We don't create APP here yet to keep logic out of the lambda.
                }
            };

        while (state->window == nullptr) {
            int events;
            struct android_poll_source* source;

            // -1 means block indefinitely until an event arrives
            if (ALooper_pollOnce(-1, nullptr, &events, (void**)&source) >= 0) {
                if (source) source->process(state, source);
            }

            if (state->destroyRequested) return;
        }

        platform_state = state;
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