#pragma once

#include <emath/emath.hpp>
#include <engine_export.h>

#include <tuple>
#include <memory>
#include <string>
#include <vector>
#include <format>
#include <unordered_map>

class ENGINE_EXPORT ShaderProgram {
public:
    using InternalType = std::tuple<uint32_t, uint32_t, int32_t>;

    friend struct std::formatter<ShaderProgram>;
    ShaderProgram(std::shared_ptr<class Shader> vertex, std::shared_ptr<class Shader> fragment);
    ShaderProgram(const char* vertex, const char* fragment);
    
    // ShaderProgram(const ShaderProgram& other);
    ShaderProgram(ShaderProgram&& other) noexcept;
    auto operator=(ShaderProgram&& other) noexcept -> ShaderProgram& ;
    
    ~ShaderProgram();

    auto id() const noexcept -> uint32_t ;
    auto use() const -> void ;
    auto uniform_count() const                  -> int32_t ;
    auto attribs_count() const                  -> int32_t ;
    auto uniform_location(const char*) const -> uint32_t;
    auto attrib_location(const char*) const -> uint32_t;
    auto uniforms() const noexcept -> const std::unordered_map<std::string, InternalType>&;
    auto attribs() const noexcept -> const std::unordered_map<std::string, InternalType>&;
    static auto current_program() -> uint32_t;
    static auto glsl_type_to_string(uint32_t type) -> const char*;
    
    auto set_uniform(const std::string &name, const uint32_t &value) const -> void;
    auto set_uniform(const std::string &name, const float &value) const -> void;
    auto set_uniform(const std::string &name, const int32_t &value) const -> void;
    auto set_uniform(const std::string &name, const emath::vec2 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::vec3 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::vec4 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::ivec2 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::ivec3 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::ivec4 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::uvec2 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::uvec3 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::uvec4 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::mat2 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::mat3 &value) const -> void;
    auto set_uniform(const std::string &name, const emath::mat4 &value) const -> void;
    auto set_uniform(const std::string& name, const emath::mat4* value, int32_t count) const -> void;

private:
    auto get_program_info(uint32_t what) const -> int32_t;
    auto link() const -> void;
    auto check_link_status() -> std::string;
    auto uniform_location_prv(const char* name) const -> uint32_t;
    auto attrib_location_prv(const char* name) const -> uint32_t;
    auto dump_uniforms()                   -> void ;
    auto dump_attribs()                    -> void ;

private:
    uint32_t m_Id;
    std::vector<std::shared_ptr<class Shader>> m_Shaders;
    std::unordered_map<std::string, InternalType> m_Attribs;
    std::unordered_map<std::string, InternalType> m_Uniforms;
};

#ifdef __cpp_lib_formatters
// custom ShaderProgram::InternalType Format
template<>
struct std::formatter<ShaderProgram::InternalType> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const ShaderProgram::InternalType& obj, auto& context) const {
    auto [loc, type, size] = obj;
    return std::format_to(context.out(),
    R"({{ "loc": {}, "type": {}, "size": {} }})",
    loc, ShaderProgram::glsl_type_to_string(type), size);
  }
};

// custom ShaderProgram Format
template<>
struct std::formatter<ShaderProgram> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const ShaderProgram& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "attribs": {}, "uniforms": {} }})",
    obj.m_Id, obj.m_Attribs, obj.m_Uniforms);
  }
};
#endif