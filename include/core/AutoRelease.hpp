#pragma once

template <typename T>
class AutoRelease {

private:
    T resource{};
    std::function<void(T)> deleter;
    std::shared_ptr<std::atomic<size_t>> ref_count;

public:
    AutoRelease() = default;
    AutoRelease(T res, std::function<void(T)> del) noexcept
        : resource(res), deleter(std::move(del)), ref_count(std::make_shared<std::atomic<size_t>>(1)) {}

    AutoRelease(AutoRelease&& other) noexcept
        : resource(other.resource), deleter(std::move(other.deleter)), ref_count(std::move(other.ref_count)) {
        other.resource = 0;
    }

    AutoRelease& operator=(AutoRelease&& other) noexcept {
        if (this != &other) {
            release();
            resource = other.resource;
            deleter = std::move(other.deleter);
            ref_count = std::move(other.ref_count);
            other.resource = 0;
        }
        return *this;
    }

    AutoRelease(const AutoRelease&) = default;
    AutoRelease& operator=(const AutoRelease&) = default;

    bool operator==(const AutoRelease& other) const
    {
        return this->resource == other.resource;
    }

    ~AutoRelease() {
        release();
    }

    void release() noexcept {
        if (ref_count && --(*ref_count) == 0) {
            deleter(resource);
            resource = 0;
        }
    }

    T get() const noexcept {
        return resource;
    }

    constexpr operator T() const noexcept {
        return resource;
    }

    size_t use_count() const noexcept {
        return ref_count ? ref_count->load(std::memory_order_relaxed) : 0;
    }
    FOR_TEST
};
