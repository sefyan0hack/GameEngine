#pragma once

template <typename T>
class AutoRelease {

private:
    T resource{};
    std::function<void(T)> deleter;
    std::shared_ptr<std::atomic<size_t>> ref_count;

public:
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
            other.resource = T{};
        }
        return *this;
    }

    AutoRelease(const AutoRelease& other)
    : resource(other.resource), deleter(other.deleter), ref_count(other.ref_count) {
        if (ref_count) {
            ++(*ref_count);
        }
    }

    AutoRelease& operator=(const AutoRelease& other) {
        if (this != &other) {
            T new_resource = other.resource;
            std::function<void(T)> new_deleter = other.deleter;
            auto new_ref_count = other.ref_count;

            if (new_ref_count) {
                ++(*new_ref_count);
            }

            std::swap(resource, new_resource);
            std::swap(deleter, new_deleter);
            std::swap(ref_count, new_ref_count);

            if (new_ref_count) {
                if (--(*new_ref_count) == 0) {
                    if (new_resource != 0) {
                        new_deleter(new_resource);
                    }
                }
            }
        }
        return *this;
    }

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
