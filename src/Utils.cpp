#include <core/Utils.hpp>

std::future<std::vector<char>> load_file_async(const std::string& filename) {
    return std::async(std::launch::async, [filename]() -> std::vector<char> {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file) {
            throw std::runtime_error("Failed to open file");
        }
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        file.read(buffer.data(), size);
        return buffer;
    });
}