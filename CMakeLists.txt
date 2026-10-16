cmake_minimum_required(VERSION 3.28)

set(CMAKE_SYSTEM_VERSION 10.0)

project(WotlkSDK LANGUAGES CXX C VERSION 1.0.0)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "WotlkSDK targets 32-bit WoW (3.3.5a). Configure with: cmake -A Win32 <path>")
endif()

if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR 
        (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")))
    message(FATAL_ERROR "Unsupported toolset, WotlkSDK strictly requires MSVC or clang-cl.")
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
    _X86_
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
)

target_compile_options(WotlkSDK_Common INTERFACE
    /utf-8
    /W3
    /wd4458
    /Gw
    /Gd
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