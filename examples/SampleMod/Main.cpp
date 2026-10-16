#include <windows.h>
#include <WOTLK-SDK/wotlkcpp.h>

/**
 * @brief Sample WoW 3.3.5a Mod DLL demonstrating WotlkSDK usage.
 *
 * This sample shows:
 * 1. Programmatic AutoLogin configuration
 * 2. WotlkSDK lifecycle initialization
 * 3. Registering custom console CVars
 * 4. Listening to game loop update ticks
 * 5. Listening to world enter and leave events
 */

void onWorldUpdate() {
    // This callback runs on every engine frame tick
    if (GAME::IS_IN_WORLD()) {
        guid_t myGuid = PLAYER::GET_UID();
        uint32_t pendingSpell = SPELL::GET_ID();
        const char* zone = ZONE::GET_ZONE_TEXT();
        (void)myGuid;
        (void)pendingSpell;
        (void)zone;
    }
}

void onWorldEnter() {
    // Runs when the local player enters the world (after loading screen)
    guid_t playerGuid = PLAYER::GET_UID();
    const char* playerName = PLAYER::GET_NAME();
    uint32_t playerLevel = PLAYER::GET_LEVEL();
    (void)playerGuid;
    (void)playerName;
    (void)playerLevel;

    // Example Discord Rich Presence query
    auto presence = PRESENCE::GET_DATA();
    (void)presence;
}

void onWorldLeave() {
    // Runs when the player logs out or leaves the world
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // 1. (Optional) Programmatically configure auto-login credentials
        // If not set here, AutoLogin will fall back to command line flags (-login, -password),
        // environment variables (WOTLK_LOGIN, WOTLK_PASSWORD), or WoW default realmlist.wtf.
        // WotlkSDK::AutoLogin::Configure("username", "password", "logon.warmane.com", "Lordaeron", "MyCharacter");

        // 2. Initialize the SDK and install engine hooks
        if (!WotlkSDK::Initialize()) {
            return FALSE;
        }

        // 3. Register engine lifecycle callbacks
        WotlkSDK::ON_UPDATE->add(onWorldUpdate);
        WotlkSDK::ON_ENTER->add(onWorldEnter);
        WotlkSDK::ON_LEAVE->add(onWorldLeave);

        // 4. Register a custom console CVar with automatic type checking and callbacks
        WotlkSDK::CVAR_REGISTRY->add<int>({
            .name = "sample_mod_enabled",
            .desc = "Toggles SampleMod features (1 = enabled, 0 = disabled)",
            .init = 1,
            .on_change = [](int newValue, bool changed) {
                if (changed) {
                    // Handle runtime CVar change
                }
            }
        });
    }

    return TRUE;
}
