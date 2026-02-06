cmake_minimum_required(VERSION 3.28)

project(
    GameEngine
    DESCRIPTION "Cross Platform C++ Game Engine"
    LANGUAGES C CXX
    VERSION 0.0.0
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CMakePrintSystemInformation)
include(FeatureSummary)
include(InstaledPackages)
include(helpers)
include(Options)
include(LinkTimeOpt)
include(Emscripten)
include(Docs)
include(Harden)
include(Coverage)
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

set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

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

####################################################################################################
# Sub Dirs
####################################################################################################

add_subdirectory(Engine)
add_subdirectory(Game)

####################################################################################################
# TEST
####################################################################################################

if(WITH_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests/unit)
endif()

if(FUZZ_TEST)
    add_subdirectory(tests/fuzz)
endif()

