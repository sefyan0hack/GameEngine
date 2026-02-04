# GameEngine

[![CodeCov](https://codecov.io/github/sefyan0hack/GameEngine/graph/badge.svg?token=FXDLJT6XHA)](https://codecov.io/github/sefyan0hack/GameEngine) [![CI](https://github.com/sefyan0hack/GameEngine/actions/workflows/CI.yml/badge.svg)](https://github.com/sefyan0hack/GameEngine/actions/workflows/CI.yml) 
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Author](https://img.shields.io/badge/Sofyane%20Bentaleb-orange)]() 
[![lang](https://img.shields.io/badge/language-c++-blue)]() 
[![std](https://img.shields.io/badge/std-c++23-blue)]() 
[![MSVC](https://img.shields.io/badge/MSVC-2022+-ff6417)]() 
[![GCC](https://img.shields.io/badge/GCC-14+-ff6417)]() 
[![Calng](https://img.shields.io/badge/Clang-19+-ff6417)]() 
[![Camke](https://img.shields.io/badge/CMake-3.28+-ff6417)]()

---

## Screenshots

<p align="center">
  <img src="media/Game.png" width="85%" alt="View">
</p>

---

> **GameEngine** — a modern, high-performance foundation for building games and interactive applications with C++23.

## Platforms
- **Windows** (MSVC/GCC/Clang)
- **Linux** (GCC / Clang)
- **Web** (Emscripten)
- **Android** (NDK)

---

## Prerequisites
- C++ compiler with C++23 support (MSVC 2022+, GCC 14+, Clang 19+)
- [CMake](https://cmake.org/) ≥ 3.28
- Optional for targets:
  - Emscripten SDK (for web builds)
  - Android NDK (for Android)
  - OpenGL / Vulkan dev headers for native graphics
- Ninja or Make (recommended: Ninja)

---

## Getting started

Clone the repository and create an out-of-source build directory:

```bash
git clone https://github.com/sefyan0hack/GameEngine.git
cd GameEngine
cmake -S . -B build
cmake --build build --config Release
