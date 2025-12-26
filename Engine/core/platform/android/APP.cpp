#include "APP.hpp"

auto APP::run() -> void {
    auto state = static_cast<struct android_app*>(m_State);

    while (m_Running) {
        int events;
        struct android_poll_source* source;
        int timeout = (state->window == nullptr) ? -1 : 0; //  -1 (wait forever) if no window, 0 (don't wait) if rendering

        while (ALooper_pollOnce(timeout, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(state, source);
            if (state->destroyRequested != 0) m_Running = false;
        }

        if (m_Running && state->window != nullptr) APP::loop_body(this);
    }
}