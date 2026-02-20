#pragma once
#include <variant>
#include <queue>

using Event = std::variant<
    struct Keyboard::KeyDownEvent,
    struct Keyboard::KeyUpEvent,
    struct Mouse::ButtonDownEvent,
    struct Mouse::ButtonUpEvent,
    struct Mouse::MoveEvent,
    struct Mouse::EnterEvent,
    struct Mouse::LeaveEvent,
    struct Mouse::MovementEvent,
    struct CWindow::ResizeEvent,
    struct CWindow::LoseFocusEvent,
    struct CWindow::SetFocusEvent,
    struct CWindow::QuitEvent
>;

template<class T>
class EQueue : public std::queue<T> { //TODO: on push make sur that the pusher it holds the event type ??  some metaprograming time
private: EQueue() = default;
public:

    auto pull(T& event) -> bool
    {
        if (this->empty()) return false;
        event = std::move(this->front());
        this->pop();
        return true;
    }

    auto clear() { while(!this->empty()) this->pop(); }

    static auto self() -> EQueue&
    {
        static EQueue eQ;
        return eQ;
    }
};

using EventQ = EQueue<Event>;