#pragma once
#include <string>
#include <string_view>
#include <source_location>
#include <array>
#include <utility>
#include <sstream>
#include <type_traits>
#include <engine_export.h>
#include "Exception.hpp"
#include "Utils.hpp"

template <typename T>
class ENGINE_EXPORT Function;

template <typename R, typename... Args>
class Function<R(*)(Args...)> {
public:
    using FuncType = R(*)(Args...);
    using BeforeType = void(*)(void);
    using AfterType = void(*)(std::string);

    Function();
    Function(FuncType fptr, std::string name, BeforeType before = nullptr, AfterType after = nullptr);

    auto operator()(Args... args, std::source_location loc = std::source_location::current()) -> R;

    auto name() const -> std::string;
    auto function() const -> FuncType;
    auto return_type() const -> std::string_view;
    auto args_types() const  -> std::array<std::string_view, sizeof...(Args)>;
    auto calls() const -> std::size_t;

    static auto default_([[maybe_unused]] Args... args) -> R;
private:
    auto this_func_sig(Args... args) const -> std::string ;
    auto function_info(const std::source_location& loc, Args... args) -> std::string;

private:
    FuncType m_Func;
    BeforeType m_Befor;
    AfterType m_After;
    std::string m_Name;
    std::string_view m_ReturnType;
    std::array<std::string_view, sizeof...(Args)> m_ArgsTypes;
    std::size_t m_Calls;
};

// impl

template <typename R, typename... Args>
inline Function<R(*)(Args...)>::Function()
    : m_Func(&default_)
    , m_Befor(nullptr)
    , m_After(nullptr)
    , m_Name()
    , m_ReturnType(::type_name<R>())
    , m_ArgsTypes{::type_name<Args>()...}
    , m_Calls(0)
    {}

template <typename R, typename... Args>
inline Function<R (*)(Args...)>::Function(FuncType fptr, std::string name, BeforeType before, AfterType after)
    : m_Func(fptr)
    , m_Befor(before)
    , m_After(after)
    , m_Name(name)
    , m_ReturnType(::type_name<R>())
    , m_ArgsTypes{::type_name<Args>()...}
    , m_Calls(0)
    {}

template <typename R, typename... Args>
inline auto  Function<R(*)(Args...)>::default_([[maybe_unused]] Args... args) -> R
{
    if constexpr (!std::is_void_v<R>) return R{};
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::operator()(Args... args, std::source_location loc) -> R
{
    if(m_Func == nullptr) [[unlikely]] throw Exception( "{} not loaded!", m_Name);
    m_Calls++;

    if(m_Befor) [[unlikely]] m_Befor();

    if constexpr (std::is_void_v<R>) {
        m_Func(args...);
        if(m_After) [[likely]] m_After(function_info(loc, args...));
    } else {
        R result = m_Func(args...);
        if(m_After) [[likely]] m_After(function_info(loc, args...));
        return result;
    }
}

template <typename R, typename... Args>
inline auto Function<R (*)(Args...)>::name() const -> std::string
{
    return m_Name;
}

template <typename R, typename... Args>
inline auto Function<R (*)(Args...)>::function() const -> FuncType
{
    return m_Func;
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::return_type() const -> std::string_view
{
    return m_ReturnType;
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::args_types() const -> std::array<std::string_view, sizeof...(Args)>
{
    return m_ArgsTypes;
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::calls() const -> std::size_t
{
    return m_Calls;
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::this_func_sig(Args... args) const -> std::string {
    std::string result = ::format("{} {}(", m_ReturnType, m_Name);
    std::size_t index = 0;

    [[maybe_unused]] auto format_arg = [&](const auto& val) {
        if (index > 0) result += ", ";
        std::string v;

        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_pointer_v<T>) {
            v = utils::pointer_to_string(val);
        } else if constexpr (std::is_arithmetic_v<T>) {
            v = std::to_string(val);
        } else if constexpr (std::is_convertible_v<T, std::string>) {
            v = std::string(val);
        } else if constexpr (requires(std::ostream& os) { os << val; }) {
            std::stringstream ss;
            ss << val;
            v = ss.str();
        } else {
            v = "??";
        }

        result += ::format("{} arg_{} = {}", m_ArgsTypes[index], index + 1, v);
        index++;
    };

    (format_arg(args), ...);

    return result + ")";
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::function_info(const std::source_location& loc, Args... args) -> std::string
{
    return ::format(
        "call Number: {} ; instrments(Befor: {}, After: {}) |>=> {} -> {}:{}\n",
        m_Calls,
        m_Befor ? "true" : "false", m_After ? "true" : "false",
        this_func_sig(args...),
        loc.file_name(), loc.line()
    );
}