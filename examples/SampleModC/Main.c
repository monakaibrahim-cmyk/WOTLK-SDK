#include <windows.h>
#include <WotlkSDK_C.h>

/**
 * @brief Sample Pure C WoW 3.3.5a Mod DLL using WotlkSDK Native C API.
 *
 * This sample is compiled with a pure C compiler (C99/C11/C17/C23) without any C++
 * runtime or STL dependencies.
 */

static void on_frame_update(void) {
    /* Runs on every engine paint/render frame tick */
    if (GAME_IS_IN_WORLD()) {
        wotlk_guid_t my_uid = PLAYER_GET_UID();
        uint32_t pending_spell = SPELL_GET_ID();
        const char* zone = ZONE_GET_ZONE_TEXT();
        (void)my_uid;
        (void)pending_spell;
        (void)zone;
    }
}

static void on_world_enter(void) {
    /* Runs when player enters the world */
    wotlk_guid_t player_guid = PLAYER_GET_UID();
    const char* player_name = PLAYER_GET_NAME();
    uint32_t player_level = PLAYER_GET_LEVEL();
    (void)player_guid;
    (void)player_name;
    (void)player_level;

    /* Discord Rich Presence snapshot */
    wotlk_rich_presence_t presence;
    PRESENCE_GET_DATA(&presence);
}

static void on_world_leave(void) {
    /* Runs when player logs out */
}

static void on_cvar_change(int new_value, int changed) {
    if (changed) {
        /* Handle runtime CVar change in pure C */
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    (void)lpReserved;

    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        /* 1. Optional: Programmatically configure AutoLogin in pure C */
        /* wotlk_autologin_configure("username", "password", "logon.warmane.com", "Lordaeron", "MyCharacter"); */

        /* 2. Initialize WotlkSDK (installs core engine hooks) */
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
    }

    return TRUE;
}
