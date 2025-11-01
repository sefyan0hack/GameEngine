#pragma once

class GraphicApi
{
    public:
        virtual ~GraphicApi() = default;

    public:
        virtual auto context() const -> GLCTX = 0;
        virtual auto creation_time() const -> std::time_t = 0;
        virtual auto has_extension(const std::string& ext) const -> bool = 0;

        virtual  auto extensions() -> std::vector<std::string> = 0;
};