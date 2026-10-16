# WotlkSDK API Reference
**Target Client:** World of Warcraft: Wrath of the Lich King 3.3.5a (Build 12340)

---

## 1. Core Lifecycle & API

### Headers
- C++: `<WOTLK-SDK/wotlkcpp.h>` or `<WOTLK-SDK/WotlkSDK.h>`
- Pure C: `<WOTLK-SDK/wotlkc.h>` or `<WOTLK-SDK/WotlkSDK_C.h>`

### Lifecycle Functions

| Function | Return Type | Description |
|---|---|---|
| `WotlkSDK::Initialize(retryBudgetMs)` | `bool` | Patches engine integrity validation pointers and atomically installs startup hook transactions. Returns `true` on success. |
| `WotlkSDK::IsInitialized()` | `bool` | Checks whether the SDK has completed initialization in the current process. |
| `WotlkSDK::Shutdown()` | `void` | Gracefully shuts down background worker threads (Discord Rich Presence) and releases SDK resources. |

### Version Constants
- `WotlkSDK::VERSION_MAJOR`: `1`
- `WotlkSDK::VERSION_MINOR`: `0`
- `WotlkSDK::VERSION_PATCH`: `0`
- `WotlkSDK::VERSION_STRING`: `"1.0.0"`
- `WotlkSDK::TARGET_WOW_BUILD`: `12340`
- `WotlkSDK::STARTUP_RETRY_BUDGET_MS`: `5000`

---

## 2. Auto-Login Subsystem

Header: `<WOTLK-SDK/AutoLogin.h>`

Programmatically configures credentials, destination realms, and automatic character entry.

### Functions

| Function | Parameters | Description |
|---|---|---|
| `AutoLogin::Configure(user, pass, realmlist, realm, char)` | `std::string_view` for each | Configures all login parameters simultaneously. |
| `AutoLogin::SetLogin(user)` | `std::string_view` | Sets account username. |
| `AutoLogin::SetPassword(pass)` | `std::string_view` | Sets account password. |
| `AutoLogin::SetRealmlist(host)` | `std::string_view` | Sets destination realmlist host. |
| `AutoLogin::SetRealmName(realm)` | `std::string_view` | Sets destination realm name. |
| `AutoLogin::SetCharacter(char)` | `std::string_view` | Sets character name for automated login. |
| `AutoLogin::GetLogin()` | - | Returns configured username. |
| `AutoLogin::GetPassword()` | - | Returns configured password. |
| `AutoLogin::GetRealmlist()` | - | Returns configured realmlist host. |
| `AutoLogin::GetRealmName()` | - | Returns configured realm name. |
| `AutoLogin::GetCharacter()` | - | Returns configured character name. |

### Configuration Fallback Hierarchy
1. **Programmatic C/C++ API**: `AutoLogin::Configure(...)` / `wotlk_autologin_configure(...)`
2. **Command-Line Parameters**:
   - `-login` / `-user` / `-email` / `-account <username>`
   - `-password` / `-pass <password>`
   - `-realmlist` / `-server` / `-host <realmlist>`
   - `-realmname` / `-realm <name>`
   - `-character` / `-char <name>`
3. **Environment Variables**:
   - `WOTLK_LOGIN`, `WOTLK_PASSWORD`, `WOTLK_REALMLIST`, `WOTLK_REALMNAME`, `WOTLK_CHARACTER`
4. **Client Defaults**: `Data/<locale>/realmlist.wtf` and `WTF/Config.wtf`

---

## 3. Engine Registries & Callbacks

Header: `<WOTLK-SDK/Extensions.h>`

Global registries and callbacks accessible directly via `SCREAMING_SNAKE_CASE`:

| Identifier | Type | Description |
|---|---|---|
| `WotlkSDK::CVAR_REGISTRY` | `CVarRegistry` | Dynamically register custom console variables with auto-clamping and callbacks. |
| `WotlkSDK::EVENT_REGISTRY` | `EventRegistry` | Register and fire custom FrameScript engine events. |
| `WotlkSDK::LUA_LIB_REGISTRY` | `LuaLibRegistry` | Register custom C/C++ functions into the client's Lua 5.1 environment. |
| `WotlkSDK::TOKEN_REGISTRY` | `TokenRegistry` | Extend unit token resolving logic (`"player"`, `"target"`, etc.). |
| `WotlkSDK::ON_UPDATE` | `CallbackList` | Fired on every engine render/paint frame tick. |
| `WotlkSDK::ON_ENTER` | `CallbackList` | Fired when local player enters the world (after loading screen). |
| `WotlkSDK::ON_LEAVE` | `CallbackList` | Fired when local player leaves the world or logs out. |
| `WotlkSDK::POST_LOAD` | `CallbackList` | Fired after glue XML interface is loaded. |
| `WotlkSDK::CHAR_ENUM` | `CallbackList` | Fired after the character selection list is enumerated. |

---

## 4. Typed Modules (`SCREAMING_SNAKE_CASE`)

Header: `<WOTLK-SDK/API.h>`

### `PLAYER::` Module
- `PLAYER::GET_UID()` / `PLAYER::GET_GUID()`: Returns 64-bit GUID (`guid_t`) of the local player.
- `PLAYER::GET_TARGET_UID()` / `PLAYER::GET_TARGET_GUID()`: Returns target entity GUID.
- `PLAYER::GET_UNIT()`: Returns `CGUnit_C*` pointer.
- `PLAYER::GET_PLAYER()`: Returns `CGPlayer_C*` pointer.
- `PLAYER::GET_NAME()`: Returns active or selected character name string (`const char*`).
- `PLAYER::GET_LEVEL()`: Returns character level (`1-80`).
- `PLAYER::GET_RACE()` / `PLAYER::GET_RACE_NAME()`: Numeric race ID and string name (`"Human"`, `"Orc"`, etc.).
- `PLAYER::GET_CLASS()` / `PLAYER::GET_CLASS_NAME()`: Numeric class ID and string name (`"Paladin"`, `"Mage"`, etc.).
- `PLAYER::GET_GENDER()` / `PLAYER::GET_GENDER_NAME()`: Gender ID (`0=Male, 1=Female`) and string name.
- `PLAYER::GET_HEALTH()` / `PLAYER::GET_MAX_HEALTH()`: Current and maximum health values.
- `PLAYER::GET_POWER(index)` / `PLAYER::GET_MAX_POWER(index)`: Power points (`0=Mana`, `1=Rage`, `3=Energy`, `6=Runic Power`).
- `PLAYER::GET_MANA()` / `PLAYER::GET_MAX_MANA()`: Current and maximum mana values.
- `PLAYER::GET_POSITION()`: Returns `Vec3f {x, y, z}` world coordinates.
- `PLAYER::GET_FACING()`: Facing orientation angle in radians `[0, 2*PI)`.
- `PLAYER::IS_IN_COMBAT()`: Returns `true` if in combat lockdown.
- `PLAYER::IS_ALIVE()` / `PLAYER::IS_DEAD()` / `PLAYER::IS_GHOST()`: Vitality states.
- `PLAYER::SET_TARGET(guid)`: Programmatically sets the active target.
- `PLAYER::CLEAR_TARGET()`: Clears active target.

### `SPELL::` Module
- `SPELL::GET_ID()`: Returns spell ID of currently pending/casting spell.
- `SPELL::IS_TARGETING_AOE()`: Checks if reticle AoE targeting cursor is active.
- `SPELL::CANCEL_AOE_TARGETING()`: Cancels AoE reticle targeting.
- `SPELL::CAST(spellId, item, target)`: Casts spell on target.

### `GAME::` Module
- `GAME::GET_STATE()`: Numeric engine state (`0=Login`, `1=CharSelect`, `2=InGame`).
- `GAME::GET_STATE_NAME()`: String representation (`"Login Screen"`, `"Character Selection"`, `"In Game"`).
- `GAME::IS_IN_WORLD()`: Returns `true` if player is loaded into world.
- `GAME::IS_LOADING()`: Returns `true` if an engine loading screen is rendering.
- `GAME::GET_WINDOW()`: Returns game `HWND` window handle (`void*`).
- `GAME::GET_TIME_MS()`: Returns high-resolution async millisecond timer.

### `ZONE::` Module
- `ZONE::GET_ZONE_TEXT()` / `ZONE::GET_TEXT()`: Active zone text (`"Dalaran"`).
- `ZONE::GET_SUBZONE_TEXT()`: Active subzone text (`"Runeweaver Square"`).
- `ZONE::GET_MINIMAP_ZONE_TEXT()`: Minimap hover text.
- `ZONE::GET_AREA_ID()`: Numeric area identifier from `AreaTable.dbc`.
- `ZONE::GET_MAP_ID()`: Numeric map identifier (`0=Eastern Kingdoms`, `1=Kalimdor`, `530=Outland`, `571=Northrend`).
- `ZONE::GET_MAP_NAME()`: Internal map directory string.
- `ZONE::GET_CONTINENT_NAME()`: Continent display string.

### `PRESENCE::` Module
- `PRESENCE::GET_DATA()`: Returns a complete `RICH_PRESENCE_DATA` snapshot struct containing:
  - `game_state`, `game_state_name`
  - `character_name`, `level`, `race_name`, `class_name`
  - `zone_text`, `subzone_text`, `continent_name`, `map_name`
  - `map_id`, `area_id`
  - `is_in_world`, `is_loading`, `is_in_combat`

### `CVAR::` Module
- `CVAR::GET_INT(name)`: Reads integer CVar value.
- `CVAR::GET_FLOAT(name)`: Reads float CVar value.
- `CVAR::SET(name, value)`: Sets CVar string value.
- `CVAR::REFRESH(name)`: Triggers CVar refresh signal.

### `LUA::` Module
- `LUA::EXECUTE(script)`: Executes raw Lua 5.1 code in the client UI environment.

---

## 5. Discord Social SDK & Rich Presence Subsystem

Header: `<WOTLK-SDK/Discord.h>`

The Discord subsystem features dual-mode architecture:
1. **Official Discord Social SDK (`discord-game-sdk`) via vcpkg**:
   - Manages presence, activity, multiplayer lobbies, voice chat, and user accounts.
   - Access the underlying `discord::Core*` using `DISCORD::GET_CORE()`.
   - Requires `discord_game_sdk.dll` alongside the target DLL (automatically copied post-build).
2. **Native Zero-Dependency Windows Named Pipe IPC Fallback**:
   - Directly communicates with Discord Desktop over `\\.\pipe\discord-ipc-0`..`9`.
   - Used when built without vcpkg or when running without `discord_game_sdk.dll`.

### Compile-Time Auto-Start
```cpp
#define DISCORD_RICH_PRESENCE
#define DISCORD_CLIENT_ID "123456789012345678"
#include <WOTLK-SDK/wotlkcpp.h>
```
When defined, the worker thread automatically starts when `WotlkSDK::Initialize()` is called and shuts down on `WotlkSDK::Shutdown()` or DLL unload.

### C++ Manual Control & Discord Social SDK Core Access
```cpp
// Manual lifecycle controls
DISCORD::START("123456789012345678", 5000);
DISCORD::UPDATE();
DISCORD::STOP();
bool connected = DISCORD::IS_CONNECTED();

// Access Discord Social SDK Core (if linked via vcpkg discord-game-sdk)
void* corePtr = DISCORD::GET_CORE();
if (corePtr) {
    // Cast to discord::Core* to access lobbies, voice, and user accounts
    auto* core = static_cast<discord::Core*>(corePtr);
    (void)core;
}

// Template override for custom presence formatting (C++ only)
struct MyPresenceFormatter {
    static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, DISCORD::Activity& out) {
        DISCORD::DefaultFormatter::Format(in, out);
        out.details = std::format("[Guild Raid] {}", out.details);
        out.party_size = 25;
        out.party_max = 25;
    }
};

DISCORD::SET_FORMATTER<MyPresenceFormatter>();
```

### Pure C API
```c
DISCORD_START("123456789012345678", 5000);

if (DISCORD_IS_CONNECTED()) {
    DISCORD_UPDATE();
}

void* core = DISCORD_GET_CORE();
(void)core;

DISCORD_STOP();
```

---

## 6. Native Pure C API Reference

Header: `<WOTLK-SDK/wotlkc.h>`

Exposes an `extern "C"` ABI for pure C mod development with zero C++ runtime dependencies.

| C Function | Macro Alias | Description |
|---|---|---|
| `wotlk_sdk_initialize(retry_ms)` | `WOTLK_SDK_INITIALIZE` | Initializes SDK hooks (1=success, 0=error). |
| `wotlk_sdk_is_initialized()` | `WOTLK_SDK_IS_INITIALIZED` | Queries initialization status. |
| `wotlk_sdk_shutdown()` | `WOTLK_SDK_SHUTDOWN` | Shuts down SDK background workers. |
| `wotlk_autologin_configure(...)` | `AUTOLOGIN_CONFIGURE` | Programmatically sets auto-login credentials. |
| `wotlk_on_update_add(fn)` | `WOTLK_ON_UPDATE_ADD` | Registers per-frame render tick callback. |
| `wotlk_on_enter_add(fn)` | `WOTLK_ON_ENTER_ADD` | Registers world enter callback. |
| `wotlk_on_leave_add(fn)` | `WOTLK_ON_LEAVE_ADD` | Registers world leave callback. |
| `wotlk_cvar_register_int(...)` | `WOTLK_CVAR_REGISTER_INT` | Registers custom integer CVar. |
| `wotlk_cvar_get_int(name)` | `CVAR_GET_INT` | Reads integer CVar value. |
| `wotlk_cvar_set(name, val)` | `CVAR_SET` | Sets CVar string value. |
| `wotlk_get_player_guid()` | `WOTLK_GET_PLAYER_GUID` | Retrieves player GUID. |
| `wotlk_get_target_guid()` | `WOTLK_GET_TARGET_GUID` | Retrieves target GUID. |
| `wotlk_is_in_world()` | `WOTLK_IS_IN_WORLD` | Returns 1 if in world, 0 otherwise. |
| `wotlk_in_combat_lockdown()` | `WOTLK_IN_COMBAT_LOCKDOWN` | Returns 1 if in combat lockdown. |
| `wotlk_get_game_window()` | `WOTLK_GET_GAME_WINDOW` | Returns game HWND. |
| `wotlk_get_async_time_ms()` | `WOTLK_GET_ASYNC_TIME_MS` | Returns async time in ms. |
| `PLAYER_GET_UID()` | - | Retrieves player GUID. |
| `PLAYER_GET_NAME()` | - | Retrieves player name. |
| `PLAYER_GET_LEVEL()` | - | Retrieves player level. |
| `SPELL_GET_ID()` | - | Retrieves casting spell ID. |
| `SPELL_CAST(id, target)` | - | Casts spell. |
| `GAME_GET_STATE()` | - | Retrieves engine state. |
| `ZONE_GET_ZONE_TEXT()` | - | Retrieves zone text. |
| `PRESENCE_GET_DATA(out)` | - | Fills `wotlk_rich_presence_t` snapshot. |
| `LUA_EXECUTE(script)` | - | Executes Lua script buffer. |
| `wotlk_discord_start(id, ms)` | `DISCORD_START` | Starts Discord presence worker. |
| `wotlk_discord_stop()` | `DISCORD_STOP` | Stops Discord presence worker. |
| `wotlk_discord_update()` | `DISCORD_UPDATE` | Forces presence update. |
| `wotlk_discord_is_connected()` | `DISCORD_IS_CONNECTED` | Checks presence connection. |
| `wotlk_discord_get_core()` | `DISCORD_GET_CORE` | Retrieves Discord Social SDK core handle (void*). |

---

## 7. HOOK-KIT

Path: `tools/HOOK-KIT/` (with `tools/hookkit/` alias link)  
Headers: `<HOOK-KIT/HOOK-KIT.h>` or `<hookkit/hookkit.h>`

Type-safe atomic detours, mid-function hooks, and binary patching engine:
- `HOOK_KIT::HookTransaction`: Atomic multi-hook installation with automatic rollback on error.
- `HOOK_KIT::RetryBudget`: Configurable installation timeout for contested hooks.
- `HOOK_KIT::SiteHook` & `HOOK_KIT::CallsiteHook`: Mid-function instrumentation.
- `HOOK_KIT::WildHook`: Arbitrary register calling convention manipulation.
- `HOOK_KIT::ScopedUnprotect`: RAII memory page protection manager (`VirtualProtect`).
- `HOOK_KIT::PatchBytes`: Memory byte patching with automatic original byte restoration.

---

## 8. Compiler & Build System Requirements

### MSVC Enforcement (MinGW64 Unsupported)
WotlkSDK strictly requires **Microsoft Visual C++ (MSVC / cl.exe)** or **clang-cl**. MinGW / GCC / MinGW64 are explicitly unsupported due to:
- **Game Engine ABI**: WoW 3.3.5a engine binaries use MSVC calling conventions (`__thiscall`, `__stdcall`, struct padding, and virtual function dispatch).
- **Discord Social SDK**: Official Discord Game SDK libraries (`discord_game_sdk.lib`) are compiled for MSVC and cannot be linked with MinGW without runtime errors and missing symbols.
- **vcpkg Integration**: Manifest mode natively targets `x86-windows` and `x64-windows` MSVC triplets.

### CMake Presets
The repository provides configured `CMakePresets.json`:
- `x86-msvc-release`: 32-bit Win32 target for native WoW 3.3.5a in-game mods and hooks.
- `x86-msvc-debug`: 32-bit Win32 debug target with symbol tables.
- `x64-msvc-release`: 64-bit target for companion external applications and bots.
- `ninja-msvc-x86`: Fast build preset utilizing the Ninja generator with MSVC `cl.exe`.
