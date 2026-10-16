#pragma once

#include <cstdint>

#define WOTLK_SDK_VERSION_MAJOR 1
#define WOTLK_SDK_VERSION_MINOR 0
#define WOTLK_SDK_VERSION_PATCH 0
#define WOTLK_SDK_VERSION_STRING "1.0.0"

#define WOTLK_SDK_TARGET_WOW_BUILD 12340
#define WOTLK_SDK_TARGET_WOW_VERSION "3.3.5a"

namespace WotlkSDK {

inline constexpr uint32_t VERSION_MAJOR = WOTLK_SDK_VERSION_MAJOR;
inline constexpr uint32_t VERSION_MINOR = WOTLK_SDK_VERSION_MINOR;
inline constexpr uint32_t VERSION_PATCH = WOTLK_SDK_VERSION_PATCH;
inline constexpr const char* VERSION_STRING = WOTLK_SDK_VERSION_STRING;

inline constexpr uint32_t TARGET_WOW_BUILD = WOTLK_SDK_TARGET_WOW_BUILD;
inline constexpr const char* TARGET_WOW_VERSION = WOTLK_SDK_TARGET_WOW_VERSION;

}  // namespace WotlkSDK
