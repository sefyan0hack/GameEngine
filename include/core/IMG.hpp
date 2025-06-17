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
    auto Load(IMGType type, size_t Width, size_t Height, size_t Channels, std::byte* data) -> bool;
    auto ReLoad(IMGType type, size_t Width, size_t Height, size_t Channels, std::byte* data) -> bool;

    auto format() const -> GLenum;
    auto internalformat() const -> GLint;

    auto Path() const -> std::string;
    auto Type() const -> IMGType;
    auto Data() const -> const std::byte&;
    auto Width() const -> size_t;
    auto Height() const -> size_t;
    auto Channels() const -> size_t;

private:
    std::string m_Path;
    IMGType m_Type;
    std::byte* m_Data;
    size_t m_Width, m_Height, m_Channels;
};