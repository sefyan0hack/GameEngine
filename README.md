# GameEngine

**A high-performance C++ game engine**  
Welcome to the **GameEngine** repository! This engine is designed to provide a solid foundation for building games and interactive applications. It leverages modern C++ standards, powerful libraries, and intuitive tools to help you create amazing experiences.

## Features

- **platform compatibility** (Windows)
- **Win32 API**
- **Modular architecture** for easy extension and customization
- **Input handling** for keyboard, mouse
  
## Getting Started

### Prerequisites

To build and use GameEngine, you’ll need the following:

- **C++ compiler** (e.g., GCC(14), Clang, MSVC(19))
- **C++23** or later
- **CMake** (version 3.25 or later)
- **Make** ( or ninja)
- **OpenGL** (comes with GPU Drivers in all the time)
- **GLM** for mathematical operations (all deps in same repo no need for clone or fetch)

### Get The Project 

```bash
git clone https://github.com/sefyan0hack/GameEngine.git
cd GameEngine
   
mkdir build
cd build

cmake ..
cmake --build . --config Release

