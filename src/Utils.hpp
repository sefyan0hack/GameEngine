#pragma once

#include <thread>
#include <chrono>
#include <functional>

template<typename Function, typename... Args>
void setTimeOut( unsigned long delay, Function&& func, Args&&... args) {
    std::thread([func, delay, args...]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        std::invoke(func, std::forward<Args>(args)...);
    }).detach();
}