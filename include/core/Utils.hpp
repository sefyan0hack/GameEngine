#pragma once

template <typename T>
concept Formatable = requires(T t) {
    std::format("{}", t);
};

template<typename Function, typename... Args>
auto setTimeOut( unsigned long delay, Function&& func, Args&&... args) -> void
{
    std::thread([delay, func = std::forward<Function>(func), ...args = std::forward<Args>(args)]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        std::invoke(func, std::forward<Args>(args)...);
    }).detach();
}

template<typename Function, typename... Args>
auto Repeat( unsigned long interval, Function&& func, Args&&... args) -> void
{
    std::thread([interval, func = std::forward<Function>(func), ...args = std::forward<Args>(args)]() mutable {
        while(true){
            std::invoke(func, std::forward<Args>(args)...);
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));   
        }
    }).detach();
}

// compile time
consteval auto get_file_directory(const std::string_view file_path = __FILE__) -> std::string_view
{
    const size_t last_slash = file_path.find_last_of("/\\");
    return (last_slash == std::string_view::npos) ? "" : file_path.substr(0, last_slash);
}

constexpr auto is_odd(auto x) -> bool
{
    return x % 2 != 0;
}

template <std::size_t N>
struct FixedString {
    constexpr FixedString(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    char value[N];
};

template <FixedString Str>
struct ToUpper {
    static constexpr auto compute() {
        constexpr std::size_t N = sizeof(Str.value);
        std::array<char, N> result{};
        for (std::size_t i = 0; i < N - 1; ++i) {
            char c = Str.value[i];
            if (c >= 'a' && c <= 'z') {
                result[i] = c - ('a' - 'A');
            } else {
                result[i] = c;
            }
        }
        result[N - 1] = '\0';
        return result;
    }
    static constexpr std::array<char, sizeof(Str.value)> value = compute();
};

template <FixedString Str>
constexpr std::array<char, sizeof(Str.value)> ToUpper<Str>::value;

#define TO_UPPER(str) std::string_view(ToUpper<FixedString{str}>::value.data())

inline std::future<std::optional<std::vector<char>>> load_file_async(const std::string& filename) {
    using namespace std;
    return async(launch::async, [filename]() -> optional<vector<char>> {
        ifstream file(filename, ios::binary | ios::ate);
        if (!file) {
            return std::nullopt;
        }
        streamsize size = file.tellg();
        file.seekg(0, ios::beg);

        vector<char> buffer(static_cast<size_t>(size));
        file.read(buffer.data(), size);
        return buffer;
    });
}

inline std::vector<std::string> split(std::string s, const std::string& delimiter) {
    using namespace std;
    if(s.empty()) return {};
    
    vector<string> tokens;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

inline std::string replace(std::string s, char c, char with)
{
    if( c == '\0' || with == '\0')
        return s;
    for(auto& ch: s)
        if(ch == c) 
            ch = with;
    return s;
}

template<class T>
requires std::convertible_to<T, std::string> || Formatable<T>
std::string to_string(const std::vector<T>& vec) {
    std::string result = "[ ";
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i != vec.size() - 1) {
            result += ", ";
        }
    }
    result += " ]";
    return result;
}