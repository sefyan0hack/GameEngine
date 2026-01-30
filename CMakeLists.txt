cmake_minimum_required(VERSION 3.28)

project(
    GameEngine
    LANGUAGES C CXX
    VERSION 0.0.0
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_USE_RELATIVE_PATHS true)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CMakePrintSystemInformation)
include(CTest)
include(FeatureSummary)
include(InstaledPackages)
include(helpers)
include(Options)
include(LinkTimeOpt)
include(Emscripten)
include(RTTI)
include(Docs)
include(Harden)
include(StaticLink)
include(Coverage)
include(Sanitize)
include(CompilerFlags)

if(WIN32)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    if(CONSOLE_ATTACHED)
        set(CMAKE_WIN32_EXECUTABLE OFF)
    else()
        set(CMAKE_WIN32_EXECUTABLE ON)
    endif()

    set(DIR_PLT_NAME "windows")
elseif(EMSCRIPTEN)
    set(DIR_PLT_NAME "web")
elseif(ANDROID)
    set(DIR_PLT_NAME "android")
elseif(LINUX)
    set(DIR_PLT_NAME "linux")
endif()

set(PLT_DIR_PATH_SUFFIX "platform/${DIR_PLT_NAME}")

####################################################################################################
# Vsisibility
####################################################################################################

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

####################################################################################################
# Position Independent Code
####################################################################################################

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

####################################################################################################
# Check if We Want Fetch or include Deps
####################################################################################################

if(EXISTS ${CMAKE_SOURCE_DIR}/3party)
    set(ONLINE FALSE)
else()
    set(ONLINE TRUE)
endif()

if(ONLINE)
    include(FetchDeps)
else()
    include(IncludeDeps)
endif()

####################################################################################################
# Resources
####################################################################################################

include(CMakeRC)

file(GLOB_RECURSE RES_FILES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/res/*"
)

cmrc_add_resource_library( resources
    ALIAS res
    NAMESPACE res

    ${RES_FILES}
)

set_target_properties(resources PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

####################################################################################################
# Global defines 
####################################################################################################

add_compile_definitions(
    SYSTEM_HOST_NAME="${CMAKE_HOST_SYSTEM_NAME}"
    SYSTEM_HOST_VERSION="${CMAKE_HOST_SYSTEM_VERSION}"
    SYSTEM_HOST_ARCH="${CMAKE_HOST_SYSTEM_PROCESSOR}"

    COMPILER_NAME="${CMAKE_CXX_COMPILER_ID}"
    COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"

    LINKER_NAME="${CMAKE_CXX_COMPILER_LINKER_ID}"
    LINKER_VERSION="${CMAKE_CXX_COMPILER_LINKER_VERSION}"

    CXX_STANDARD="${CMAKE_CXX_STANDARD}"

    OPENGL_LIB="${CORE_OPENGL_LIB}"

    DYN_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
    DYN_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"

    $<$<BOOL:${GAME_HOT_RELOADABLE}>:GAME_HOT_RELOADABLE>
    $<$<BOOL:${CONSOLE_ATTACHED}>:CONSOLE_ATTACHED>
    $<$<BOOL:${ROBUST_GL_CHECK}>:ROBUST_GL_CHECK>

    $<$<PLATFORM_ID:Windows>:WINDOWS_PLT>
    $<$<PLATFORM_ID:Unix>:UNIX_PLT>
    $<$<PLATFORM_ID:Linux>:LINUX_PLT>
    $<$<PLATFORM_ID:Emscripten>:WEB_PLT>
    $<$<PLATFORM_ID:Android>:ANDROID_PLT>

    $<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:Linux>>:CORE_GL>
    $<$<OR:$<PLATFORM_ID:Android>,$<PLATFORM_ID:Emscripten>>:ES_GL>

    $<$<CXX_COMPILER_ID:GNU>:GNU_CPL>
    $<$<CXX_COMPILER_ID:Clang>:CLANG_CPL>
    $<$<CXX_COMPILER_ID:MSVC>:MSVC_CPL>

    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

####################################################################################################
# Sub Dirs
####################################################################################################

add_subdirectory(Engine)
add_subdirectory(Game)

####################################################################################################
# TEST
####################################################################################################

if(WITH_TESTS)
    add_subdirectory(tests/unit)
endif()

