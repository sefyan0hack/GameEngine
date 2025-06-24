#pragma once
#include <core/Log.hpp>
#include <core/Utils.hpp>

template <typename T>
class Function;

template <typename R, typename... Args>
class Function<R(APIENTRY*)(Args...)> {
public:
    using FuncType = R(APIENTRY*)(Args...);
    using BeforType = void(*)(void);
    using AfterType = void(*)(std::string);

    Function();
    auto operator= (Function&& other) -> Function&;
    auto operator= (const Function& other) -> Function& = delete;
    Function(Function&& other) = delete;
    Function(const Function& other) = delete;

    auto operator()(Args... args, std::source_location loc = std::source_location::current()) -> R;

    static auto APIENTRY default_([[maybe_unused]] Args... args) -> R;
    
    auto ReturnType() const -> std::string;
    auto ArgsValues() const -> std::array<std::string, sizeof...(Args)>;
    auto ArgsTypes() const  -> std::array<std::string, sizeof...(Args)>;
    constexpr auto ArgsCount() const -> std::size_t;
    auto CallsCount() const -> std::size_t;

    static auto functionCount() -> std::size_t;
    
private:
    auto this_func_sig() const -> std::string ;

    auto function_info(const std::source_location& loc) -> std::string;
    auto format_arguments(std::string& result) const -> void;

    static auto to_string(const auto& value) -> std::string ;

public:
    FuncType m_Func;
    BeforType m_Befor;
    AfterType m_After;
    std::string m_Name;
private:
    std::string m_ReturnType;
    std::array<std::string, sizeof...(Args)> m_ArgsTypes;
    std::tuple<Args...> m_ArgsValues;
    std::size_t m_CallCount;
    inline static std::size_t m_Count = 0;

    FOR_TEST
};

// impl

template <typename R, typename... Args>
Function<R(APIENTRY*)(Args...)>::Function()
    : m_Func(&default_)
    , m_Befor(nullptr)
    , m_After(nullptr)
    , m_Name("Function")
    , m_ReturnType(::type_name<R>())
    , m_ArgsTypes{::type_name<Args>()...}
    , m_ArgsValues{}
    , m_CallCount(0)
    { 
        m_Count++;
    }

template <typename R, typename... Args>
auto Function<R(APIENTRY *)(Args...)>::operator=(Function &&other) -> Function &
{
    this->m_Func = std::exchange(other.m_Func, nullptr);
    this->m_Befor = std::exchange(other.m_Befor, nullptr);
    this->m_After = std::exchange(other.m_After, nullptr);
    this->m_Name = std::exchange(other.m_Name, std::string());
    this->m_ReturnType = std::exchange(other.m_ReturnType, std::string());
    this->m_ArgsTypes = std::exchange(other.m_ArgsTypes, {});
    this->m_ArgsValues = std::exchange(other.m_ArgsValues, {});
    this->m_CallCount = std::exchange(other.m_CallCount, 0);
    
    return *this;
}

template <typename R, typename... Args>
auto APIENTRY Function<R(APIENTRY*)(Args...)>::default_([[maybe_unused]] Args... args) -> R
{
    if constexpr (!std::is_void_v<R>) {
        return R{};
    }
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::operator()(Args... args, std::source_location loc) -> R
{
    if(m_Func == nullptr) Error( "{} not loaded!", m_Name);
    m_ArgsValues = std::make_tuple(args...);
    m_CallCount++;

    if(m_Befor) m_Befor();

    if constexpr (std::is_void_v<R>) {
        m_Func(args...);
        if(m_After) m_After(function_info(loc));
    } else {
        R result = m_Func(args...);
        if(m_After) m_After(function_info(loc));
        return result;
    }
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::ArgsValues() const -> std::array<std::string, sizeof...(Args)>
{
    std::array<std::string, sizeof...(Args)> result;
    std::size_t i = 0;
    std::apply([&result, &i](auto&&... args) {
        ((result[i++] = to_string(args)), ...);
    }, m_ArgsValues);
    return result;
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::ReturnType() const -> std::string
{
    return m_ReturnType;
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::ArgsTypes() const -> std::array<std::string, sizeof...(Args)>
{
    return m_ArgsTypes;
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::CallsCount() const -> std::size_t
{
    return m_CallCount;
}

template <typename R, typename... Args>
constexpr auto Function<R(APIENTRY*)(Args...)>::ArgsCount() const -> std::size_t
{
    return sizeof...(Args);
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::functionCount() -> std::size_t
{
    return m_Count;
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::this_func_sig() const -> std::string {
    std::string result = std::format("{} {}(", m_ReturnType, m_Name);
    format_arguments(result);
    return result + ")";
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::function_info(const std::source_location& loc) -> std::string
{
    return std::format(
        "call Number: {} ; instrments(Befor: {}, After: {}) |>=> {} -> {}:{}\n",
        m_CallCount,
        m_Befor ? "true" : "false", m_After ? "true" : "false",
        this_func_sig(),
        loc.file_name(), loc.line()
    );
}

// in clang ubuntu arm the  `zip` not compiling
// template <typename R, typename... Args>
// auto Function<R(APIENTRY*)(Args...)>::format_arguments(std::string& result) const -> void
// {
//     std::size_t index = 1;
//     bool first = true;
//     for (const auto& [type, value] : std::views::zip(m_ArgsTypes, ArgsValues())) {
//         if (!first) result += ", ";
//         result += std::format("{} arg_{} = {}", type, index++, value);
//         first = false;
//     }
// }

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::format_arguments(std::string& result) const -> void
{
    std::size_t index = 1;
    bool first = true;
    auto argsValues = ArgsValues();

    for (std::size_t i = 0; i < m_ArgsTypes.size(); ++i) {
        if (!first) result += ", ";
        result += std::format("{} arg_{} = {}", m_ArgsTypes[i], index++, argsValues[i]);
        first = false;
    }
}

template <typename R, typename... Args>
auto Function<R(APIENTRY*)(Args...)>::to_string(const auto& value) -> std::string
{
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_pointer_v<T>) {
        return pointer_to_string(value);
    } else if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(value);
    } else {
        return std::string(value);
    }
}