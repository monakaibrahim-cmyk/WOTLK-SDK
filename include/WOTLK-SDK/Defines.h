#pragma once

#if !defined(_WIN32) && !defined(_WIN64)
    #error "WotlkSDK strictly targets Windows applications (World of Warcraft 3.3.5a)."
#endif

#if !defined(_M_IX86) && !defined(__i386__) && !defined(_M_X64) && !defined(__x86_64__)
    #error "WotlkSDK targets x86 and x64 architectures."
#endif

#if defined(WOTLK_SDK_BUILD_SHARED) || defined(WOTLK_SDK_SHARED)
    #if defined(WOTLK_SDK_EXPORTS)
        #define WOTLK_SDK_API __declspec(dllexport)
    #else
        #define WOTLK_SDK_API __declspec(dllimport)
    #endif
#else
    #define WOTLK_SDK_API
#endif

#define WOTLK_SDK_NODISCARD [[nodiscard]]
