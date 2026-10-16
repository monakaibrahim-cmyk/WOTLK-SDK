cmake_minimum_required(VERSION 3.28)

set(CMAKE_SYSTEM_VERSION 10.0)

project(WotlkSDK LANGUAGES CXX C VERSION 1.0.0)

# Architecture Configuration (Support both 32-bit Win32 and 64-bit x64)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "WotlkSDK target architecture: x64 (64-bit companion / external tools).")
    set(WOTLK_SDK_ARCH "x64")
else()
    message(STATUS "WotlkSDK target architecture: x86 / Win32 (32-bit native WoW 3.3.5a client).")
    set(WOTLK_SDK_ARCH "x86")
endif()

# Compiler Enforcement: Strictly require MSVC or clang-cl (reject GCC/MinGW)
if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR 
        (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")))
    message(FATAL_ERROR 
        "Unsupported toolset: '${CMAKE_CXX_COMPILER_ID}'.\n"
        "WotlkSDK strictly requires Microsoft Visual C++ (MSVC / cl.exe) or clang-cl.\n"
        "MinGW / GCC are NOT supported due to MSVC ABI incompatibility, struct packing,\n"
        "__thiscall convention, and Discord Social SDK (discord-game-sdk) linkage requirements.\n"
        "Please build using Visual Studio 2022 (v143) or CMake Presets (e.g. cmake --preset x86-msvc-release)."
    )
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if (ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
endif()

# Build Options
option(BUILD_SHARED_LIBS "Build WotlkSDK as a shared library (.dll) instead of static (.lib)" OFF)
option(WOTLK_SDK_BUILD_EXAMPLES "Build WotlkSDK sample projects" ON)
option(WOTLK_SDK_DISCORD_SOCIAL_SDK "Enable Discord Social SDK (discord-game-sdk) integration via vcpkg" ON)

# Discord Social SDK Integration (via vcpkg)
if (WOTLK_SDK_DISCORD_SOCIAL_SDK)
    find_path(DISCORD_GAME_SDK_INCLUDE_DIR
        NAMES "discord.h" "discord-game-sdk/discord.h"
        PATH_SUFFIXES "discord-game-sdk" "include/discord-game-sdk"
    )

    find_library(DISCORD_GAME_SDK_LIB
        NAMES discord_game_sdk discord_game_sdk.dll.lib
        PATH_SUFFIXES lib
    )

    find_library(DISCORD_GAME_SDK_CPP_LIB
        NAMES discord_game_sdk_cpp
        PATH_SUFFIXES lib
    )

    if (DISCORD_GAME_SDK_INCLUDE_DIR AND DISCORD_GAME_SDK_LIB)
        message(STATUS "Discord Social SDK (Game SDK) detected via vcpkg:")
        message(STATUS "  Include: ${DISCORD_GAME_SDK_INCLUDE_DIR}")
        message(STATUS "  Library: ${DISCORD_GAME_SDK_LIB}")
        set(WOTLK_SDK_HAVE_DISCORD_GAME_SDK TRUE CACHE INTERNAL "Discord Social SDK found")
    else()
        message(STATUS "Discord Social SDK not found in vcpkg; using native zero-dependency Windows Named Pipe IPC backend.")
        set(WOTLK_SDK_HAVE_DISCORD_GAME_SDK FALSE CACHE INTERNAL "Discord Social SDK found")
    endif()
endif()

add_subdirectory(deps)
add_subdirectory(tools)

add_library(WotlkSDK_Common INTERFACE)

target_include_directories(WotlkSDK_Common INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/tools"
)

target_compile_definitions(WotlkSDK_Common INTERFACE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:_X86_>
    $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:_AMD64_>
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
)

target_compile_options(WotlkSDK_Common INTERFACE
    /utf-8
    /W3
    /wd4458
    /Gw
    $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:/Gd>
)

target_link_libraries(WotlkSDK_Common INTERFACE
    Hookkit
    unordered_dense::unordered_dense
)

add_subdirectory(src)

if (WOTLK_SDK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# CMake Package Export & Installation
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(WOTLK_SDK_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/WotlkSDK")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/WotlkSDKConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    EXPORT WotlkSDKTargets
    FILE WotlkSDKTargets.cmake
    NAMESPACE WotlkSDK::
    DESTINATION "${WOTLK_SDK_CMAKE_DIR}"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/WotlkSDKConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/WotlkSDKConfig.cmake"
    INSTALL_DESTINATION "${WOTLK_SDK_CMAKE_DIR}"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/WotlkSDKConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/WotlkSDKConfigVersion.cmake"
    DESTINATION "${WOTLK_SDK_CMAKE_DIR}"
)