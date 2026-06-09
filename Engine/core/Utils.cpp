#include "Utils.hpp"
#include "Exception.hpp"

#include <fstream>

namespace utils
{

auto read_file(const char* path) -> std::string
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    
    if (!file) {
        throw Exception("Couldn't open file [{}] : errno {}", path, errno);
    }
    
    auto size = file.tellg();
    if (size <= 0) {
        return "";
    }
    
    file.seekg(0, std::ios::beg);
    std::string content;
    content.resize(static_cast<size_t>(size));
    
    if (!file.read(content.data(), static_cast<std::streamsize>(size))) {
        throw Exception("Failed to read file [{}]", path);
    }
    
    return content;
}

auto read_file(std::string path) -> std::string
{
    return read_file(path.c_str());
}

}