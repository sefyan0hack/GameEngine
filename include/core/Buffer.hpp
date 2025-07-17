#pragma once

#include <core/Utils.hpp>
#include <core/Log.hpp>

class Buffer
{
public:
    Buffer(std::size_t size, bool zeroed = false)
    : m_Size(0)
    , m_Data(nullptr)
    , m_Cursor(0)
    {
        m_Size = size;
        Expect(m_Size > 0, "Size < 0");

        m_Data = new std::byte[m_Size];

        if(zeroed) { 
            std::memset(m_Data, 0, m_Size);
        }
    }

    Buffer(const Buffer&) = delete;
    auto operator=(const Buffer&) -> Buffer& = delete;

    Buffer(Buffer&& other) noexcept
        : m_Size(std::exchange(other.m_Size, 0))
        , m_Data(std::exchange(other.m_Data, nullptr))
        , m_Cursor(std::exchange(other.m_Cursor, 0))
    {}

    auto operator=(Buffer&& other) -> Buffer&
    {
        if(this != &other){
            this->m_Size = std::exchange(other.m_Size, 0);
            this->m_Data = std::exchange(other.m_Data, nullptr);
            this->m_Cursor = std::exchange(other.m_Cursor, 0);
        }

        return *this;
    }

    auto operator==(Buffer& other) const -> bool
    {
        return this->m_Size == other.m_Size && this->m_Data == other.m_Data;
    }

    auto operator!=(Buffer& other) const -> bool
    {
        return !(*this == other);
    }

    ~Buffer() {
        if(m_Data) {
            delete[] m_Data;
            m_Data = nullptr;
        };
    }

    auto operator << (const auto& val) -> void
    {
        push(val);
    }

    operator const uint8_t*() const {
        return reinterpret_cast<const uint8_t*>(m_Data);
    }

    operator const int8_t*() const {
        return reinterpret_cast<const int8_t*>(m_Data);
    }

    auto clone() const -> Buffer
    {
        auto newBuffer = Buffer(m_Size);
        if(m_Size > 0){
            std::memcpy(newBuffer.m_Data, this->m_Data, newBuffer.size());
        }

        return newBuffer;
    }

    auto write(std::byte* val, std::size_t loc, std::size_t size) -> void
    {
        Expect(loc < m_Size - 1 ," can't write in loc > size of the buffer");
        std::memcpy(&m_Data[loc], val, size);
    }

    auto push(Pointer auto val, std::size_t size) -> void
    {
        Expect(m_Cursor != m_Size - 1 ," no space left in the Buffer");
        std::memcpy(&m_Data[m_Cursor+=size], val, size);
    }

    auto push(const auto& val) -> void
    {
        Expect(m_Cursor != m_Size - 1 ," no space left in the Buffer");
        constexpr std::size_t size_of_type = sizeof(val);
        std::memcpy(&m_Data[m_Cursor+=size_of_type], &val, size_of_type);
    }

    auto size() const -> std::size_t
    {
        return m_Size;
    }

    auto data() const -> const std::byte*
    {
        return m_Data;
    }

    auto empty() const -> bool
    {
        return m_Size == 0;
    }

private:
    std::size_t m_Size;
    std::byte* m_Data;
    std::size_t m_Cursor;

    FOR_TEST
};