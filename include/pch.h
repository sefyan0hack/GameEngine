#pragma once

#include <version>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#ifdef __cpp_lib_stacktrace
#   include <stacktrace>
#endif
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
#ifdef __cpp_lib_format
#   include <format>
#else
#error "This project uses <format>, and it need's modern compiler with -std=c++23"
#endif
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
