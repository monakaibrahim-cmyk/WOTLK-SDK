# WotlkSDK API Reference

## 1. Core Lifecycle & API

Header: `<WotlkSDK.h>` / `<WOTLK-SDK/WotlkSDK.h>` / `<WOTLK-SDK/wotlkcpp.h>`

| Method / Variable | Description |
|---|---|
| `bool WotlkSDK::Initialize(uint32_t retryBudgetMs = 5000)` | Installs hooks and patches engine memory. Returns `true` on success. |
| `bool WotlkSDK::IsInitialized()` | Returns `true` if already initialized in this process. |
| `WotlkSDK::VERSION_STRING` | `"1.0.0"` |
| `WotlkSDK::TARGET_WOW_BUILD` | `12340` (WoW 3.3.5a) |

---

## 2. Auto-Login Subsystem

Header: `<WOTLK-SDK/AutoLogin.h>`

| Method / Variable | Description |
|---|---|
| `Configure(user, pass, realmlist, realmName, character)` | Programmatic credential configuration. |
| `SetLogin(user)` / `GetLogin()` | Username accessor. |
| `SetPassword(pass)` / `GetPassword()` | Password accessor. |
| `SetRealmlist(host)` / `GetRealmlist()` | Realmlist host override. |
| `SetRealmName(name)` / `GetRealmName()` | Realm name override. |
| `SetCharacter(char)` / `GetCharacter()` | Character name for auto-entry. |

### Configuration Fallback Hierarchy:
1. Programmatic API (`WotlkSDK::AutoLogin::Configure`)
2. Command-line parameters (`-login`, `-password`, `-realmlist`, `-realmname`, `-character`)
3. Environment variables (`WOTLK_LOGIN`, `WOTLK_PASSWORD`, `WOTLK_REALMLIST`, `WOTLK_REALMNAME`, `WOTLK_CHARACTER`)
4. Client defaults (`Data/<locale>/realmlist.wtf`, `WTF/Config.wtf`)

---

## 3. Registries & Callbacks

Header: `<WOTLK-SDK/Extensions.h>`

| Variable (`SCREAMING_SNAKE_CASE`) | Type | Description |
|---|---|---|
| `WotlkSDK::CVAR_REGISTRY` | `CVarRegistry` | Dynamically register custom CVars with auto-clamping & callbacks. |
| `WotlkSDK::EVENT_REGISTRY` | `EventRegistry` | Register and fire custom FrameScript events. |
| `WotlkSDK::LUA_LIB_REGISTRY` | `LuaLibRegistry` | Register custom C/C++ functions into the client Lua 5.1 environment. |
| `WotlkSDK::TOKEN_REGISTRY` | `TokenRegistry` | Extend unit token resolving (`"player"`, `"target"`, etc.). |
| `WotlkSDK::ON_UPDATE` | `CallbackList` | Fired on every engine render/paint tick. |
| `WotlkSDK::ON_ENTER` | `CallbackList` | Fired when local player enters the world. |
| `WotlkSDK::ON_LEAVE` | `CallbackList` | Fired when local player leaves the world. |
| `WotlkSDK::POST_LOAD` | `CallbackList` | Fired after glue XML interface is loaded. |
| `WotlkSDK::CHAR_ENUM` | `CallbackList` | Fired after character list is enumerated. |

---

## 4. Engine Subsystems

All namespaces and types use `UpperCamelCase`:

* `WotlkSDK::ObjectMgr`: Object enumeration, unit/player/gameobject casting, `TARGET_GUID`.
* `WotlkSDK::NetClient`: Client network connection, `NetClient::login`.
* `WotlkSDK::GameUI`: Combat lockdown state (`inCombatLockdown`), targeting, world frame.
* `WotlkSDK::Login`: Character selection display list (`TSGRA`), character select & enter world.
* `WotlkSDK::Storm`: Blizzard Storm library (Memory, File I/O, Strings, Synchronization).
* `WotlkSDK::Lua`: Embedded Lua 5.1 runtime calls, global registers, stack helpers.
* `WotlkSDK::ClientDB`: Direct DBC record access and caching.
* `WotlkSDK::OS`: Window handle (`getGameWindow`), clipboard, timing.

---

## 5. Native Pure C API

Header: `<WotlkSDK_C.h>` / `<WOTLK-SDK/WotlkSDK_C.h>` / `<WOTLK-SDK/wotlkc.h>`

For pure C (C99/C11/C17/C23) projects and DLLs.

| C Function / Macro | Description |
|---|---|
| `wotlk_sdk_initialize(uint32_t retry_budget_ms)` | Initializes SDK hooks (returns 1 on success, 0 on error). |
| `wotlk_sdk_is_initialized()` | Queries initialization status. |
| `wotlk_autologin_configure(...)` | Programmatically configures auto-login. |
| `wotlk_on_update_add(fn)` | Adds a per-frame render tick callback. |
| `wotlk_on_enter_add(fn)` | Adds a world enter callback. |
| `wotlk_on_leave_add(fn)` | Adds a world leave callback. |
| `wotlk_cvar_register_int(...)` | Registers a custom console CVar in pure C. |
| `wotlk_cvar_get_int(name)` | Reads integer CVar value. |
| `wotlk_cvar_set(name, value)` | Sets CVar string value. |
| `wotlk_get_player_guid()` | Retrieves local player `uint64_t` GUID. |
| `wotlk_get_target_guid()` | Retrieves target `uint64_t` GUID. |
| `wotlk_is_in_world()` | Returns 1 if player is currently in world, 0 otherwise. |
| `wotlk_in_combat_lockdown()` | Returns 1 if in combat lockdown, 0 otherwise. |
| `wotlk_get_game_window()` | Returns game HWND window handle as `void*`. |
| `wotlk_get_async_time_ms()` | Returns client async timer in milliseconds. |

*Note: All functions also have uppercase macros (e.g. `WOTLK_SDK_INITIALIZE`, `WOTLK_ON_UPDATE_ADD`).*
