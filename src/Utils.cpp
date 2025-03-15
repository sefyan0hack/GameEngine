#include <core/Utils.hpp>
#include <core/Log.hpp>

std::future<std::vector<char>> load_file_async(const std::string& filename) {
    return std::async(std::launch::async, [filename]() -> std::vector<char> {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file) {
            Log::Error("Failed to open file {}", filename);
        }
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        file.read(buffer.data(), size);
        return buffer;
    });
}

std::vector<std::string> split(std::string s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

std::string replace(std::string s, char c, char with)
{
    for(auto& ch: s) if(ch == c) ch = with;
    return s;
}