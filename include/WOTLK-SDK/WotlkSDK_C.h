#ifndef WOTLK_SDK_C_H
#define WOTLK_SDK_C_H

/**
 * @file WotlkSDK_C.h
 * @brief Native Pure C API for WotlkSDK (World of Warcraft 3.3.5a build 12340).
 *
 * Exposes a pure C (C99/C11/C17/C23) extern "C" ABI with zero C++ runtime or STL
 * dependencies. Enables developing native mods and DLLs in pure C without linking libstdc++
 * or MSVC C++ runtime libraries.
 */

#include <stdint.h>
#include <stddef.h>

#if defined(_WIN32)
    #if defined(WOTLK_SDK_BUILD_SHARED) || defined(WOTLK_SDK_SHARED)
        #if defined(WOTLK_SDK_EXPORTS)
            #define WOTLK_C_API __declspec(dllexport)
        #else
            #define WOTLK_C_API __declspec(dllimport)
        #endif
    #else
        #define WOTLK_C_API
    #endif
#else
    #define WOTLK_C_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */
/* Version and Build Information                                             */
/* ========================================================================= */

/** @brief Major version component of the SDK. */
#define WOTLK_SDK_C_VERSION_MAJOR 1

/** @brief Minor version component of the SDK. */
#define WOTLK_SDK_C_VERSION_MINOR 0

/** @brief Patch version component of the SDK. */
#define WOTLK_SDK_C_VERSION_PATCH 0

/** @brief Semantic version string representation. */
#define WOTLK_SDK_C_VERSION_STRING "1.0.0"

/** @brief Target World of Warcraft client build number. */
#define WOTLK_SDK_C_TARGET_WOW_BUILD 12340

/** @brief Target World of Warcraft release version string. */
#define WOTLK_SDK_C_TARGET_WOW_VERSION "3.3.5a"

/**
 * @brief Retrieves the SDK semantic version string.
 * @return const char* Version string (e.g., "1.0.0").
 */
WOTLK_C_API const char* wotlk_sdk_get_version_string(void);

/**
 * @brief Retrieves the major version number.
 * @return uint32_t Major version component.
 */
WOTLK_C_API uint32_t    wotlk_sdk_get_version_major(void);

/**
 * @brief Retrieves the minor version number.
 * @return uint32_t Minor version component.
 */
WOTLK_C_API uint32_t    wotlk_sdk_get_version_minor(void);

/**
 * @brief Retrieves the patch version number.
 * @return uint32_t Patch version component.
 */
WOTLK_C_API uint32_t    wotlk_sdk_get_version_patch(void);

/**
 * @brief Retrieves the target client engine build number.
 * @return uint32_t Build number (12340).
 */
WOTLK_C_API uint32_t    wotlk_sdk_get_target_wow_build(void);

/* ========================================================================= */
/* Core Lifecycle API                                                        */
/* ========================================================================= */

/**
 * @brief Initializes WotlkSDK, patches client engine validation pointers, and installs core hooks.
 * @param retry_budget_ms Timeout for hook installation in milliseconds (default: 5000).
 * @return int 1 on success, 0 on failure.
 */
WOTLK_C_API int  wotlk_sdk_initialize(uint32_t retry_budget_ms);

/**
 * @brief Checks if WotlkSDK has already initialized in the current process.
 * @return int 1 if initialized, 0 otherwise.
 */
WOTLK_C_API int  wotlk_sdk_is_initialized(void);

/**
 * @brief Shuts down background workers (Discord Rich Presence) and releases SDK resources.
 */
WOTLK_C_API void wotlk_sdk_shutdown(void);

/* ========================================================================= */
/* Auto-Login API                                                            */
/* ========================================================================= */

/**
 * @brief Programmatically configures auto-login credentials and destination realm.
 *
 * Precedence hierarchy:
 * 1. Programmatic API (this function)
 * 2. Command-line flags (-login, -password, -realmlist, -realmname, -character)
 * 3. Environment variables (WOTLK_LOGIN, WOTLK_PASSWORD, etc.)
 * 4. Game defaults (realmlist.wtf and WTF/Config.wtf)
 *
 * @param login Account username.
 * @param password Account password.
 * @param realmlist Optional realmlist host override.
 * @param realm_name Optional realm display name.
 * @param character Optional character name for automatic character selection.
 */
WOTLK_C_API void wotlk_autologin_configure(
    const char* login,
    const char* password,
    const char* realmlist,
    const char* realm_name,
    const char* character
);

/** @brief Sets account username. */
WOTLK_C_API void wotlk_autologin_set_login(const char* login);

/** @brief Sets account password. */
WOTLK_C_API void wotlk_autologin_set_password(const char* password);

/** @brief Sets realmlist host override. */
WOTLK_C_API void wotlk_autologin_set_realmlist(const char* realmlist);

/** @brief Sets destination realm name. */
WOTLK_C_API void wotlk_autologin_set_realm_name(const char* realm_name);

/** @brief Sets destination character name for automatic selection. */
WOTLK_C_API void wotlk_autologin_set_character(const char* character);

/** @brief Retrieves configured account username. */
WOTLK_C_API const char* wotlk_autologin_get_login(void);

/** @brief Retrieves configured account password. */
WOTLK_C_API const char* wotlk_autologin_get_password(void);

/** @brief Retrieves configured realmlist host. */
WOTLK_C_API const char* wotlk_autologin_get_realmlist(void);

/** @brief Retrieves configured destination realm name. */
WOTLK_C_API const char* wotlk_autologin_get_realm_name(void);

/** @brief Retrieves configured character name. */
WOTLK_C_API const char* wotlk_autologin_get_character(void);

/* ========================================================================= */
/* Lifecycle Callback System                                                 */
/* ========================================================================= */

/** @brief Generic zero-argument callback function pointer type. */
typedef void (*wotlk_callback_fn)(void);

/**
 * @brief Registers a callback fired on every engine render/paint tick.
 * @param callback Callback function pointer.
 */
WOTLK_C_API void wotlk_on_update_add(wotlk_callback_fn callback);

/**
 * @brief Registers a callback fired when the local player enters the game world.
 * @param callback Callback function pointer.
 */
WOTLK_C_API void wotlk_on_enter_add(wotlk_callback_fn callback);

/**
 * @brief Registers a callback fired when the local player leaves the game world.
 * @param callback Callback function pointer.
 */
WOTLK_C_API void wotlk_on_leave_add(wotlk_callback_fn callback);

/**
 * @brief Registers a callback fired after the glue XML interface has loaded.
 * @param callback Callback function pointer.
 */
WOTLK_C_API void wotlk_on_post_load_add(wotlk_callback_fn callback);

/**
 * @brief Registers a callback fired when the character select list is populated.
 * @param callback Callback function pointer.
 */
WOTLK_C_API void wotlk_on_char_enum_add(wotlk_callback_fn callback);

/* ========================================================================= */
/* Console & CVar System                                                     */
/* ========================================================================= */

/** @brief Callback function type for integer CVar changes. */
typedef void (*wotlk_cvar_change_int_fn)(int new_value, int changed);

/** @brief Callback function type for float CVar changes. */
typedef void (*wotlk_cvar_change_float_fn)(float new_value, int changed);

/**
 * @brief Registers a custom integer console variable with min/max bounds.
 * @param name CVar name string.
 * @param desc Helpful description displayed in the console.
 * @param init_val Initial default value.
 * @param min_val Minimum clamped value.
 * @param max_val Maximum clamped value.
 * @param on_change Optional callback fired on value update.
 */
WOTLK_C_API void wotlk_cvar_register_int(
    const char* name,
    const char* desc,
    int init_val,
    int min_val,
    int max_val,
    wotlk_cvar_change_int_fn on_change
);

/**
 * @brief Registers a custom floating point console variable with min/max bounds.
 * @param name CVar name string.
 * @param desc Helpful description displayed in the console.
 * @param init_val Initial default value.
 * @param min_val Minimum clamped value.
 * @param max_val Maximum clamped value.
 * @param on_change Optional callback fired on value update.
 */
WOTLK_C_API void wotlk_cvar_register_float(
    const char* name,
    const char* desc,
    float init_val,
    float min_val,
    float max_val,
    wotlk_cvar_change_float_fn on_change
);

/** @brief Reads an integer CVar value. */
WOTLK_C_API int   wotlk_cvar_get_int(const char* name);

/** @brief Reads a floating point CVar value. */
WOTLK_C_API float wotlk_cvar_get_float(const char* name);

/** @brief Sets a CVar string value. */
WOTLK_C_API void  wotlk_cvar_set(const char* name, const char* value);

/** @brief Triggers a refresh notification for a CVar. */
WOTLK_C_API void  wotlk_cvar_refresh(const char* name);

/* ========================================================================= */
/* Game & Engine Queries                                                     */
/* ========================================================================= */

/** @brief 64-bit entity GUID type in pure C. */
typedef uint64_t wotlk_guid_t;

/** @brief Retrieves the 64-bit GUID of the local player. */
WOTLK_C_API wotlk_guid_t wotlk_get_player_guid(void);

/** @brief Retrieves the 64-bit GUID of the current target. */
WOTLK_C_API wotlk_guid_t wotlk_get_target_guid(void);

/** @brief Checks if the player is currently in the game world. */
WOTLK_C_API int          wotlk_is_in_world(void);

/** @brief Checks if the player is in combat lockdown. */
WOTLK_C_API int          wotlk_in_combat_lockdown(void);

/** @brief Retrieves the game window handle (HWND). */
WOTLK_C_API void*        wotlk_get_game_window(void);

/** @brief Retrieves the asynchronous millisecond timestamp. */
WOTLK_C_API uint64_t     wotlk_get_async_time_ms(void);

/* ========================================================================= */
/* Typed Modules: SCREAMING_SNAKE_CASE API for Pure C Modders                */
/* ========================================================================= */

/* --- PLAYER Module --- */

/** @brief Retrieves the 64-bit GUID of the player character. */
WOTLK_C_API wotlk_guid_t PLAYER_GET_UID(void);

/** @brief Alias for PLAYER_GET_UID(). */
WOTLK_C_API wotlk_guid_t PLAYER_GET_GUID(void);

/** @brief Retrieves the 64-bit GUID of the active target entity. */
WOTLK_C_API wotlk_guid_t PLAYER_GET_TARGET_UID(void);

/** @brief Alias for PLAYER_GET_TARGET_UID(). */
WOTLK_C_API wotlk_guid_t PLAYER_GET_TARGET_GUID(void);

/** @brief Retrieves active character name string. */
WOTLK_C_API const char*  PLAYER_GET_NAME(void);

/** @brief Retrieves character level (1-80). */
WOTLK_C_API uint32_t     PLAYER_GET_LEVEL(void);

/** @brief Retrieves numeric race ID. */
WOTLK_C_API uint8_t      PLAYER_GET_RACE(void);

/** @brief Retrieves race display string (e.g., "Human", "Orc"). */
WOTLK_C_API const char*  PLAYER_GET_RACE_NAME(void);

/** @brief Retrieves numeric class ID. */
WOTLK_C_API uint8_t      PLAYER_GET_CLASS(void);

/** @brief Retrieves class display string (e.g., "Paladin"). */
WOTLK_C_API const char*  PLAYER_GET_CLASS_NAME(void);

/** @brief Retrieves gender ID (0=Male, 1=Female). */
WOTLK_C_API uint8_t      PLAYER_GET_GENDER(void);

/** @brief Retrieves gender display string ("Male" or "Female"). */
WOTLK_C_API const char*  PLAYER_GET_GENDER_NAME(void);

/** @brief Retrieves current health points. */
WOTLK_C_API uint32_t     PLAYER_GET_HEALTH(void);

/** @brief Retrieves maximum health points. */
WOTLK_C_API uint32_t     PLAYER_GET_MAX_HEALTH(void);

/** @brief Retrieves power points for a given power type index. */
WOTLK_C_API uint32_t     PLAYER_GET_POWER(uint32_t index);

/** @brief Retrieves maximum power points for a given power type index. */
WOTLK_C_API uint32_t     PLAYER_GET_MAX_POWER(uint32_t index);

/** @brief Retrieves current mana points. */
WOTLK_C_API uint32_t     PLAYER_GET_MANA(void);

/** @brief Retrieves maximum mana points. */
WOTLK_C_API uint32_t     PLAYER_GET_MAX_MANA(void);

/** @brief Retrieves 3D position coordinates {x, y, z}. */
WOTLK_C_API void         PLAYER_GET_POSITION(float* out_x, float* out_y, float* out_z);

/** @brief Retrieves player facing angle in radians. */
WOTLK_C_API float        PLAYER_GET_FACING(void);

/** @brief Checks if player is in combat lockdown. */
WOTLK_C_API int          PLAYER_IS_IN_COMBAT(void);

/** @brief Checks if player is alive (not dead, not ghost). */
WOTLK_C_API int          PLAYER_IS_ALIVE(void);

/** @brief Checks if player is dead. */
WOTLK_C_API int          PLAYER_IS_DEAD(void);

/** @brief Checks if player is in ghost form. */
WOTLK_C_API int          PLAYER_IS_GHOST(void);

/** @brief Sets target entity by GUID. */
WOTLK_C_API void         PLAYER_SET_TARGET(wotlk_guid_t guid);

/** @brief Clears active target selection. */
WOTLK_C_API void         PLAYER_CLEAR_TARGET(void);

/* --- SPELL Module --- */

/** @brief Retrieves currently casting or pending spell ID. */
WOTLK_C_API uint32_t     SPELL_GET_ID(void);

/** @brief Checks if cursor is in AoE targeting mode. */
WOTLK_C_API int          SPELL_IS_TARGETING_AOE(void);

/** @brief Cancels AoE targeting mode. */
WOTLK_C_API int          SPELL_CANCEL_AOE_TARGETING(void);

/** @brief Casts a spell by ID on target. */
WOTLK_C_API int          SPELL_CAST(uint32_t spell_id, wotlk_guid_t target);

/* --- GAME Module --- */

/** @brief Retrieves engine state ID (0=Login, 1=CharSelect, 2=InGame). */
WOTLK_C_API uint32_t     GAME_GET_STATE(void);

/** @brief Retrieves engine state display name string. */
WOTLK_C_API const char*  GAME_GET_STATE_NAME(void);

/** @brief Checks if player is in world. */
WOTLK_C_API int          GAME_IS_IN_WORLD(void);

/** @brief Checks if loading screen is active. */
WOTLK_C_API int          GAME_IS_LOADING(void);

/** @brief Retrieves game HWND window handle. */
WOTLK_C_API void*        GAME_GET_WINDOW(void);

/** @brief Retrieves async timer in milliseconds. */
WOTLK_C_API uint64_t     GAME_GET_TIME_MS(void);

/* --- ZONE Module --- */

/** @brief Retrieves zone text (e.g., "Dalaran"). */
WOTLK_C_API const char*  ZONE_GET_TEXT(void);

/** @brief Alias for ZONE_GET_TEXT(). */
WOTLK_C_API const char*  ZONE_GET_ZONE_TEXT(void);

/** @brief Retrieves subzone text (e.g., "Runeweaver Square"). */
WOTLK_C_API const char*  ZONE_GET_SUBZONE_TEXT(void);

/** @brief Retrieves minimap zone text. */
WOTLK_C_API const char*  ZONE_GET_MINIMAP_ZONE_TEXT(void);

/** @brief Retrieves Area ID from AreaTable.dbc. */
WOTLK_C_API uint32_t     ZONE_GET_AREA_ID(void);

/** @brief Retrieves Map ID from Map.dbc. */
WOTLK_C_API uint32_t     ZONE_GET_MAP_ID(void);

/** @brief Retrieves internal map folder name. */
WOTLK_C_API const char*  ZONE_GET_MAP_NAME(void);

/** @brief Retrieves continent display name (e.g., "Northrend"). */
WOTLK_C_API const char*  ZONE_GET_CONTINENT_NAME(void);

/* --- PRESENCE Module --- */

/**
 * @brief Live snapshot of game, character, and geographic state for rich presence integration.
 */
typedef struct wotlk_rich_presence_t {
    uint32_t    game_state;      /**< Numeric engine state code (0=Login, 1=CharSelect, 2=InGame). */
    const char* game_state_name; /**< State description string. */
    const char* character_name;  /**< Character name. */
    uint32_t    level;           /**< Level (1-80). */
    const char* race_name;       /**< Race name string. */
    const char* class_name;      /**< Class name string. */
    const char* zone_text;       /**< Zone name. */
    const char* subzone_text;    /**< Subzone name. */
    const char* continent_name;  /**< Continent name. */
    const char* map_name;        /**< Map directory name. */
    uint32_t    map_id;          /**< Map ID. */
    uint32_t    area_id;         /**< Area ID. */
    int         is_in_world;     /**< 1 if in world, 0 otherwise. */
    int         is_loading;      /**< 1 if loading screen active, 0 otherwise. */
    int         is_in_combat;    /**< 1 if in combat lockdown, 0 otherwise. */
} wotlk_rich_presence_t;

/**
 * @brief Captures an atomic snapshot of current client state.
 * @param out_data Output pointer to wotlk_rich_presence_t structure.
 */
WOTLK_C_API void PRESENCE_GET_DATA(wotlk_rich_presence_t* out_data);

/* --- CVAR Module --- */
#define CVAR_GET_INT             wotlk_cvar_get_int
#define CVAR_GET_FLOAT           wotlk_cvar_get_float
#define CVAR_SET                 wotlk_cvar_set
#define CVAR_REFRESH             wotlk_cvar_refresh
#define CVAR_REGISTER_INT        wotlk_cvar_register_int
#define CVAR_REGISTER_FLOAT      wotlk_cvar_register_float

/* --- LUA Module --- */

/**
 * @brief Executes a snippet of Lua script in the game client environment.
 * @param script Null-terminated script buffer.
 * @return int 1 on success, 0 on failure.
 */
WOTLK_C_API int  LUA_EXECUTE(const char* script);

/* --- AUTOLOGIN Module --- */
#define AUTOLOGIN_CONFIGURE      wotlk_autologin_configure
#define AUTOLOGIN_SET_LOGIN      wotlk_autologin_set_login
#define AUTOLOGIN_SET_PASSWORD   wotlk_autologin_set_password
#define AUTOLOGIN_SET_REALMLIST  wotlk_autologin_set_realmlist
#define AUTOLOGIN_SET_REALM_NAME wotlk_autologin_set_realm_name
#define AUTOLOGIN_SET_CHARACTER  wotlk_autologin_set_character
#define AUTOLOGIN_GET_LOGIN      wotlk_autologin_get_login
#define AUTOLOGIN_GET_PASSWORD   wotlk_autologin_get_password
#define AUTOLOGIN_GET_REALMLIST  wotlk_autologin_get_realmlist
#define AUTOLOGIN_GET_REALM_NAME wotlk_autologin_get_realm_name
#define AUTOLOGIN_GET_CHARACTER  wotlk_autologin_get_character

/* --- DISCORD Module --- */
#ifndef DISCORD_CLIENT_ID
#define DISCORD_CLIENT_ID ""
#endif

/**
 * @brief Starts the threaded Discord Rich Presence background worker in pure C.
 * @param client_id Discord Application Client ID string (pass NULL to use DISCORD_CLIENT_ID).
 * @param interval_ms Refresh interval in milliseconds.
 * @return int 1 on success, 0 on failure.
 */
WOTLK_C_API int  wotlk_discord_start(const char* client_id, uint32_t interval_ms);

/**
 * @brief Stops the threaded Discord Rich Presence worker in pure C.
 */
WOTLK_C_API void wotlk_discord_stop(void);

/**
 * @brief Forces an immediate presence update across the Discord named pipe in pure C.
 */
WOTLK_C_API void wotlk_discord_update(void);

/**
 * @brief Checks if Discord Rich Presence is currently connected in pure C.
 * @return int 1 if connected, 0 otherwise.
 */
WOTLK_C_API int   wotlk_discord_is_connected(void);

/**
 * @brief Retrieves the active Discord Social SDK (Game SDK) Core pointer in pure C.
 * @return void* pointer to discord::Core if active, or NULL.
 */
WOTLK_C_API void* wotlk_discord_get_core(void);

#define DISCORD_START            wotlk_discord_start
#define DISCORD_STOP             wotlk_discord_stop
#define DISCORD_UPDATE           wotlk_discord_update
#define DISCORD_IS_CONNECTED     wotlk_discord_is_connected
#define DISCORD_GET_CORE         wotlk_discord_get_core

/* ========================================================================= */
/* SCREAMING_SNAKE_CASE Macro Aliases for Legacy/Standard Functions          */
/* ========================================================================= */
#define WOTLK_SDK_INITIALIZE           wotlk_sdk_initialize
#define WOTLK_SDK_IS_INITIALIZED       wotlk_sdk_is_initialized
#define WOTLK_SDK_SHUTDOWN             wotlk_sdk_shutdown
#define WOTLK_AUTOLOGIN_CONFIGURE      wotlk_autologin_configure
#define WOTLK_AUTOLOGIN_SET_LOGIN      wotlk_autologin_set_login
#define WOTLK_AUTOLOGIN_SET_PASSWORD   wotlk_autologin_set_password
#define WOTLK_AUTOLOGIN_SET_REALMLIST  wotlk_autologin_set_realmlist
#define WOTLK_AUTOLOGIN_SET_REALM_NAME wotlk_autologin_set_realm_name
#define WOTLK_AUTOLOGIN_SET_CHARACTER  wotlk_autologin_set_character
#define WOTLK_DISCORD_START            wotlk_discord_start
#define WOTLK_DISCORD_STOP             wotlk_discord_stop
#define WOTLK_DISCORD_UPDATE           wotlk_discord_update
#define WOTLK_DISCORD_IS_CONNECTED     wotlk_discord_is_connected
#define WOTLK_DISCORD_GET_CORE         wotlk_discord_get_core
#define WOTLK_ON_UPDATE_ADD            wotlk_on_update_add
#define WOTLK_ON_ENTER_ADD             wotlk_on_enter_add
#define WOTLK_ON_LEAVE_ADD             wotlk_on_leave_add
#define WOTLK_ON_POST_LOAD_ADD         wotlk_on_post_load_add
#define WOTLK_ON_CHAR_ENUM_ADD         wotlk_on_char_enum_add
#define WOTLK_CVAR_REGISTER_INT        wotlk_cvar_register_int
#define WOTLK_CVAR_REGISTER_FLOAT      wotlk_cvar_register_float
#define WOTLK_CVAR_GET_INT             wotlk_cvar_get_int
#define WOTLK_CVAR_GET_FLOAT           wotlk_cvar_get_float
#define WOTLK_CVAR_SET                 wotlk_cvar_set
#define WOTLK_GET_PLAYER_GUID          wotlk_get_player_guid
#define WOTLK_GET_TARGET_GUID          wotlk_get_target_guid
#define WOTLK_IS_IN_WORLD              wotlk_is_in_world
#define WOTLK_IN_COMBAT_LOCKDOWN       wotlk_in_combat_lockdown
#define WOTLK_GET_GAME_WINDOW          wotlk_get_game_window
#define WOTLK_GET_ASYNC_TIME_MS        wotlk_get_async_time_ms

#ifdef __cplusplus
}
#endif

#endif /* WOTLK_SDK_C_H */
