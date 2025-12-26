#include "APP.hpp"

auto APP::run() -> void {
    auto state = static_cast<struct android_app*>(m_State);
    state->userData = this;

    state->onAppCmd = [](struct android_app* state, int32_t cmd) {
        auto app = static_cast<APP*>(state->userData);
        switch(cmd) {
            case APP_CMD_INIT_WINDOW:
                // Potentially re-init renderer here if window was lost
                break;
            case APP_CMD_TERM_WINDOW:
                break;
            case APP_CMD_DESTROY:
                app->m_Running = false;
                break;
        }
    };

    while (m_Running) {
        int events;
        struct android_poll_source* source;

        while (ALooper_pollOnce(0, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(state, source);
            if (state->destroyRequested != 0) m_Running = false;
        }

        if (m_Running) APP::loop_body(this);
    }
}