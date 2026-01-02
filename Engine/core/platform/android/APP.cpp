#include "APP.hpp"

extern struct android_app* g_android_app;
auto APP::run() -> void {

    while (m_Running) {
        int events;
        struct android_poll_source* source;
        int timeout = (g_android_app->window == nullptr) ? -1 : 0; //  -1 (wait forever) if no window, 0 (don't wait) if rendering

        while (ALooper_pollOnce(timeout, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(g_android_app, source);
            if (g_android_app->destroyRequested != 0) m_Running = false;
        }

        if (m_Running && g_android_app->window != nullptr) APP::loop_body(this);
    }
}