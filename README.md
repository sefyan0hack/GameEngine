# GameEngine

[![CodeCov](https://codecov.io/github/sefyan0hack/GameEngine/graph/badge.svg?token=FXDLJT6XHA)](https://codecov.io/github/sefyan0hack/GameEngine) [![CI](https://github.com/sefyan0hack/GameEngine/actions/workflows/CI.yml/badge.svg)](https://github.com/sefyan0hack/GameEngine/actions/workflows/CI.yml) 
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![lang](https://img.shields.io/badge/language-c++-blue)]() 
[![std](https://img.shields.io/badge/std-c++23-blue)]() 
[![MSVC](https://img.shields.io/badge/MSVC-2022+-ff6417)]() 
[![GCC](https://img.shields.io/badge/GCC-14+-ff6417)]() 
[![Calng](https://img.shields.io/badge/Clang-19+-ff6417)]() 
[![Camke](https://img.shields.io/badge/CMake-3.28+-ff6417)]()

> **GameEngine** — a modern, high-performance foundation for building games and interactive applications with C++23.

---
## Screenshots

<p align="center">
  <img src="media/Game.png" width="100%" alt="View">
</p>

## Platforms
- **Windows** (MSVC/GCC/Clang)
- **Linux** (GCC / Clang)
- **Web** (Emscripten)
- **Android** (NDK)

---

## Prerequisites
- C++ compiler with C++23 support (MSVC 19.44+, GCC 15+, Clang 20+)
- [CMake](https://cmake.org/) ≥ 3.28
- Optional for targets:
  - Emscripten SDK (for web builds)
  - Android NDK (for Android)
  - OpenGL / Vulkan dev headers for native graphics
- Ninja or Make (recommended: Ninja)

---
## Getting started

1. Clone the repository

  ```bash
  git clone --recurse-submodules https://github.com/sefyan0hack/GameEngine.git
  cd GameEngine
  ```

2. Configure Project

  2.1 Windows & Linux :

  ```bash
  cmake -S . -B build
  ```

  2.2 Android :

  ```bash
  #Windows
  set ANDROID_NDK_LATEST_HOME=C:/path/to/android_ndk_rXX
  set ANDROID_SDK_ROOT=C:/path/to/android_sdk
  cmake -S . -B build ^
    -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK_LATEST_HOME%/build/cmake/android.toolchain.cmake ^
    -DANDROID_ABI="arm64-v8a"
  ```

  ```bash
  #Linux
  export ANDROID_NDK_LATEST_HOME=/path/to/android_ndk_rXX
  export ANDROID_SDK_ROOT=/path/to/android_sdk
  cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_LATEST_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI="arm64-v8a"
  ```

  2.3 Web :

  ```bash
  emcmake cmake -S . -B build
  ```

3. Build Project

  ```bash
  cmake --build build --config Release
  ```