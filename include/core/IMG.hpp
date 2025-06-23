#pragma once 


enum class IMGType : uint8_t {
    JPEG,
    PNG,
};

class IMG
{
public:
    IMG() = default;
    IMG(const char* path);
    IMG(IMG&& other);
    auto operator= (IMG&& other) -> IMG&;
    ~IMG();
private:
    auto operator= (const IMG&& other) -> IMG&;
    IMG(const IMG& other);

public:
    IMG clone(const IMG& other);

    auto Load(const char* path) -> bool;
    auto ReLoad(const char* path) -> bool;
    auto Load(IMGType type, std::size_t Width, std::size_t Height, std::size_t Channels, std::byte* data) -> bool;
    auto ReLoad(IMGType type, std::size_t Width, std::size_t Height, std::size_t Channels, std::byte* data) -> bool;

    auto format() const -> GLenum;
    auto internalformat() const -> GLint;

    auto Path() const -> std::string;
    auto Type() const -> IMGType;
    auto Data() const -> const std::byte&;
    auto Width() const -> std::size_t;
    auto Height() const -> std::size_t;
    auto Channels() const -> std::size_t;

private:
    std::string m_Path;
    IMGType m_Type;
    std::byte* m_Data;
    std::size_t m_Width, m_Height, m_Channels;
};