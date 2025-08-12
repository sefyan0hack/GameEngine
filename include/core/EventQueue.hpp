#pragma once
#include <queue>

#include <core/Event.hpp>

class EventQueue {
public:
    EventQueue() = default;
    ~EventQueue() = default;

    EventQueue(const EventQueue&) = delete;
    auto operator=(const EventQueue&) -> EventQueue& = delete;

    EventQueue(EventQueue&&) = delete;
    auto operator=(EventQueue&&) -> EventQueue& = delete;


    void push(Event event) {
        // std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(event));
        // cv_.notify_one();
    }

    bool poll(Event& event) {
        // std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        event = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void wait_and_poll(Event& event) {
        // std::unique_lock<std::mutex> lock(mutex_);
        // cv_.wait(lock, [this]{ return !queue_.empty(); });
        event = std::move(queue_.front());
        if (queue_.empty()) return;
        queue_.pop();
    }

    void clear() {
        // std::lock_guard<std::mutex> lock(mutex_);
        queue_ = {};
    }

private:
    std::queue<Event> queue_;
    // mutable std::mutex mutex_;
    // std::condition_variable cv_;
};