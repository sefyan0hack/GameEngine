#pragma once
#include <version>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <csignal>
#include <cstdint>
#include <iomanip>
#include <cstring>
#include <filesystem>
#include <thread>
#include <vector>
#include <ranges>
#include <atomic>
#include <cstddef>
#include <utility>
#include <functional>
#include <array>
#include <source_location>
#include <tuple>
#include <queue>
#include <bitset>
#include <optional>
#include <chrono>
#include <string_view>
#include <cstdlib>
#include <mutex>
#include <unordered_map>
#include <map>
#include <initializer_list>
#include <ctime>
#include <future>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <typeinfo>
#include <concepts>
#include <variant>
#include <any>

#ifdef __cpp_lib_stacktrace
#   include <stacktrace>
#else
#warning "there is no <stacktrace>"
#endif

#if defined(__cpp_lib_format)
#   include <format>
#else
#error "This project uses <format>, and it need's modern compiler with -std=c++23"
#endif