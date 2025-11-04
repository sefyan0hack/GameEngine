#pragma once

#include <vector>
#include <string>
#include <variant>
#include <ctime>
#include "gl.h"

using GCTX = std::variant<std::monostate, GL_CTX>;

class GApi
{
    public:
        virtual ~GApi() = default;

        virtual auto context() const -> GCTX = 0;
        virtual auto is_valid() const -> bool = 0;
        virtual auto creation_time() const -> std::time_t = 0;
        virtual auto has_extension(const std::string& ext) const -> bool = 0;
};