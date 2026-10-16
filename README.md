# WotlkSDK
## World of Warcraft 3.3.5a (12340) Software Development Kit

WotlkSDK is a modern C++23 development kit and base library targeting the 32-bit World of Warcraft 3.3.5a (build 12340) client. It provides reverse-engineered engine offsets, data structures, hooking infrastructure, engine extension registries, and a fully configurable auto-login system.

Designed as a clean, modular API, WotlkSDK can be statically linked directly into your custom mods/DLLs (eliminating extra runtime DLL dependencies) or compiled as a standalone injector-ready DLL.

---

## Architecture & API Design

### 1. Main Header & Entry Point
Include the SDK via a single top-level header:
```cpp
#include <WotlkSDK.h>
```

### 2. Static Library (`.lib`) vs Shared DLL (`.dll`)
* **Static Library (`WotlkSDK.lib`, default)**:
  - Best for custom mods, hook DLLs, and injection loaders.
  - Linked directly into your DLL/executable; no need to ship an extra DLL.
  - Exposes `WotlkSDK::Initialize()` and `WotlkSDK::IsInitialized()`.
  - Does **not** export a `DllMain`, preventing symbol conflicts with consumer DLLs.
* **Shared Library (`WotlkSDK.dll`)**:
  - Build by passing `-DBUILD_SHARED_LIBS=ON` to CMake.
  - Embeds `DllMain` and auto-initializes upon process attachment (ready for standalone injection).

### 3. UpperCamelCase & SCREAMING_SNAKE_CASE Conventions
* **Namespaces, Classes, and Structs**: formatted in `UpperCamelCase` (e.g. `WotlkSDK`, `ObjectMgr`, `NetClient`, `GameUI`, `Login`, `Storm`, `Lua`, `ClientDB`, `AutoLogin`, `Extensions`). Backward-compatible lowercase aliases are preserved for all namespaces.
* **Variables, Registries, Callbacks & Constants**: accessible via `SCREAMING_SNAKE_CASE` (e.g. `CVAR_REGISTRY`, `EVENT_REGISTRY`, `LUA_LIB_REGISTRY`, `TOKEN_REGISTRY`, `ON_UPDATE`, `ON_ENTER`, `ON_LEAVE`, `POST_LOAD`, `CHAR_ENUM`, `TARGET_GUID`, `LOCALES`).
* **Environment Variables**: all environment configuration variables follow `SCREAMING_SNAKE_CASE` (`WOTLK_LOGIN`, `WOTLK_PASSWORD`, etc.).

---

## API Usage Example

```cpp
#include <WotlkSDK.h>
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // Optional: programmatically configure auto-login
        WotlkSDK::AutoLogin::Configure("username", "password", "logon.warmane.com", "Lordaeron", "MyCharacter");

        // Initialize the SDK and hook system
        if (!WotlkSDK::Initialize()) {
            return FALSE;
        }

        // Register custom frame updates
        WotlkSDK::ON_UPDATE->add([]() {
            // Per-frame game loop logic
        });

        // Register a custom CVar
        WotlkSDK::CVAR_REGISTRY->add<int>({
            .name = "my_custom_cvar",
            .desc = "Enables custom feature",
            .init = 1,
            .on_change = [](int val, bool changed) {
                // React to CVar changes
            }
        });
    }
    return TRUE;
}
```

### Pure C DLL Example (`Main.c`)

```c
#include <WotlkSDK_C.h>
#include <windows.h>

static void on_frame(void) {
    /* Pure C per-frame game tick */
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    (void)lpReserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        /* Optional auto-login */
        wotlk_autologin_configure("username", "password", "logon.example.com", "Realm", "Character");

        /* Initialize SDK */
        if (!wotlk_sdk_initialize(5000)) {
            return FALSE;
        }

        /* Register callbacks and CVars in pure C */
        wotlk_on_update_add(on_frame);
    }
    return TRUE;
}
```

---

## Features

### Auto Login
Configure credentials and realm options programmatically in code, via command-line arguments, or via environment variables:

1. **Programmatic C++ API:**
   ```cpp
   WotlkSDK::AutoLogin::Configure("USER", "PASS", "realmlist", "realmname", "character");
   ```
2. **Command Line Arguments:**
   ```cmd
   Wow.exe -login "USERNAME" -password "PASSWORD" -realmlist "logon.example.com" -realmname "Realm Name" -character "Character Name"
   ```
3. **Environment Variables (`SCREAMING_SNAKE_CASE`):**
   - `WOTLK_LOGIN`: Account username (overridden by programmatic API and `-login`)
   - `WOTLK_PASSWORD`: Account password (overridden by programmatic API and `-password`)
   - `WOTLK_REALMLIST`: Sets the `realmList` CVar (overridden by programmatic API and `-realmlist`)
   - `WOTLK_REALMNAME`: Sets the `realmName` CVar (overridden by programmatic API and `-realmname`)
   - `WOTLK_CHARACTER`: Automatically selects and enters the world with the character (overridden by `-character`)

### Client Offsets & SDK Headers (`include/`)
Comprehensive engine definitions and offsets for WoW 3.3.5a:
- **`ObjectMgr`**: `CGObject_C`, `CGUnit_C`, `CGPlayer_C`, `CGGameObject_C`, `TARGET_GUID`, object enumerator
- **`FrameScript` & `Lua`**: Lua 5.1 runtime integration, FrameScript event dispatching, `LUA_REF_ERROR_HANDLER`, `ENVIRON_INDEX`, `GLOBALS_INDEX`, `REGISTRY_INDEX`
- **`Login` & `Glue`**: `LoginUI`, character selection, and glue screen managers
- **`GameUI`**: World frame, unit targeting, `PORTRAIT_USE_RENDER_TARGET`, `PORTRAIT_DEPTH_TEXTURE`, `inCombatLockdown`
- **`Graphics`**: `CGxDevice`, `CGxFont`, `CGxShader`, `CGxTex`, FreeType and BLP codecs
- **`NetClient`**: Client network handling, `NetClient::login`
- **`Storm`**: Memory allocation (`Storm::Memory`), file I/O (`Storm::File`), strings (`Storm::Str`), synchronization (`Storm::Sync`)

### Hooking Infrastructure (`tools/hookkit/`)
Type-safe x86 hooking framework built on MinHook and asmjit:
- Atomic multi-hook transactions (`hookkit::HookTransaction`) with retry budgets
- Support for calling conventions: `eCdecl`, `eStdcall`, `eThiscall`, `eFastcall`, and register-based `WildAbi`
- Call-site trampoline builders and memory patch utilities

### Engine Extensions & Registries (`WotlkSDK/Extensions.h`)
All registries and callbacks are accessible via `SCREAMING_SNAKE_CASE`:
- **`WotlkSDK::CVAR_REGISTRY`**: Register custom CVars with auto clamping, type conversion, and change callbacks
- **`WotlkSDK::EVENT_REGISTRY`**: Register custom FrameScript events dynamically
- **`WotlkSDK::LUA_LIB_REGISTRY`**: Register C functions into the client's Lua state
- **`WotlkSDK::TOKEN_REGISTRY`**: Extend unit tokens with custom resolution logic
- **Lifecycle Callbacks**:
  - `WotlkSDK::ON_UPDATE`: Engine paint frame tick
  - `WotlkSDK::ON_ENTER`: World enter event
  - `WotlkSDK::ON_LEAVE`: World leave event
  - `WotlkSDK::POST_LOAD`: Glue XML loading
  - `WotlkSDK::CHAR_ENUM`: Character list enumeration

---

## Building

### Requirements
- **Operating System**: Windows (or cross-compilation targeting Win32)
- **Compiler**: Visual Studio 2022 (MSVC) or Clang-cl with C++23 support
- **Architecture**: 32-bit (x86 / Win32)
- **Build System**: CMake 3.28+

### Build Static Library (Default, Recommended for SDK consumers)
```cmd
cmake -B build -A Win32
cmake --build build --config Release
```
Outputs `build/bin/Release/WotlkSDK.lib`.

### Build Shared Library (Standalone DLL)
```cmd
cmake -B build -A Win32 -DBUILD_SHARED_LIBS=ON
cmake --build build --config Release
```
Outputs `build/bin/Release/WotlkSDK.dll`.

---

## License
Licensed under the GNU General Public License v3 (see [LICENSE](LICENSE)). Third-party components retain their respective licenses:
- `MinHook` - Tsuda Kageyu
- `asmjit` - Petr Kobalicek
- `unordered_dense` - Martin Leitner-Ankerl
