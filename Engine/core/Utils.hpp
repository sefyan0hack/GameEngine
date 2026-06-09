#pragma once
#include <string>

namespace utils {

/**
 * @brief Read an entire file into a std::string.
 *
 * @param path File system path to the file.
 * @return std::string The file contents.
 *
 * @throws Exception If the file cannot be opened. The exception includes
 */
auto read_file(const char* path) -> std::string;
auto read_file(std::string path) -> std::string;

}// namespace utils
