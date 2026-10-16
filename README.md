# WotlkSDK
### World of Warcraft 3.3.5a (Build 12340) Software Development Kit

WotlkSDK is a high-performance C++23 development kit and base library designed for World of Warcraft 3.3.5a (build 12340). It provides reverse-engineered engine offsets, typed subsystems, an atomic hooking framework, console and event registries, a configurable auto-login engine, and official **Discord Social SDK** integration via **vcpkg** with a zero-dependency named pipe fallback.

Designed for professional mod authors and reverse engineers, WotlkSDK can be statically linked directly into custom mods/DLLs (eliminating third-party runtime dependencies) or built as a standalone injector-ready DLL. Both modern C++23 and native Pure C (C99/C11/C17/C23) ABIs are supported out of the box.

---

## Toolchain Requirement: MSVC (No MinGW64)

> [!IMPORTANT]
> **WotlkSDK strictly requires Microsoft Visual C++ (MSVC / cl.exe) or clang-cl.**
> MinGW / GCC / MinGW64 are **explicitly unsupported** for the following architectural reasons:
> 1. **Engine ABI Fidelity**: World of Warcraft 3.3.5a was compiled using MSVC. Engine virtual tables, `__thiscall` conventions, structure padding, and SEH exception handling match MSVC calling conventions.
> 2. **Discord Social SDK Binary Compatibility**: The official Discord Social SDK (`discord-game-sdk`) ships precompiled `.lib` and `.dll` binaries targeting MSVC. MinGW cannot link these libraries due to symbol decoration and CRT runtime mismatches.
> 3. **vcpkg Toolchain Integration**: vcpkg triplets (`x86-windows`, `x64-windows`) natively integrate with the MSVC toolchain and Visual Studio.

---

## Architecture & API Design

### 1. Unified Headers
The SDK provides dedicated, cleanly separated entrypoints for C++ and pure C consumers:

```cpp
// Modern C++23:
#include <WOTLK-SDK/wotlkcpp.h>

// Native Pure C:
#include <WOTLK-SDK/wotlkc.h>
```

### 2. Static Library (`.lib`) vs Shared DLL (`.dll`)
* **Static Library (`WotlkSDK.lib`, default & recommended)**:
  - Best for custom mods, hook DLLs, and injection loaders.
  - Links directly into your consumer DLL/executable without shipping external runtime libraries.
  - Exposes `WotlkSDK::Initialize()`, `WotlkSDK::IsInitialized()`, and `WotlkSDK::Shutdown()`.
  - Does not define `DllMain`, preventing entry point collisions with consumer modules.
* **Shared Library (`WotlkSDK.dll`)**:
  - Built by passing `-DBUILD_SHARED_LIBS=ON` to CMake.
  - Automatically initializes on `DLL_PROCESS_ATTACH` and shuts down on `DLL_PROCESS_DETACH` (ideal for standalone injection).

### 3. Naming Conventions
* **Namespaces, Classes, and Structs**: formatted in `UpperCamelCase` (e.g., `WotlkSDK`, `ObjectMgr`, `NetClient`, `GameUI`, `Login`, `Storm`, `Lua`, `ClientDB`, `AutoLogin`, `Extensions`, `Discord`, `HookKit`).
* **Variables, Registries, and Callbacks**: accessible via `SCREAMING_SNAKE_CASE` (e.g., `CVAR_REGISTRY`, `EVENT_REGISTRY`, `LUA_LIB_REGISTRY`, `TOKEN_REGISTRY`, `ON_UPDATE`, `ON_ENTER`, `ON_LEAVE`, `POST_LOAD`, `CHAR_ENUM`).
* **Typed Module APIs**: organized by domain in `SCREAMING_SNAKE_CASE` (e.g., `PLAYER::GET_UID()`, `SPELL::GET_ID()`, `GAME::IS_IN_WORLD()`, `ZONE::GET_ZONE_TEXT()`, `PRESENCE::GET_DATA()`, `DISCORD::START()`, `DISCORD::GET_CORE()`).

---

## Quick Start Examples

### C++ Mod Example (`Main.cpp`)

```cpp
#define DISCORD_RICH_PRESENCE
#define DISCORD_CLIENT_ID "123456789012345678"
#include <WOTLK-SDK/wotlkcpp.h>
#include <windows.h>

// Custom Discord Rich Presence formatter using C++ template override
struct MyPresenceFormatter {
    static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, DISCORD::Activity& out) {
        DISCORD::DefaultFormatter::Format(in, out);
        out.details = std::format("[Guild Raid] {}", out.details);
    }
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // 1. (Optional) Configure auto-login credentials
        WotlkSDK::AutoLogin::Configure("username", "password", "logon.example.com", "Lordaeron", "MyCharacter");

        // 2. Initialize SDK and install atomic engine hooks
        if (!WotlkSDK::Initialize()) {
            return FALSE;
        }

        // 3. Register engine render tick and world event listeners
        WotlkSDK::ON_UPDATE->add([]() {
            if (GAME::IS_IN_WORLD()) {
                guid_t targetGuid = PLAYER::GET_TARGET_UID();
                (void)targetGuid;
            }
        });

        // 4. Register a custom console CVar
        WotlkSDK::CVAR_REGISTRY->add<int>({
            .name = "my_custom_mod",
            .desc = "Toggles custom mod functionality (1=on, 0=off)",
            .init = 1,
            .on_change = [](int val, bool changed) {
                // React to runtime CVar updates
            }
        });

        // 5. Install custom Discord presence template formatter
        DISCORD::SET_FORMATTER<MyPresenceFormatter>();

        // 6. Access underlying Discord Social SDK Core (if linked via vcpkg)
        void* discordCore = DISCORD::GET_CORE();
        (void)discordCore;
    } else if (reason == DLL_PROCESS_DETACH) {
        WotlkSDK::Shutdown();
    }
    return TRUE;
}
```

### Native Pure C Example (`Main.c`)

```c
#include <WOTLK-SDK/wotlkc.h>
#include <windows.h>

static void on_frame_update(void) {
    if (GAME_IS_IN_WORLD()) {
        wotlk_guid_t player_uid = PLAYER_GET_UID();
        const char* zone = ZONE_GET_ZONE_TEXT();
        (void)player_uid;
        (void)zone;
    }
}

static void on_cvar_modified(int new_val, int changed) {
    if (changed) {
        /* Handle runtime CVar adjustment */
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    (void)lpReserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        /* Optional auto-login configuration */
        wotlk_autologin_configure("username", "password", "logon.example.com", "Realm", "Character");

        /* Initialize SDK hooks */
        if (!wotlk_sdk_initialize(5000)) {
            return FALSE;
        }

        /* Register frame update callback */
        wotlk_on_update_add(on_frame_update);

        /* Register custom console variable */
        wotlk_cvar_register_int("pure_c_mod", "Pure C toggle", 1, 0, 1, on_cvar_modified);

        /* Optional Discord Rich Presence control */
        DISCORD_START("123456789012345678", 5000);

        /* Access Discord Social SDK core handle */
        void* core = DISCORD_GET_CORE();
        (void)core;
    } else if (reason == DLL_PROCESS_DETACH) {
        wotlk_sdk_shutdown();
    }
    return TRUE;
}
```

---

## Core Subsystems

### Auto-Login
Automates account authentication, realmlist configuration, realm selection, and character world entry.

1. **Programmatic API:**
   ```cpp
   WotlkSDK::AutoLogin::Configure("USER", "PASS", "realmlist", "realmname", "character");
   ```
2. **Command-Line Parameters:**
   ```cmd
   Wow.exe -login "USERNAME" -password "PASSWORD" -realmlist "logon.example.com" -realmname "Realm" -character "Name"
   ```
   *Case-insensitive aliases are supported: `-user`, `-pass`, `-server`, `-realm`, `-char`.*
3. **Environment Variables (`SCREAMING_SNAKE_CASE`):**
   - `WOTLK_LOGIN`, `WOTLK_PASSWORD`, `WOTLK_REALMLIST`, `WOTLK_REALMNAME`, `WOTLK_CHARACTER`

### Discord Social SDK & Rich Presence (`WOTLK-SDK/Discord.h`)
The Discord subsystem supports both official **Discord Social SDK** (`discord-game-sdk`) via **vcpkg** and a native zero-dependency Windows Named Pipe IPC fallback:

* **vcpkg Integration (`vcpkg.json`)**:
  - The repository includes a `vcpkg.json` manifest specifying the `discord-game-sdk` port.
  - Automatically locates headers and `discord_game_sdk.lib` during CMake configuration.
  - Automatically copies `discord_game_sdk.dll` next to your compiled mod binaries post-build.
* **Full Social SDK Core Access (`DISCORD::GET_CORE()`)**:
  - Cast the returned `void*` to `discord::Core*` to access multiplayer lobbies, voice chat, friends lists, and user account linking.
* **Zero-Dependency Fallback**:
  - If built without vcpkg or without `discord_game_sdk.dll`, the SDK automatically switches to its built-in Windows Named Pipe client (`\\.\pipe\discord-ipc-0`..`9`).
* **Live State Tracking**: Character name, level, race, class, zone, subzone, continent, game state (login, realm select, character select, in-world), party size, and elapsed timestamps.
* **Compile-Time Auto-Start**: Define `DISCORD_RICH_PRESENCE` and `DISCORD_CLIENT_ID` before including `<WOTLK-SDK/wotlkcpp.h>`.
* **C++ Template Overrides**: Customize presence formatting using `DISCORD::SET_FORMATTER<CustomFormatter>()`.

### Hooking Infrastructure (`tools/HOOK-KIT/`)
Modern type-safe detour and patching framework built on MinHook and asmjit:
- `HOOK_KIT::HookTransaction`: Atomic multi-hook transactions with rollback on failure.
- `HOOK_KIT::RetryBudget`: Configurable installation timeout for contested memory addresses.
- `HOOK_KIT::SiteHook` & `HOOK_KIT::CallsiteHook`: Mid-function instrumentation.
- `HOOK_KIT::WildHook`: Calling convention normalization and register manipulation.
- `HOOK_KIT::ScopedUnprotect`: RAII memory page protection manager (`VirtualProtect`).
- Backward-compatible `tools/hookkit/` symlink and `<hookkit/hookkit.h>` header alias.

### Engine Extensions & Registries (`WOTLK-SDK/Extensions.h`)
- **`WotlkSDK::CVAR_REGISTRY`**: Register custom console variables with auto clamping, type casting, and callbacks.
- **`WotlkSDK::EVENT_REGISTRY`**: Register and dispatch custom FrameScript events.
- **`WotlkSDK::LUA_LIB_REGISTRY`**: Register custom C/C++ functions into the client's Lua state.
- **`WotlkSDK::TOKEN_REGISTRY`**: Extend unit token resolution logic.
- **Lifecycle Callbacks**:
  - `WotlkSDK::ON_UPDATE`: Frame render tick.
  - `WotlkSDK::ON_ENTER`: World enter event.
  - `WotlkSDK::ON_LEAVE`: World leave event.
  - `WotlkSDK::POST_LOAD`: Glue XML loading complete.
  - `WotlkSDK::CHAR_ENUM`: Character list enumerated.

---

## Building with MSVC and vcpkg

### Prerequisites
- **Operating System**: Windows 10/11
- **Compiler**: Visual Studio 2022 (MSVC v143) with C++23 support
- **Package Manager**: [vcpkg](https://github.com/microsoft/vcpkg) (set `VCPKG_ROOT` environment variable)
- **Build System**: CMake 3.28+

### Step 1: Install Dependencies via vcpkg
In the project directory (vcpkg manifest mode will install automatically when configuring CMake), or install explicitly:
```cmd
vcpkg install discord-game-sdk:x86-windows
```

### Step 2: Build using CMake Presets (Recommended)
WotlkSDK includes standard `CMakePresets.json` configurations:

```cmd
# 32-bit Win32 Release Build (for WoW 3.3.5a in-game mods)
cmake --preset x86-msvc-release
cmake --build --preset x86-msvc-release

# 64-bit x64 Release Build (for companion tools & external bots)
cmake --preset x64-msvc-release
cmake --build --preset x64-msvc-release
```

### Step 3: Traditional Command-Line Build
```cmd
# Target 32-bit Win32 with vcpkg integration
cmake -B build -A Win32 -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

Outputs:
- Static Library: `build/bin/Release/WotlkSDK.lib`
- Sample Mods: `build/bin/Release/SampleMod.dll`, `build/bin/Release/SampleModC.dll`
- Discord Runtime: `build/bin/Release/discord_game_sdk.dll`

### Generating Complete Doxygen Documentation
Run Doxygen from the repository root to produce complete HTML documentation:
```cmd
doxygen Doxyfile
```
Documentation will be generated in `docs/html/index.html`.

---

## License
Licensed under the GNU General Public License v3 (see [LICENSE](LICENSE)). Third-party components retain their respective licenses:
- `MinHook` - Tsuda Kageyu
- `asmjit` - Petr Kobalicek
- `unordered_dense` - Martin Leitner-Ankerl
- `Discord Game SDK` - Discord Inc.
