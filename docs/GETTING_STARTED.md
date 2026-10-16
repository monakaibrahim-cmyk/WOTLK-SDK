# Getting Started with WotlkSDK

This guide explains how to integrate **WotlkSDK** into your own World of Warcraft 3.3.5a (build 12340) mod or reverse-engineering project.

---

## 1. Project Integration

### Option A: Via CMake Subdirectory (`add_subdirectory`)
Add the SDK repository as a submodule or directory inside your project:

```cmake
# Add WotlkSDK
add_subdirectory(WOTLK-SDK)

# Create your custom mod DLL
add_library(MyMod SHARED
    src/Main.cpp
)

# Link against the SDK
target_link_libraries(MyMod PRIVATE
    WotlkSDK::WotlkSDK
)
```

### Option B: Via `find_package` (Pre-installed)
If you built and installed WotlkSDK via `cmake --install build`:

```cmake
find_package(WotlkSDK CONFIG REQUIRED)

add_library(MyMod SHARED
    src/Main.cpp
)

target_link_libraries(MyMod PRIVATE
    WotlkSDK::WotlkSDK
)
```

---

## 2. Basic Code Structure

In your mod DLL's `DllMain`:

```cpp
#include <WotlkSDK.h>
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // 1. (Optional) Auto-login setup
        WotlkSDK::AutoLogin::Configure("username", "password", "logon.example.com", "Realm", "Character");

        // 2. Initialize SDK
        if (!WotlkSDK::Initialize()) {
            return FALSE;
        }

        // 3. Register your callbacks
        WotlkSDK::ON_UPDATE->add([]() {
            // Per-frame game loop
        });
    }
    return TRUE;
}
```

```

---

## 3. Pure C DLL Integration (`.c`)

If your project is written in pure C (C99/C11/C17/C23) with no C++ compiler or STL:

Include `<WotlkSDK_C.h>`:

```c
#include <WotlkSDK_C.h>
#include <windows.h>

static void on_frame(void) {
    /* Pure C update tick */
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    (void)lpReserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        /* 1. Optional: Auto-login */
        wotlk_autologin_configure("user", "pass", "logon.example.com", "Realm", "Character");

        /* 2. Initialize SDK */
        if (!wotlk_sdk_initialize(5000)) {
            return FALSE;
        }

        /* 3. Register callbacks */
        wotlk_on_update_add(on_frame);
    }
    return TRUE;
}
```

---

## 4. Building

Configure using a 32-bit x86 Visual Studio 2022 generator:

```cmd
cmake -B build -A Win32
cmake --build build --config Release
```
