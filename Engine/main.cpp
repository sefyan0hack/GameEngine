
#include <memory>
#include <random>

#include <Engine.hpp>


MAIN_FUNC {
    try {
        APP* Main_App = nullptr;

        #if defined(ANDROID_PLT)
        if(arg1 == nullptr) throw Exception("android_app is null");

        auto state = arg1;
        state->userData = Main_App;

        state->onAppCmd = [](struct android_app* state, int32_t cmd) -> void {
            auto app = reinterpret_cast<APP*>(state->userData);

            switch(cmd){
                case APP_CMD_INIT_WINDOW:
                    {
                        if(!app) app = new APP(state);
                        state->userData = app;
                    }
                    break;
                case APP_CMD_TERM_WINDOW:
                    {
                        if(app) delete app;
                        state->userData = nullptr;
                    }
                    break;
            }
        };

        while (true) {
            int events;
            struct android_poll_source* source;

            // Poll for events
            while (ALooper_pollOnce(state->userData ? 0 : -1, nullptr, &events, (void**)&source) >= 0) {
                if (source != nullptr) source->process(state, source);
                if (state->destroyRequested != 0) return;
            }
            if (state->userData) APP::loop_body(state->userData);
        }

        #else
        Main_App = new APP();
        Main_App->run();
        delete Main_App;
        #endif

    } catch(const Exception& e) {
        debug::log(e.all());

    } catch(const std::exception& e) {
        debug::log(e.what());

    } catch(...) {
        debug::log("Unknown Exception");
    };
}