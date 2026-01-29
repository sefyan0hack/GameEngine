#pragma once
#include <string>
#include <string_view>
#include <source_location>
#include <array>
#include <tuple>
#include <utility>
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
    auto operator= (Function&& other) -> Function&;
    auto operator= (const Function& other) -> Function&;
    Function(Function&& other);
    Function(const Function& other);

    auto operator()(Args... args, std::source_location loc = std::source_location::current()) -> R;

    static auto default_([[maybe_unused]] Args... args) -> R;
    auto name() -> std::string;
    auto function() -> FuncType;
    auto return_type() const -> std::string_view;
    auto args_values() const -> std::array<std::string, sizeof...(Args)>;
    auto args_types() const  -> std::array<std::string_view, sizeof...(Args)>;
    constexpr auto args_count() const -> std::size_t;
    auto calls_count() const -> std::size_t;

private:
    auto this_func_sig() const -> std::string ;

    auto function_info(const std::source_location& loc) -> std::string;
    auto format_arguments(std::string& result) const -> void;

    static auto to_string(const auto& value) -> std::string ;

private:
    FuncType m_Func;
    BeforeType m_Befor;
    AfterType m_After;
    std::string m_Name;
    std::string_view m_ReturnType;
    std::array<std::string_view, sizeof...(Args)> m_ArgsTypes;
    std::tuple<Args...> m_ArgsValues;
    std::size_t m_CallCount;
};

// impl

template <typename R, typename... Args>
inline Function<R(*)(Args...)>::Function()
    : m_Func(&default_)
    , m_Befor(nullptr)
    , m_After(nullptr)
    , m_Name("Function")
    , m_ReturnType(::type_name<R>())
    , m_ArgsTypes{::type_name<Args>()...}
    , m_ArgsValues{}
    , m_CallCount(0)
    {}

template <typename R, typename... Args>
inline Function<R (*)(Args...)>::Function(FuncType fptr, std::string name, BeforeType before, AfterType after)
    : m_Func(fptr)
    , m_Befor(before)
    , m_After(after)
    , m_Name(name)
    , m_ReturnType(::type_name<R>())
    , m_ArgsTypes{::type_name<Args>()...}
    , m_ArgsValues{}
    , m_CallCount(0)
{
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::operator=(const Function &other) -> Function&
{
    if(this != &other){
        this->m_Func = other.m_Func;
        this->m_Befor = other.m_Befor;
        this->m_After = other.m_After;
        this->m_Name = other.m_Name;
        this->m_ReturnType = other.m_ReturnType;
        this->m_ArgsTypes = other.m_ArgsTypes;
        this->m_ArgsValues = other.m_ArgsValues;
        this->m_CallCount = other.m_CallCount;
    }

    return *this;
}

template <typename R, typename... Args>
inline Function<R(*)(Args...)>::Function(const Function &other)
    : m_Func(other.m_Func)
    , m_Befor(other.m_Befor)
    , m_After(other.m_After)
    , m_Name(other.m_Name)
    , m_ReturnType(other.m_ReturnType)
    , m_ArgsTypes(other.m_ArgsTypes)
    , m_ArgsValues(other.m_ArgsValues)
    , m_CallCount(other.m_CallCount)
    {}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::operator=(Function &&other) -> Function&
{
    if(this != &other){
        this->m_Func = std::exchange(other.m_Func, nullptr);
        this->m_Befor = std::exchange(other.m_Befor, nullptr);
        this->m_After = std::exchange(other.m_After, nullptr);
        this->m_Name = std::exchange(other.m_Name, "Function");
        this->m_ReturnType = std::exchange(other.m_ReturnType, ::type_name<R>());
        this->m_ArgsTypes = std::exchange(other.m_ArgsTypes, {::type_name<Args>()...});
        this->m_ArgsValues = std::exchange(other.m_ArgsValues, {});
        this->m_CallCount = std::exchange(other.m_CallCount, 0);
    }

    return *this;
}

template <typename R, typename... Args>
inline Function<R(*)(Args...)>::Function(Function &&other)
    : m_Func(std::exchange(other.m_Func, nullptr))
    , m_Befor(std::exchange(other.m_Befor, nullptr))
    , m_After(std::exchange(other.m_After, nullptr))
    , m_Name(std::exchange(other.m_Name, "Function"))
    , m_ReturnType(std::exchange(other.m_ReturnType, ::type_name<R>()))
    , m_ArgsTypes(std::exchange(other.m_ArgsTypes, {::type_name<Args>()...}))
    , m_ArgsValues(std::exchange(other.m_ArgsValues, {}))
    , m_CallCount(std::exchange(other.m_CallCount, 0))
    {}

template <typename R, typename... Args>
inline auto  Function<R(*)(Args...)>::default_([[maybe_unused]] Args... args) -> R
{
    if constexpr (!std::is_void_v<R>) {
        return R{};
    }
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::operator()(Args... args, std::source_location loc) -> R
{
    if(m_Func == nullptr) [[unlikely]] throw Exception( "{} not loaded!", m_Name);
    m_ArgsValues = std::make_tuple(args...);
    m_CallCount++;

    if(m_Befor) [[unlikely]] m_Befor();

    if constexpr (std::is_void_v<R>) {
        m_Func(args...);
        if(m_After) [[likely]] m_After(function_info(loc));
    } else {
        R result = m_Func(args...);
        if(m_After) [[likely]] m_After(function_info(loc));
        return result;
    }
}

template <typename R, typename... Args>
inline auto Function<R (*)(Args...)>::name() -> std::string
{
    return m_Name;
}

template <typename R, typename... Args>
inline auto Function<R (*)(Args...)>::function() -> FuncType
{
    return m_Func;
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::args_values() const -> std::array<std::string, sizeof...(Args)>
{
    std::array<std::string, sizeof...(Args)> result;
    std::size_t i = 0;
    std::apply([&result, &i](auto&&... args) {
        ((result[i++] = to_string(args)), ...);
    }, m_ArgsValues);
    return result;
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
inline auto Function<R(*)(Args...)>::calls_count() const -> std::size_t
{
    return m_CallCount;
}

template <typename R, typename... Args>
constexpr auto Function<R(*)(Args...)>::args_count() const -> std::size_t
{
    return sizeof...(Args);
}


template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::this_func_sig() const -> std::string {
    std::string result = ::format("{} {}(", m_ReturnType, m_Name);
    format_arguments(result);
    return result + ")";
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::function_info(const std::source_location& loc) -> std::string
{
    return ::format(
        "call Number: {} ; instrments(Befor: {}, After: {}) |>=> {} -> {}:{}\n",
        m_CallCount,
        m_Befor ? "true" : "false", m_After ? "true" : "false",
        this_func_sig(),
        loc.file_name(), loc.line()
    );
}

// in clang ubuntu arm the  `zip` not compiling
// template <typename R, typename... Args>
// auto Function<R(*)(Args...)>::format_arguments(std::string& result) const -> void
// {
//     std::size_t index = 1;
//     bool first = true;
//     for (const auto& [type, value] : std::views::zip(m_ArgsTypes, ArgsValues())) {
//         if (!first) result += ", ";
//         result += ::format("{} arg_{} = {}", type, index++, value);
//         first = false;
//     }
// }

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::format_arguments(std::string& result) const -> void
{
    std::size_t index = 1;
    bool first = true;
    auto argsValues = args_values();

    for (std::size_t i = 0; i < m_ArgsTypes.size(); ++i) {
        if (!first) result += ", ";
        result += ::format("{} arg_{} = {}", m_ArgsTypes[i], index++, argsValues[i]);
        first = false;
    }
}

template <typename R, typename... Args>
inline auto Function<R(*)(Args...)>::to_string(const auto& value) -> std::string
{
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_pointer_v<T>) {
        return utils::pointer_to_string(value);
    } else if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(value);
    } else {
        return std::string(value);
    }
}