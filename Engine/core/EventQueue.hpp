#pragma once
#include <queue>
#include <engine_export.h>

#include "Event.hpp"

class ENGINE_API EventQueue {
public:
    EventQueue() = default;

    EventQueue(const EventQueue&) = delete;
    auto operator=(const EventQueue&) -> EventQueue& = delete;

    EventQueue(EventQueue&&) = delete;
    auto operator=(EventQueue&&) -> EventQueue& = delete;


    void push(Event event) {
        queue_.push(std::move(event));
    }

    bool poll(Event& event) {
        if (queue_.empty()) return false;
        event = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void wait_and_poll(Event& event) {
        event = std::move(queue_.front());
        if (queue_.empty()) return;
        queue_.pop();
    }

    void clear() {
        queue_ = {};
    }

private:
    std::queue<Event> queue_;
};