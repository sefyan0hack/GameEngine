#pragma once

template <typename T>
concept Pointer = std::is_pointer_v<T>;

template<typename T>
concept FunctionPointer =
    std::is_pointer_v<std::remove_cv_t<T>> &&
    std::is_function_v<std::remove_pointer_t<std::remove_cv_t<T>>>;


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
        streamsize size = static_cast<std::streamsize>(file.tellg());
        file.seekg(0, ios::beg);
        
        if (static_cast<size_t>(size) > vector<char>().max_size()){
            return nullopt;
        }

        vector<char> buffer(static_cast<size_t>(size));
        file.read(buffer.data(), size);
        return buffer;
    });
}

inline std::vector<std::string> split(const char* cstr, const std::string& delimiter) {
    using namespace std;
    if (cstr == nullptr) return {};
    string s(cstr);
    
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
requires std::convertible_to<T, std::string> || std::formattable<T, char>
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

inline auto getenv_(const char* name) -> std::string
{
    #if defined(MSVC_CPL)
    char* buffer = nullptr;
    size_t len = 0;
    if (_dupenv_s(&buffer, &len, name) != 0 || buffer == nullptr) {
        if (buffer) free(buffer);
        return {};
    }
    std::string value(buffer, len);
    free(buffer);
    return value;

    #else

    auto env = std::getenv(name);
    return env ? std::string(env) : "";

    #endif
}

inline std::string strerror_()
{
    char buf[256] = {0};
    #if defined(LINUX_PLT) || defined(WEB_PLT)
    strerror_r(errno, buf, sizeof(buf));
    #else
    strerror_s(buf, sizeof(buf), errno);
    #endif
    return buf;
}

inline auto file_to_str(std::ifstream& file) -> std::optional<std::string>
{
    if (!file){ 
        return std::nullopt;
    }else{
        return std::string(std::istreambuf_iterator<char>(file), {});
    }
}

inline auto file_to_str(const char* path) -> std::optional<std::string>
{
    std::ifstream file(path, std::ios::binary);

    if (!file){ 
        return std::nullopt;
    }else{
        return std::string(std::istreambuf_iterator<char>(file), {});
    }
}


template <class T>
inline auto to_hex(const T& data) -> std::string
{
    return to_hex(data, 1);
}

template <class T>
inline auto to_hex(T* data) -> std::string
{
    return to_hex(data, 1);
}

template <class T, size_t N>
inline auto to_hex(const T (&data)[N]) -> std::string
{
    return to_hex(data, N);
}

template <class T>
inline auto to_hex(const T* data, size_t n) -> std::string
{
    auto result = std::stringstream{};
    auto data_as_byte = reinterpret_cast<const unsigned char*>(data);
    size_t total_bytes = n * sizeof(T);

    for (size_t i = 0; i < total_bytes; ++i) {
        unsigned char byte = data_as_byte[i];
        result << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(byte) << ' ';
       if ((i + 1) % 8 == 0) result << "  ";
    }
    return result.str();
}

auto pointer_to_string(Pointer auto ptr) -> std::string
{
    using Pointee = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

    if (ptr == nullptr) return "null";
    if constexpr (std::is_constructible_v<std::string, decltype(ptr)>) return std::string(ptr);

    if constexpr (std::is_function_v<Pointee>) {
        return ::type_name<Pointee>();
    } else if constexpr (std::is_arithmetic_v<Pointee>) {
        return std::to_string(*ptr);
    } else if constexpr (std::formattable<Pointee, char>) {
        return std::format("{}", *ptr);
    } else if constexpr (std::is_same_v<Pointee, void>) {
        return std::format("<{:p}> : void*", static_cast<const void*>(ptr));
    } else {
        if constexpr (std::is_pointer_v<Pointee>) return pointer_to_string(*ptr);
        return std::format("<{:p}> ({}) : [ {} ]", static_cast<const void*>(ptr), sizeof(Pointee), to_hex(ptr));
    }
}