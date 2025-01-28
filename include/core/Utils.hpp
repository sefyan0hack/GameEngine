#pragma once

#include <thread>
#include <chrono>
#include <functional>

template<typename Function, typename... Args>
auto setTimeOut( unsigned long delay, Function&& func, Args&&... args) -> void
{
    std::thread([func, delay, args...]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        std::invoke(func, std::forward<Args>(args)...);
    }).detach();
}

// compile time
constexpr std::string_view get_file_directory(const std::string_view file_path = __FILE__) {
    const size_t last_slash = file_path.find_last_of("/\\");
    return (last_slash == std::string_view::npos) ? "" : file_path.substr(0, last_slash);
}

constexpr std::string_view current_directory = get_file_directory();
