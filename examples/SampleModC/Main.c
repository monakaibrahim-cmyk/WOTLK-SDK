/**
 * @file Main.c
 * @brief Reference pure C mod implementation demonstrating WotlkSDK Native C API.
 *
 * Demonstrates:
 * - Pure C compilation without C++ runtime or standard library dependencies
 * - Engine lifecycle initialization and shutdown via C bindings
 * - Frame update and world enter/leave callbacks
 * - Console CVar registration and callbacks in C
 * - Discord Rich Presence queries and control in C
 */

#include <windows.h>
#include <WOTLK-SDK/wotlkc.h>

/**
 * @brief Frame update callback executing on each engine render tick.
 */
static void on_frame_update(void) {
    if (GAME_IS_IN_WORLD()) {
        wotlk_guid_t my_uid = PLAYER_GET_UID();
        uint32_t pending_spell = SPELL_GET_ID();
        const char* zone = ZONE_GET_ZONE_TEXT();
        (void)my_uid;
        (void)pending_spell;
        (void)zone;
    }
}

/**
 * @brief Dispatched when the local player character enters the game world.
 */
static void on_world_enter(void) {
    wotlk_guid_t player_guid = PLAYER_GET_UID();
    const char* player_name = PLAYER_GET_NAME();
    uint32_t player_level = PLAYER_GET_LEVEL();
    (void)player_guid;
    (void)player_name;
    (void)player_level;

    /* Snapshot current state for rich presence */
    wotlk_rich_presence_t presence;
    PRESENCE_GET_DATA(&presence);
}

/**
 * @brief Dispatched when the player character logs out or leaves the world.
 */
static void on_world_leave(void) {
}

/**
 * @brief Callback fired when the custom console variable is modified.
 */
static void on_cvar_change(int new_value, int changed) {
    if (changed) {
        /* Process runtime CVar change */
    }
}

/**
 * @brief Dynamic link library entry point for the pure C mod.
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    (void)lpReserved;

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        /* 1. (Optional) Programmatically configure AutoLogin in pure C */
        /* wotlk_autologin_configure("username", "password", "logon.example.com", "Lordaeron", "MyCharacter"); */

        /* 2. Initialize WotlkSDK hooks */
        if (!wotlk_sdk_initialize(5000)) {
            return FALSE;
        }

        /* 3. Register lifecycle callbacks */
        wotlk_on_update_add(on_frame_update);
        wotlk_on_enter_add(on_world_enter);
        wotlk_on_leave_add(on_world_leave);

        /* 4. Register custom console CVar in pure C */
        wotlk_cvar_register_int(
            "sample_c_mod_enabled",
            "Toggles Sample C Mod (1 = enabled, 0 = disabled)",
            1,
            0,
            1,
            on_cvar_change
        );

        /* 5. (Optional) Start threaded Discord Rich Presence in pure C */
        /* DISCORD_START("123456789012345678", 5000); */
    } else if (reason == DLL_PROCESS_DETACH) {
        wotlk_sdk_shutdown();
    }

    return TRUE;
}
