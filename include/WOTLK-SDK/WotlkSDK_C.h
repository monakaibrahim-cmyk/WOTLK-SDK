#ifndef WOTLK_SDK_C_H
#define WOTLK_SDK_C_H

/**
 * @file WotlkSDK_C.h
 * @brief Native Pure C API for WotlkSDK (World of Warcraft 3.3.5a build 12340).
 *
 * This header can be directly included in pure C (C99/C11/C17/C23) DLL projects.
 * It exposes an extern "C" ABI with no C++ runtime or STL dependencies.
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
#define WOTLK_SDK_C_VERSION_MAJOR 1
#define WOTLK_SDK_C_VERSION_MINOR 0
#define WOTLK_SDK_C_VERSION_PATCH 0
#define WOTLK_SDK_C_VERSION_STRING "1.0.0"

#define WOTLK_SDK_C_TARGET_WOW_BUILD 12340
#define WOTLK_SDK_C_TARGET_WOW_VERSION "3.3.5a"

WOTLK_C_API const char* wotlk_sdk_get_version_string(void);
WOTLK_C_API uint32_t    wotlk_sdk_get_version_major(void);
WOTLK_C_API uint32_t    wotlk_sdk_get_version_minor(void);
WOTLK_C_API uint32_t    wotlk_sdk_get_version_patch(void);
WOTLK_C_API uint32_t    wotlk_sdk_get_target_wow_build(void);

/* ========================================================================= */
/* Core Lifecycle API                                                        */
/* ========================================================================= */

/**
 * @brief Initializes WotlkSDK, patches engine pointers, and installs core hooks.
 * @param retry_budget_ms Timeout for hook installation in milliseconds (default: 5000).
 * @return 1 on success, 0 on failure.
 */
WOTLK_C_API int wotlk_sdk_initialize(uint32_t retry_budget_ms);

/**
 * @brief Checks if WotlkSDK has already been initialized in this process.
 * @return 1 if initialized, 0 otherwise.
 */
WOTLK_C_API int wotlk_sdk_is_initialized(void);

/**
 * @brief Shuts down background workers and releases SDK resources.
 */
WOTLK_C_API void wotlk_sdk_shutdown(void);

/* ========================================================================= */
/* Auto-Login API                                                            */
/* ========================================================================= */

/**
 * @brief Programmatically configures auto-login credentials and destination realm.
 *
 * Precedence hierarchy:
 * 1. Programmatic API (this method)
 * 2. Command-line flags (-login, -password, -realmlist, -realmname, -character)
 * 3. Environment variables (WOTLK_LOGIN, WOTLK_PASSWORD, etc.)
 * 4. Game defaults (realmlist.wtf and WTF/Config.wtf)
 */
WOTLK_C_API void wotlk_autologin_configure(
    const char* login,
    const char* password,
    const char* realmlist,
    const char* realm_name,
    const char* character
);

WOTLK_C_API void wotlk_autologin_set_login(const char* login);
WOTLK_C_API void wotlk_autologin_set_password(const char* password);
WOTLK_C_API void wotlk_autologin_set_realmlist(const char* realmlist);
WOTLK_C_API void wotlk_autologin_set_realm_name(const char* realm_name);
WOTLK_C_API void wotlk_autologin_set_character(const char* character);

WOTLK_C_API const char* wotlk_autologin_get_login(void);
WOTLK_C_API const char* wotlk_autologin_get_password(void);
WOTLK_C_API const char* wotlk_autologin_get_realmlist(void);
WOTLK_C_API const char* wotlk_autologin_get_realm_name(void);
WOTLK_C_API const char* wotlk_autologin_get_character(void);

/* ========================================================================= */
/* Lifecycle Callback System                                                 */
/* ========================================================================= */
typedef void (*wotlk_callback_fn)(void);

WOTLK_C_API void wotlk_on_update_add(wotlk_callback_fn callback);
WOTLK_C_API void wotlk_on_enter_add(wotlk_callback_fn callback);
WOTLK_C_API void wotlk_on_leave_add(wotlk_callback_fn callback);
WOTLK_C_API void wotlk_on_post_load_add(wotlk_callback_fn callback);
WOTLK_C_API void wotlk_on_char_enum_add(wotlk_callback_fn callback);

/* ========================================================================= */
/* Console & CVar System                                                     */
/* ========================================================================= */
typedef void (*wotlk_cvar_change_int_fn)(int new_value, int changed);
typedef void (*wotlk_cvar_change_float_fn)(float new_value, int changed);

WOTLK_C_API void wotlk_cvar_register_int(
    const char* name,
    const char* desc,
    int init_val,
    int min_val,
    int max_val,
    wotlk_cvar_change_int_fn on_change
);

WOTLK_C_API void wotlk_cvar_register_float(
    const char* name,
    const char* desc,
    float init_val,
    float min_val,
    float max_val,
    wotlk_cvar_change_float_fn on_change
);

WOTLK_C_API int   wotlk_cvar_get_int(const char* name);
WOTLK_C_API float wotlk_cvar_get_float(const char* name);
WOTLK_C_API void  wotlk_cvar_set(const char* name, const char* value);
WOTLK_C_API void  wotlk_cvar_refresh(const char* name);

/* ========================================================================= */
/* Game & Engine Queries                                                     */
/* ========================================================================= */
typedef uint64_t wotlk_guid_t;

WOTLK_C_API wotlk_guid_t wotlk_get_player_guid(void);
WOTLK_C_API wotlk_guid_t wotlk_get_target_guid(void);
WOTLK_C_API int          wotlk_is_in_world(void);
WOTLK_C_API int          wotlk_in_combat_lockdown(void);
WOTLK_C_API void*        wotlk_get_game_window(void);
WOTLK_C_API uint64_t     wotlk_get_async_time_ms(void);

/* ========================================================================= */
/* Typed Modules: SCREAMING_SNAKE_CASE API for C Modders                      */
/* e.g., PLAYER_GET_UID(), SPELL_GET_ID(), GAME_GET_STATE(), ZONE_GET_TEXT()  */
/* ========================================================================= */

/* --- PLAYER Module --- */
WOTLK_C_API wotlk_guid_t PLAYER_GET_UID(void);
WOTLK_C_API wotlk_guid_t PLAYER_GET_GUID(void);
WOTLK_C_API wotlk_guid_t PLAYER_GET_TARGET_UID(void);
WOTLK_C_API wotlk_guid_t PLAYER_GET_TARGET_GUID(void);
WOTLK_C_API const char*  PLAYER_GET_NAME(void);
WOTLK_C_API uint32_t     PLAYER_GET_LEVEL(void);
WOTLK_C_API uint8_t      PLAYER_GET_RACE(void);
WOTLK_C_API const char*  PLAYER_GET_RACE_NAME(void);
WOTLK_C_API uint8_t      PLAYER_GET_CLASS(void);
WOTLK_C_API const char*  PLAYER_GET_CLASS_NAME(void);
WOTLK_C_API uint8_t      PLAYER_GET_GENDER(void);
WOTLK_C_API const char*  PLAYER_GET_GENDER_NAME(void);
WOTLK_C_API uint32_t     PLAYER_GET_HEALTH(void);
WOTLK_C_API uint32_t     PLAYER_GET_MAX_HEALTH(void);
WOTLK_C_API uint32_t     PLAYER_GET_POWER(uint32_t index);
WOTLK_C_API uint32_t     PLAYER_GET_MAX_POWER(uint32_t index);
WOTLK_C_API uint32_t     PLAYER_GET_MANA(void);
WOTLK_C_API uint32_t     PLAYER_GET_MAX_MANA(void);
WOTLK_C_API void         PLAYER_GET_POSITION(float* out_x, float* out_y, float* out_z);
WOTLK_C_API float        PLAYER_GET_FACING(void);
WOTLK_C_API int          PLAYER_IS_IN_COMBAT(void);
WOTLK_C_API int          PLAYER_IS_ALIVE(void);
WOTLK_C_API int          PLAYER_IS_DEAD(void);
WOTLK_C_API int          PLAYER_IS_GHOST(void);
WOTLK_C_API void         PLAYER_SET_TARGET(wotlk_guid_t guid);
WOTLK_C_API void         PLAYER_CLEAR_TARGET(void);

/* --- SPELL Module --- */
WOTLK_C_API uint32_t     SPELL_GET_ID(void);
WOTLK_C_API int          SPELL_IS_TARGETING_AOE(void);
WOTLK_C_API int          SPELL_CANCEL_AOE_TARGETING(void);
WOTLK_C_API int          SPELL_CAST(uint32_t spell_id, wotlk_guid_t target);

/* --- GAME Module --- */
WOTLK_C_API uint32_t     GAME_GET_STATE(void);
WOTLK_C_API const char*  GAME_GET_STATE_NAME(void);
WOTLK_C_API int          GAME_IS_IN_WORLD(void);
WOTLK_C_API int          GAME_IS_LOADING(void);
WOTLK_C_API void*        GAME_GET_WINDOW(void);
WOTLK_C_API uint64_t     GAME_GET_TIME_MS(void);

/* --- ZONE Module --- */
WOTLK_C_API const char*  ZONE_GET_TEXT(void);
WOTLK_C_API const char*  ZONE_GET_ZONE_TEXT(void);
WOTLK_C_API const char*  ZONE_GET_SUBZONE_TEXT(void);
WOTLK_C_API const char*  ZONE_GET_MINIMAP_ZONE_TEXT(void);
WOTLK_C_API uint32_t     ZONE_GET_AREA_ID(void);
WOTLK_C_API uint32_t     ZONE_GET_MAP_ID(void);
WOTLK_C_API const char*  ZONE_GET_MAP_NAME(void);
WOTLK_C_API const char*  ZONE_GET_CONTINENT_NAME(void);

/* --- PRESENCE Module (Discord Rich Presence Helper) --- */
typedef struct wotlk_rich_presence_t {
    uint32_t    game_state;
    const char* game_state_name;
    const char* character_name;
    uint32_t    level;
    const char* race_name;
    const char* class_name;
    const char* zone_text;
    const char* subzone_text;
    const char* continent_name;
    const char* map_name;
    uint32_t    map_id;
    uint32_t    area_id;
    int         is_in_world;
    int         is_loading;
    int         is_in_combat;
} wotlk_rich_presence_t;

WOTLK_C_API void         PRESENCE_GET_DATA(wotlk_rich_presence_t* out_data);

/* --- CVAR Module --- */
#define CVAR_GET_INT             wotlk_cvar_get_int
#define CVAR_GET_FLOAT           wotlk_cvar_get_float
#define CVAR_SET                 wotlk_cvar_set
#define CVAR_REFRESH             wotlk_cvar_refresh
#define CVAR_REGISTER_INT        wotlk_cvar_register_int
#define CVAR_REGISTER_FLOAT      wotlk_cvar_register_float

/* --- LUA Module --- */
WOTLK_C_API int          LUA_EXECUTE(const char* script);

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

WOTLK_C_API int  wotlk_discord_start(const char* client_id, uint32_t interval_ms);
WOTLK_C_API void wotlk_discord_stop(void);
WOTLK_C_API void wotlk_discord_update(void);
WOTLK_C_API int  wotlk_discord_is_connected(void);

#define DISCORD_START            wotlk_discord_start
#define DISCORD_STOP             wotlk_discord_stop
#define DISCORD_UPDATE           wotlk_discord_update
#define DISCORD_IS_CONNECTED     wotlk_discord_is_connected

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
