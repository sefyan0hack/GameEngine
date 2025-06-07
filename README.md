# GameEngine

[![CodeCov](https://codecov.io/github/sefyan0hack/GameEngine/graph/badge.svg?token=FXDLJT6XHA)](https://codecov.io/github/sefyan0hack/GameEngine) [![CI](https://github.com/sefyan0hack/GameEngine/actions/workflows/CI.yml/badge.svg)](https://github.com/sefyan0hack/GameEngine/actions/workflows/CI.yml) 
[![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Author](https://img.shields.io/badge/Sofyane%20Bentaleb-orange)]() 
[![Language](https://img.shields.io/badge/language-c++-blue)]() 
[![Standard](https://img.shields.io/badge/std-c++23-blue)]() 
[![MSVC](https://img.shields.io/badge/MSVC-2022+-ff6417)]() 
[![GCC](https://img.shields.io/badge/GCC-14.2+-ff6417)]() 
[![GCC](https://img.shields.io/badge/Clang-18+-ff6417)]() 

**A high-performance C++ game engine**  
Welcome to the **GameEngine** repository! This engine is designed to provide a solid foundation for building games and interactive applications. It leverages modern C++ standards, powerful libraries, and intuitive tools to help you create amazing experiences.

## Features

- **platform compatibility** (Windows/Linux)
- **Win32 API**
- **Modular architecture** for easy extension and customization
- **Input handling** for keyboard, mouse
  
## Getting Started

### Prerequisites

To build and use GameEngine, you’ll need the following:

- **C++ compiler** (e.g., GCC(`14.2`) or Clang(`19`), MSVC(`19.41`))
- **C++23** or later
- **CMake** (version `3.30` or later)
- **Make** ( or ninja)
- **OpenGL** (4.4 comes with GPU Drivers in all the time)
- **GLM** for mathematical operations (all deps in same repo no need for clone or fetch)

### Get The Project 

```bash
$ git clone https://github.com/sefyan0hack/GameEngine.git
$ cd GameEngine

$ mkdir build

$ cmake -S . -B build
$ cmake --build build --config Release
```