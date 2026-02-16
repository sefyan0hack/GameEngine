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

include(helpers)
include(InstaledPackages)
include(CompilerFlags)
include(Options)
include(LTO)
include(Harden)
include(Coverage)

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
endif()

####################################################################################################
# Static link std lib
####################################################################################################

add_link_options(
    $<$<CXX_COMPILER_ID:GNU>:-static-libstdc++>
)

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

if(FUZZ_TESTS)
    enable_testing()
    add_subdirectory(tests/fuzz)
endif()

message(STATUS "---------------------------[Host System Info]----------------------------------")
function(print_sys_info key)
    cmake_host_system_information(RESULT ${key} QUERY ${key})
    message(STATUS "${key} : ${${key}}")
endfunction()

print_sys_info(NUMBER_OF_LOGICAL_CORES)
print_sys_info(NUMBER_OF_PHYSICAL_CORES)
print_sys_info(HOSTNAME)
print_sys_info(FQDN)
print_sys_info(TOTAL_VIRTUAL_MEMORY)
print_sys_info(AVAILABLE_VIRTUAL_MEMORY)
print_sys_info(TOTAL_PHYSICAL_MEMORY)
print_sys_info(AVAILABLE_PHYSICAL_MEMORY)
print_sys_info(IS_64BIT)
print_sys_info(HAS_FPU)
print_sys_info(HAS_MMX)
print_sys_info(HAS_MMX_PLUS)
print_sys_info(HAS_SSE)
print_sys_info(HAS_SSE2)
print_sys_info(HAS_SSE_FP)
print_sys_info(HAS_SSE_MMX)
print_sys_info(HAS_AMD_3DNOW)
print_sys_info(HAS_AMD_3DNOW_PLUS)
print_sys_info(HAS_IA64)
print_sys_info(HAS_SERIAL_NUMBER)
print_sys_info(PROCESSOR_SERIAL_NUMBER)
print_sys_info(PROCESSOR_NAME)
print_sys_info(PROCESSOR_DESCRIPTION)
print_sys_info(OS_NAME)
print_sys_info(OS_RELEASE)
print_sys_info(OS_VERSION)
print_sys_info(OS_PLATFORM)
print_sys_info(DISTRIB_INFO)

message(STATUS "--------------------------------------------------------------------------------")
