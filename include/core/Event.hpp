#pragma once
#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>


struct MouseWheelEvent {
    int16_t delta;
};

struct MouseRawEvent {
    int16_t dx;
    int16_t dy;
};

struct WindowResizeEvent {
    uint16_t width;
    uint16_t height;
};

struct QuitEvent {};
struct LoseFocusEvent {};

using Event = std::variant<
    std::monostate,
    Keyboard::Event,
    Mouse::Event,
    MouseWheelEvent,
    MouseRawEvent,
    WindowResizeEvent,
    QuitEvent,
    LoseFocusEvent
>;


class EventQueue {
public:
    EventQueue() = default;

    EventQueue(const EventQueue& other) {
        std::lock_guard<std::mutex> lock(other.mutex_);
        queue_ = other.queue_;
    }

    EventQueue(EventQueue&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mutex_);
        queue_ = std::move(other.queue_);
    }

    EventQueue& operator=(const EventQueue& other) {
        if (this == &other) return *this;
        std::scoped_lock lock(mutex_, other.mutex_);
        queue_ = other.queue_;
        return *this;
    }

    EventQueue& operator=(EventQueue&& other) noexcept {
        if (this == &other) return *this;
        std::scoped_lock lock(mutex_, other.mutex_);
        queue_ = std::move(other.queue_);
        return *this;
    }

    ~EventQueue() = default;

    void push(Event event) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(event));
        cv_.notify_one();
    }

    bool poll(Event& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        event = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void wait_and_poll(Event& event) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]{ return !queue_.empty(); });
        event = std::move(queue_.front());
        queue_.pop();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_ = {};
    }

private:
    std::queue<Event> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
};