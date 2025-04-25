# GameEngine

**A high-performance C++ game engine**  
Welcome to the **GameEngine** repository! This engine is designed to provide a solid foundation for building games and interactive applications. It leverages modern C++ standards, powerful libraries, and intuitive tools to help you create amazing experiences.

### CDash
https://my.cdash.org/index.php?project=GameEngine

|             | **GCC**                                                                 | **MSVC**                                                                  | **Clang**                                                               |
|-----------|:------------------------------------------------------------------------:|:-------------------------------------------------------------------------:|:------------------------------------------------------------------------:|
| **Windows** | [![Build Windows (GCC)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_Mingw.yml/badge.svg?event=push)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_Mingw.yml) | [![Build Windows (MSVC)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_work.yml/badge.svg?event=push)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_work.yml) | –                                                                      |
| **Linux**   | [![Build Linux (GCC)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_gcc_linux.yml/badge.svg?event=push)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_gcc_linux.yml) | –                                                                         | [![Build Linux (Clang)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_clang_linux.yml/badge.svg?event=push)](https://github.com/sefyan0hack/GameEngine/actions/workflows/cpp_clang_linux.yml) |



## Features

- **platform compatibility** (Windows)
- **Win32 API**
- **Modular architecture** for easy extension and customization
- **Input handling** for keyboard, mouse
  
## Getting Started

### Prerequisites

To build and use GameEngine, you’ll need the following:

- **C++ compiler** (e.g., GCC(`14.2`) or Clang, MSVC(`19.41`))
- **C++23** or later
- **CMake** (version `3.30` or later)
- **Make** ( or ninja)
- **OpenGL** (comes with GPU Drivers in all the time)
- **GLM** for mathematical operations (all deps in same repo no need for clone or fetch)

### Get The Project 

#### if you Want Auto fetch debs [`Good Connection`] :
```bash
$ git clone https://github.com/sefyan0hack/GameEngine.git
$ cd GameEngine
   
$ mkdir build
$ cd build

$ cmake ..
$ cmake --build . --config Release
```
#### else
and make `3party` side by side to `src` and download files need  as shown below
```
$ mkdir 3party
$ tree
3party
│   glcorearb.h // https://registry.khronos.org/OpenGL/api/GL/glcorearb.h
│   stb_image.h // https://raw.githubusercontent.com/nothings/stb/5c205738c191bcb0abc65c4febfa9bd25ff35234/stb_image.h
├───glm // https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.zip
└───KHR
        khrplatform.h // https://raw.githubusercontent.com/KhronosGroup/EGL-Registry/main/api/KHR/khrplatform.h
$ git clone https://github.com/sefyan0hack/GameEngine.git
$ cd GameEngine
   
$ mkdir build
$ cd build

$ cmake ..
$ cmake --build . --config Release
```